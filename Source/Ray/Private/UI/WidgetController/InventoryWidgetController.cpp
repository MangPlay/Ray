// RAI Project 개발자 : Nam Chan woo


#include "UI/WidgetController/InventoryWidgetController.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InteractionSystem/Components/InteractionManagerComponent.h"
#include "InteractionSystem/Components/InteractionTargetComponent.h"
#include "InventorySystem/Actors/Capture/PlayerCapture.h"
#include "InventorySystem/Components/PlayerEquipmentComponent.h"


#include "InventorySystem/Components/PlayerInventoryComponent.h"
#include "InventorySystem/Libraries/InventoryFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "UI/WidgetSystemLibrary.h"

#include "UI/HUD/RayHUD.h"
#include "UI/Widget/RayOverlayWidget.h"
#include "Utilities/RayLog.h"

void UInventoryWidgetController::BroadcastInitialValues()
{
	PlayerInventory = Cast<UPlayerInventoryComponent>(PlayerController->GetComponentByClass(UPlayerInventoryComponent::StaticClass()));
	if (!PlayerInventory) RAY_LOG_SCREEN_ERROR(TEXT("Player Inventory를 초기화 하지 못했습니다."))

	PlayerEquipment = Cast<UPlayerEquipmentComponent>(PlayerController->GetComponentByClass(UPlayerEquipmentComponent::StaticClass()));
	if (!PlayerEquipment) RAY_LOG_SCREEN_ERROR(TEXT("PlayerEquipment를 초기화 하지 못했습니다."))

	InitializePlayerCapture();
}

void UInventoryWidgetController::BindCallbacksToDependencies()
{
}

void UInventoryWidgetController::InputCloseWidget()
{
	if (bIsInteractableActorWidgetOpen)
	{
		if (UInteractionManagerComponent* InteractionManager = Cast<UInteractionManagerComponent>(PlayerController->GetComponentByClass(UInteractionManagerComponent::StaticClass())))
		{
			if (InteractionManager->BestInteractionTarget->InteractionType == EInteractionType::Tap)
			{
				InteractionManager->ServerOnInteractionFinished(InteractionManager->BestInteractionTarget, EInteractionResult::Completed);
			}
		}
	}
	else
	{
		CloseActiveWidget();
	}
}

void UInventoryWidgetController::OpenNewWidget(EWidgetType Widget)
{
	switch (Widget)
	{
	case EWidgetType::Inventory:
		OpenInventoryWidget();
		break;

	case EWidgetType::Crafting:
		break;

	case EWidgetType::Vendor:
		OpenVendorWidget();
		break;

	case EWidgetType::Storage:
		OpenStorageWidget();
		break;

	case EWidgetType::LoadGame: break;

	default: break;;
	}
}

void UInventoryWidgetController::SwitchWidgetTo(EWidgetType NewWidget)
{
	if (ActiveWidget != NewWidget)
	{
		CloseActiveWidget();
		OpenNewWidget(NewWidget);
	}
}

void UInventoryWidgetController::SwitchTabTo(EWidgetType NewTab)
{
	if (ActiveTab != NewTab)
	{
		SetActiveTab(NewTab);
	}
}

void UInventoryWidgetController::SetActiveWidget(EWidgetType Widget)
{
	ActiveWidget = Widget;
	OnSwitchedWidget.Broadcast(Widget);
}

void UInventoryWidgetController::SetActiveTab(EWidgetType Tab)
{
	ActiveTab = Tab;
	OnSwitchedTab.Broadcast(Tab);
}

void UInventoryWidgetController::DisplayMessageNotify(const FString& InMessage)
{
}

void UInventoryWidgetController::CloseActivePopUp()
{
	ActivePopUp = EWidgetPopUp::None;

	if (IsValid(PopUpRef))
	{
		PopUpRef->RemoveFromParent();
	}
}

void UInventoryWidgetController::OpenConfirmationPopUp(const FString& QuestionText, const FItemData& ItemData, FItemData InSlotData, UInventoryCore* Sender, UInventoryCore* Receiver,
                                                       EInputMethod InputMethod, EItemDestination Initiator, EItemDestination Destination, UUserWidget* SenderWidget)
{
}

void UInventoryWidgetController::OpenSplitStackPopUp(const FItemData& ItemData, FItemData InSlotData, UInventoryCore* Sender, UInventoryCore* Receiver, EInputMethod InputMethod,
                                                     EItemDestination Initiator, EItemDestination Destination, UUserWidget* SenderWidget)
{
}

void UInventoryWidgetController::OpenTextDocumentPopUp(const FItemData& ItemData, UUserWidget* SenderWidget)
{
}

EWidgetType UInventoryWidgetController::GetActiveWidget()
{
	return ActiveWidget;
}

EWidgetPopUp UInventoryWidgetController::GetActivePopUp()
{
	return ActivePopUp;
}

EWidgetType UInventoryWidgetController::GetActiveTab()
{
	return ActiveTab;
}

void UInventoryWidgetController::InitializePlayerCapture()
{
	PlayerCapture = Cast<APlayerCapture>(UGameplayStatics::GetActorOfClass(this, APlayerCapture::StaticClass()));

	if (IsValid(PlayerCapture))
	{
		PlayerCapture->InitializePlayerCapture(PlayerEquipment);
	}
}

void UInventoryWidgetController::StartPlayerCapture()
{
	PlayerCapture->EventStartCapture();
}

void UInventoryWidgetController::StopPlayerCapture()
{
	PlayerCapture->EventEndCapture();
}

APlayerCapture* UInventoryWidgetController::GetPlayerCapture()
{
	return PlayerCapture;
}

void UInventoryWidgetController::CloseActiveWidget()
{
	switch (ActiveWidget)
	{
	case EWidgetType::Inventory:
		UWidgetSystemLibrary::GetInventoryWidgetController(this)->CloseInventoryWidget();
		break;

	case EWidgetType::Crafting:
		break;

	case EWidgetType::Vendor:
		UWidgetSystemLibrary::GetInventoryWidgetController(this)->CloseVendorWidget();
		break;

	case EWidgetType::Storage:
		UWidgetSystemLibrary::GetInventoryWidgetController(this)->CloseStorageWidget();
		break;

	case EWidgetType::LoadGame:
		break;

	default: break;;
	}
}

void UInventoryWidgetController::OpenInventoryWidget()
{
	PlaySoundOnOpeningWidget();

	SetActiveWidget(EWidgetType::Inventory);

	SetActiveTab(EWidgetType::Inventory);

	UInventoryFunctionLibrary::StartPlayerCapture2D(PlayerController);

	InventoryWidgetRef = CreateWidget<URayUserWidget>(PlayerController, InventoryWidgetClass);

	ActivateWidgetWithUIConfig(InventoryWidgetRef);
}

void UInventoryWidgetController::CloseInventoryWidget()
{
	SetActiveWidget(EWidgetType::None);

	SetActiveTab(EWidgetType::None);

	UInventoryFunctionLibrary::StopPlayerCapture2D(PlayerController);

	DeactivateWidgetWithUIReset(InventoryWidgetRef);

	PlayerInventory->ServerChangeDroppedIndex(0);
}

void UInventoryWidgetController::OpenLootBarWidget()
{
	PlaySoundOnOpeningWidget();

	bIsLootBarOpen = true;

	bIsInteractableActorWidgetOpen = true;

	ShowLootBar();

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, RayHUD->OverlayWidget->UW_LootBar, EMouseLockMode::DoNotLock, true);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetIgnoreMoveInput(true);
	PlayerController->SetIgnoreLookInput(true);
}

void UInventoryWidgetController::CloseLootBarWidget()
{
	bIsInteractableActorWidgetOpen = false;
	bIsLootBarOpen = false;

	HideLootBar();

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController, false);
	PlayerController->bShowMouseCursor = false;
	PlayerController->ResetIgnoreInputFlags();
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}

void UInventoryWidgetController::OpenVendorWidget()
{
	PlaySoundOnOpeningWidget();

	bIsInteractableActorWidgetOpen = true;

	SetActiveWidget(EWidgetType::Vendor);

	SetActiveTab(EWidgetType::Vendor);

	UInventoryFunctionLibrary::StartPlayerCapture2D(PlayerController);

	VendorWidgetRef = CreateWidget<URayUserWidget>(PlayerController, VendorWidgetClass);
	ActivateWidgetWithUIConfig(VendorWidgetRef);
}

void UInventoryWidgetController::CloseVendorWidget()
{
	bIsInteractableActorWidgetOpen = false;

	SetActiveWidget(EWidgetType::None);

	SetActiveTab(EWidgetType::None);

	DeactivateWidgetWithUIReset(VendorWidgetRef);
}

void UInventoryWidgetController::OpenStorageWidget()
{
	PlaySoundOnOpeningWidget();

	bIsInteractableActorWidgetOpen = true;

	SetActiveTab(EWidgetType::Storage);

	SetActiveTab(EWidgetType::Storage);

	UInventoryFunctionLibrary::StartPlayerCapture2D(PlayerController);

	StorageWidgetRef = CreateWidget<URayUserWidget>(PlayerController, StorageWidgetClass);

	ActivateWidgetWithUIConfig(StorageWidgetRef);
}

void UInventoryWidgetController::CloseStorageWidget()
{
	bIsInteractableActorWidgetOpen = false;

	SetActiveWidget(EWidgetType::None);

	SetActiveTab(EWidgetType::None);

	UInventoryFunctionLibrary::StopPlayerCapture2D(PlayerController);

	DeactivateWidgetWithUIReset(StorageWidgetRef);
}

void UInventoryWidgetController::ActivateWidgetWithUIConfig(URayUserWidget* OpeningWidget)
{
	if (!UKismetSystemLibrary::HasMultipleLocalPlayers(this))
	{
		OpeningWidget->AddToViewport(1);
	}
	else
	{
		OpeningWidget->AddToPlayerScreen(1);
	}

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, OpeningWidget,
	                                                  EMouseLockMode::DoNotLock, true);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetIgnoreMoveInput(true);
	PlayerController->SetIgnoreLookInput(true);
}

void UInventoryWidgetController::DeactivateWidgetWithUIReset(URayUserWidget* ClosingWidget)
{
	if (IsValid(ClosingWidget))
	{
		ClosingWidget->RemoveFromParent();
	}

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController, false);
	PlayerController->bShowMouseCursor = false;
	PlayerController->ResetIgnoreInputFlags();
}

void UInventoryWidgetController::PlaySoundOnOpeningWidget()
{
	UGameplayStatics::PlaySound2D(this, InventoryOpenWidgetSound);
}

void UInventoryWidgetController::PlaySoundOnItemPickedUp()
{
	UGameplayStatics::PlaySound2D(this, PickupSound);
}

void UInventoryWidgetController::PlaySoundOnTabSwitched()
{
	UGameplayStatics::PlaySound2D(this, TabSwitchedSound);
}

void UInventoryWidgetController::PlaySoundOnItemDropped()
{
	UGameplayStatics::PlaySound2D(this, ItemDroppedSound);
}
