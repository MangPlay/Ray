// RAI Project 개발자 : Nam Chan woo


#include "InventorySystem/Components/PlayerInventoryComponent.h"
#include "InventorySystem/Components/PlayerInventoryComponent.h"


#include "InteractionSystem/Components/InteractionManagerComponent.h"
#include "InteractionSystem/Components/InteractionTargetComponent.h"
#include "InventorySystem/Components/InteractableComponent.h"
#include "InventorySystem/Components/PlayerEquipmentComponent.h"


#include "InventorySystem/Components/InteractionComponent.h"

#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "InventorySystem/Libraries/InventoryFunctionLibrary.h"

#include "InventorySystem/Objects/MainItem.h"

#include "UI/WidgetSystemLibrary.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "Utilities/RayLog.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	Inventories.Add(EInventoryPanels::P1, FInventory());
	Inventories.Add(EInventoryPanels::P2, FInventory());
	Inventories.Add(EInventoryPanels::P3, FInventory());
	Inventories.Add(EInventoryPanels::P4, FInventory());

	bCheckOwnerGold = true;
	bUseInventoryWeight = true;
	bCheckRemoveType = true;
	// ...
}

void UPlayerInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UPlayerInventoryComponent, DroppedIndex, COND_OwnerOnly);
}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UPlayerInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerInventoryComponent::InitializeInventory()
{
	Super::InitializeInventory();
}

void UPlayerInventoryComponent::BindEvents()
{
	// if (UInteractionComponent* OwnerInteractionComponent = UInventoryFunctionLibrary::GetInteractionComponent(GetOwner()))
	// {
	// 	 OwnerInteractionComponent->OnNewInteractableAssigned.AddDynamic(this, &UPlayerInventoryComponent::SetNewInteractable);
	// 	 OwnerInteractionComponent->OnRemovedCurrentInteractable.AddDynamic(this, &UPlayerInventoryComponent::RemoveCurrentInteractable);
	// }
}

void UPlayerInventoryComponent::SetNewInteractable(UInteractableComponent* Interactable)
{
	CurrentInteractable = Interactable;
}

void UPlayerInventoryComponent::RemoveCurrentInteractable()
{
	CurrentInteractable = nullptr;
}

void UPlayerInventoryComponent::ClientInitializeInventory()
{
	Super::ClientInitializeInventory();
	BindEvents();
}

void UPlayerInventoryComponent::ClientTakeItemReturnValue_Implementation(bool bSuccess, const FString& FailureMessage, bool bRemoveInteraction)
{
	if (bSuccess)
	{
		UWidgetSystemLibrary::GetInventoryWidgetController(this)->PlaySoundOnItemPickedUp();

		if (bRemoveInteraction)
		{
			APlayerController* OwnerPlayerController = Cast<APlayerController>(GetOwner());
			if (!OwnerPlayerController) return;
		
			if (UInteractionManagerComponent* InteractionManager = Cast<UInteractionManagerComponent>(OwnerPlayerController->GetComponentByClass(UInteractionManagerComponent::StaticClass())))
			{
				if (InteractionManager->BestInteractionTarget->InteractionResponse == EInteractionResponse::Persistent)
				{
					InteractionManager->ServerOnInteractionFinished(InteractionManager->BestInteractionTarget, EInteractionResult::Completed);
				}
			}
		}
	}
	else
	{
		UWidgetSystemLibrary::GetInventoryWidgetController(this)->DisplayMessageNotify(FailureMessage);
	}
}

void UPlayerInventoryComponent::InputInventory()
{
	UInventoryWidgetController* InventoryWidgetController = UWidgetSystemLibrary::GetInventoryWidgetController(this);

	if (InventoryWidgetController->bIsLootBarOpen)
	{
		APlayerController* OwnerPlayerController = Cast<APlayerController>(GetOwner());
		if (!OwnerPlayerController) return;
		
		if (UInteractionManagerComponent* InteractionManager = Cast<UInteractionManagerComponent>(OwnerPlayerController->GetComponentByClass(UInteractionManagerComponent::StaticClass())))
		{
			if (InteractionManager->BestInteractionTarget->InteractionResponse == EInteractionResponse::Persistent)
			{
				InteractionManager->ServerOnInteractionFinished(InteractionManager->BestInteractionTarget, EInteractionResult::Completed);
			}
		}
	}


	const EWidgetType ActiveWidgetType = UWidgetSystemLibrary::GetInventoryWidgetController(this)->GetActiveWidget();

	if (ActiveWidgetType == EWidgetType::Inventory)
	{
		InventoryWidgetController->CloseInventoryWidget();
	}
	else
	{
		if (InventoryWidgetController->bIsInteractableActorWidgetOpen)
		{
			APlayerController* OwnerPlayerController = Cast<APlayerController>(GetOwner());
			if (!OwnerPlayerController) return;
		
			if (UInteractionManagerComponent* InteractionManager = Cast<UInteractionManagerComponent>(OwnerPlayerController->GetComponentByClass(UInteractionManagerComponent::StaticClass())))
			{
				if (InteractionManager->BestInteractionTarget->InteractionResponse == EInteractionResponse::Persistent)
				{
					InteractionManager->ServerOnInteractionFinished(InteractionManager->BestInteractionTarget, EInteractionResult::Completed);
				}
			}
		}

		InventoryWidgetController->SwitchWidgetTo(EWidgetType::Inventory);
	}
}

void UPlayerInventoryComponent::InputTakeAll()
{
	UInventoryWidgetController* InventoryWidgetController = UWidgetSystemLibrary::GetInventoryWidgetController(this);
	if (InventoryWidgetController->bIsLootBarOpen)
	{
		OnTakeAllItems.Broadcast();
	}
}

bool UPlayerInventoryComponent::TransferItemFromInventory(FItemData& ItemData, const FItemData& InSlotData, EInputMethod Method, UInventoryCore* Sender, FText& OutFailureMessage)
{
	if (!ItemData.IsItemClassValid() || !IsValid(Sender))
	{
		OutFailureMessage = FText();
		return false;
	}

	if (ItemData.IsItemCurrency())
	{
		AddGoldToOwner(ItemData);
		RemoveItemFromInventoryArray(ItemData);
		return true;
	}

	if (Sender->bCheckOwnerGold && !HasEnoughGold(ItemData))
	{
		return false;
	}


	UPlayerEquipmentComponent* OwnerEquipment = Cast<UPlayerEquipmentComponent>(
		GetOwner()->GetComponentByClass(UPlayerEquipmentComponent::StaticClass()));
	if (IsValid(OwnerEquipment))
	{
		if (OwnerEquipment->TryToAddToPartialStack(ItemData))
		{
			if (Sender->bCheckOwnerGold && bCheckOwnerGold)
			{
				RemoveGoldFromOwner(ItemData);
				AddGoldToOwner(ItemData);
			}
			Sender->RemoveItemFromInventoryArray(ItemData);
			Sender->RemoveWeightFromInventory(ItemData);
			return true;
		}
	}

	const EInventoryPanels ItemPanel = ItemData.GetInventoryPanelFromItem();
	FInventory& InventoryRef = GetInventoryRefByPanel(ItemPanel);

	int32 StackSlotIndex;
	if (InventoryRef.HasPartialStack(ItemData, StackSlotIndex))
	{
		AddToStackInInventory(ItemData, StackSlotIndex);
		if (Sender->bCheckOwnerGold && bCheckOwnerGold)
		{
			RemoveGoldFromOwner(ItemData);
			AddGoldToOwner(ItemData);
		}
		Sender->RemoveItemFromInventoryArray(ItemData);
		Sender->RemoveWeightFromInventory(ItemData);
		return true;
	}

	if (Method == EInputMethod::RightClick)
	{
		int32 EmptySlotIndex;
		if (GetEmptyInventorySlot(ItemData, EmptySlotIndex))
		{
			AddItemToInventoryArray(ItemData, EmptySlotIndex);
		}
		else
		{
			OutFailureMessage = MessageInventoryFull;
			return false;
		}
	}
	else if (Method == EInputMethod::DragAndDrop)
	{
		AddItemToInventoryArray(ItemData, InSlotData.Index);
	}

	AddWeightToInventory(ItemData);
	if (Sender->bCheckOwnerGold && bCheckOwnerGold)
	{
		RemoveGoldFromOwner(ItemData);
		AddGoldToOwner(ItemData);
	}
	Sender->RemoveItemFromInventoryArray(ItemData);
	Sender->RemoveWeightFromInventory(ItemData);
	return true;
}

void UPlayerInventoryComponent::SplitItemsInInventory(UInventoryCore* Sender, const FItemData& ItemData,
                                                      const FItemData& InSlotData, const FItemData& StackableLeft,
                                                      EInputMethod InputMethod, EItemDestination Initiator,
                                                      EItemDestination Destination)
{
	if (Initiator == EItemDestination::InventorySlot)
	{
		if (Destination == EItemDestination::InventorySlot)
		{
			RemoveItemFromInventoryArray(ItemData);
			if (InSlotData.IsItemClassValid())
				AddToStackInInventory(const_cast<FItemData&>(ItemData), InSlotData.Index);
			else
				AddItemToInventoryArray(const_cast<FItemData&>(ItemData), InSlotData.Index);
		}
		if (Destination == EItemDestination::DropBar)
		{
			DropItemOnTheGround(ItemData, EItemDestination::InventorySlot);
		}

		if (UInventoryFunctionLibrary::IsStackableAndHaveStacks(StackableLeft, 0))
		{
			AddItemToInventoryArray(const_cast<FItemData&>(StackableLeft), StackableLeft.Index);
		}
		return;
	}

	if (Initiator == EItemDestination::EquipmentSlot)
	{
		UPlayerEquipmentComponent* OwnerEquipment = Cast<UPlayerEquipmentComponent>(
			GetOwner()->GetComponentByClass(UPlayerEquipmentComponent::StaticClass()));

		if (Destination == EItemDestination::InventorySlot)
		{
			OwnerEquipment->RemoveItemFromEquipmentArray(ItemData);
			if (InSlotData.IsItemClassValid())
				AddToStackInInventory(const_cast<FItemData&>(ItemData), InSlotData.Index);
			else
				AddItemToInventoryArray(const_cast<FItemData&>(ItemData), InSlotData.Index);
		}
		if (Destination == EItemDestination::DropBar)
		{
			DropItemOnTheGround(ItemData, EItemDestination::EquipmentSlot);
		}

		if (UInventoryFunctionLibrary::IsStackableAndHaveStacks(StackableLeft, 0))
		{
			OwnerEquipment->AddItemToEquipmentArray(StackableLeft, StackableLeft.EquipmentSlot);
		}
		return;
	}

	if (Initiator == EItemDestination::VendorSlot || Initiator == EItemDestination::StorageSlot)
	{
		FText FailureMessage;
		const bool bSuccess = TransferItemFromInventory(const_cast<FItemData&>(ItemData), InSlotData, InputMethod,
		                                                Sender, FailureMessage);
		ClientTransferItemReturnValue(bSuccess, FailureMessage.ToString());

		if (bSuccess)
		{
			if (UInventoryFunctionLibrary::IsStackableAndHaveStacks(StackableLeft, 0))
			{
				Sender->AddItemToInventoryArray(const_cast<FItemData&>(StackableLeft), StackableLeft.Index);
			}
		}
	}
}

void UPlayerInventoryComponent::ConfirmationPopupAccepted(UInventoryCore* Sender, const FItemData& ItemData,
                                                          const FItemData& InSlotData, EInputMethod InputMethod,
                                                          EItemDestination Initiator, EItemDestination Destination)
{
	if (!UInventoryFunctionLibrary::IsStackableAndHaveStacks(ItemData, 1))
	{
		if (Initiator == EItemDestination::InventorySlot || Initiator == EItemDestination::EquipmentSlot)
		{
			if (Destination == EItemDestination::DropBar)
			{
				DropItemOnTheGround(ItemData, Initiator);
			}
		}
	}
}

void UPlayerInventoryComponent::TakeItem(const FItemData& ItemData, UInventoryCore* Sender)
{
	if (!ItemData.IsItemClassValid()) return;

	FItemData localItemData = ItemData;

	FText FailureMessage;
	const bool bSuccess = TransferItemFromInventory(localItemData, FItemData(), EInputMethod::RightClick,
	                                                Sender, FailureMessage);

	FInventory& InventoryRef = Sender->GetInventoryRefByPanel(EInventoryPanels::P1);
	ClientTakeItemReturnValue(bSuccess, FailureMessage.ToString(), !InventoryRef.IsValidSlotIndex(0));

	if (bSuccess)
	{
		ClientItemLooted(localItemData);
	}
}

void UPlayerInventoryComponent::TakeAllItems(UInventoryCore* Sender)
{
	FInventory& SenderInventoryRef = Sender->GetInventoryRefByPanel(EInventoryPanels::P1);

	bool bSuccess = true;
	FText FailureMessage;

	for (const FItemData& ItemData : SenderInventoryRef.InventoryArray)
	{
		if (ItemData.IsItemClassValid())
		{
			FItemData localItemData = ItemData;


			if (TransferItemFromInventory(localItemData, FItemData(), EInputMethod::RightClick, Sender,
			                              FailureMessage))
			{
				ClientItemLooted(localItemData);
			}
			else
			{
				bSuccess = false;
				break;
			}
		}
	}

	ClientTakeItemReturnValue(bSuccess, FailureMessage.ToString(), !SenderInventoryRef.IsValidSlotIndex(0));
}

void UPlayerInventoryComponent::IncreaseItemDurability(const FItemData& ItemData, int32 Amount)
{
	if (!ItemData.IsItemClassValid()) return;

	UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject();

	if (!MainItemCDO->bUseDurability) return;

	FItemData localItemData = ItemData;
	localItemData.Quantity = FMath::Clamp(ItemData.Durability + Amount, 0, 100);

	if (localItemData.bIsEquipped)
	{
		UPlayerEquipmentComponent* PlayerEquipment = Cast<UPlayerEquipmentComponent>(
			GetOwner()->GetComponentByClass(UPlayerEquipmentComponent::StaticClass()));
		if (IsValid(PlayerEquipment))
		{
			PlayerEquipment->RemoveItemFromEquipmentArray(localItemData);
			PlayerEquipment->AddItemToEquipmentArray(localItemData, localItemData.EquipmentSlot);;
		}
	}
	else
	{
		AddItemToInventoryArray(localItemData, ItemData.Index);
	}
}

void UPlayerInventoryComponent::DecreaseItemDurability(const FItemData& ItemData, int32 Amount)
{
	if (!ItemData.IsItemClassValid()) return;

	UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject();

	if (!MainItemCDO->bUseDurability) return;

	FItemData localItemData = ItemData;
	localItemData.Quantity = FMath::Clamp(ItemData.Durability - Amount, 0, 100);

	if (localItemData.bIsEquipped)
	{
		UPlayerEquipmentComponent* PlayerEquipment = Cast<UPlayerEquipmentComponent>(
			GetOwner()->GetComponentByClass(UPlayerEquipmentComponent::StaticClass()));
		if (IsValid(PlayerEquipment))
		{
			PlayerEquipment->RemoveItemFromEquipmentArray(localItemData);
			PlayerEquipment->AddItemToEquipmentArray(localItemData, localItemData.EquipmentSlot);;
		}
	}
	else
	{
		AddItemToInventoryArray(localItemData, localItemData.Index);
	}
}

void UPlayerInventoryComponent::DropItemOnTheGround(const FItemData& ItemData, EItemDestination Initiator)
{
	if (Initiator == EItemDestination::InventorySlot || Initiator == EItemDestination::None)
	{
		RemoveItemFromInventoryArray(ItemData);
		RemoveWeightFromInventory(ItemData);
		ServerSpawnLootBag(ItemData);
		// ServerSpawnInteractable(ItemData);
	}
	else if (Initiator == EItemDestination::EquipmentSlot)
	{
		UPlayerEquipmentComponent* PlayerEquipment = Cast<UPlayerEquipmentComponent>(
			GetOwner()->GetComponentByClass(UPlayerEquipmentComponent::StaticClass()));

		PlayerEquipment->RemoveItemFromEquipmentArray(ItemData);
		PlayerEquipment->DetachItemFromEquipment(ItemData);
		RemoveWeightFromInventory(ItemData);
		ServerSpawnLootBag(ItemData);
		// ServerSpawnInteractable(ItemData);
	}
}


void UPlayerInventoryComponent::SpawnLootBagNearThePlayer(const FItemData& ItemData)
{
	// const AController* OwnerController = Cast<AController>(GetOwner());
	// if (!OwnerController) return;
	//
	// if (DroppedIndex < 1)
	// {
	// 	const APawn* OwnerPawn = OwnerController->GetPawn();
	// 	const FVector DroppedLocation = OwnerPawn->GetActorLocation() + (OwnerPawn->GetActorForwardVector() * 50.0f);
	// 	const FVector Start = DroppedLocation - FVector(0, 0, -50.0f);
	// 	const FVector End = Start - FVector(0, 0, -100.0f);
	//
	// 	ALootBag* localLootBag;
	// 	if (IsCollidingWithLootBag(Start, End, localLootBag))
	// 	{
	// 		LootBagRef = localLootBag;
	// 	}
	// 	else
	// 	{
	// 		if (UWorld* World = GetWorld())
	// 		{
	// 			const FVector SpawnLocation = FVector(0.0f, 0.0f, 0.0f);
	// 			const FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
	//
	// 			FActorSpawnParameters SpawnParams;
	// 			SpawnParams.Owner = GetOwner();
	//
	// 			LootBagRef = World->SpawnActor<ALootBag>(LootBagClass, SpawnLocation, SpawnRotation, SpawnParams);
	// 		}
	// 	}
	// }
	//
	// DroppedIndex = DroppedIndex + 1;
	//
	// if (UInventoryCore* ReceiverInventory = Cast<UInventoryCore>(
	// 	LootBagRef->GetComponentByClass(UInventoryCore::StaticClass())))
	// {
	// 	FInventory& InventoryRef = ReceiverInventory->GetInventoryRefByPanel(ItemData.GetInventoryPanelFromItem());
	//
	// 	int32 PartialStackIndex = 0;
	// 	if (InventoryRef.HasPartialStack(ItemData, PartialStackIndex))
	// 	{
	// 		ReceiverInventory->AddToStackInInventory(const_cast<FItemData&>(ItemData), PartialStackIndex);
	// 	}
	// 	else
	// 	{
	// 		ReceiverInventory->AddItemToInventoryArray(const_cast<FItemData&>(ItemData), -1);
	// 	}
	// }
}

void UPlayerInventoryComponent::SpawnItemMeshNearThePlayer(const FItemData& ItemData)
{
	// const AController* OwnerController = Cast<AController>(GetOwner());
	// if (!OwnerController) return;
	//
	// APawn* OwnerPawn = OwnerController->GetPawn();
	//
	// FRotator DroppedRotator = FRotator(0.0, 30.0f, 0.0f) * FMath::Clamp(DroppedIndex, 0, 12);
	// FVector DroppedLocation = DroppedRotator.RotateVector(OwnerPawn->GetActorForwardVector());
	// float RandValue = DroppedIndex & 1 ? FMath::FRandRange(50.0f, 150.0f) : FMath::FRandRange(-50.0f, -150.0f);
	//
	// FVector SpawnLocation = OwnerPawn->GetActorLocation() + (DroppedLocation * RandValue) + FVector(0, 0, 5.0f);
	//
	// UWorld* World = GetWorld();
	// if (!World) return;
	//
	// FActorSpawnParameters SpawnParams;
	// SpawnParams.Owner = GetOwner();
	// ABaseItemPickUp* ItemPickUpRef = World->SpawnActor<ABaseItemPickUp>(ItemData.ItemClass, SpawnLocation, FRotator(),
	//                                                                     SpawnParams);
	//
	// ItemPickUpRef->MulticastSetPhysicsSimulation();
	//
	// DroppedIndex = DroppedIndex + 1;
	//
	// if (UInventoryCore* ReceiverInventory = Cast<UInventoryCore>(
	// 	LootBagRef->GetComponentByClass(UInventoryCore::StaticClass())))
	// {
	// 	FInventory& InventoryRef = ReceiverInventory->GetInventoryRefByPanel(ItemData.GetInventoryPanelFromItem());
	//
	// 	int32 PartialStackIndex = 0;
	// 	if (InventoryRef.HasPartialStack(ItemData, PartialStackIndex))
	// 	{
	// 		ReceiverInventory->AddToStackInInventory(const_cast<FItemData&>(ItemData), PartialStackIndex);
	// 	}
	// 	else
	// 	{
	// 		ReceiverInventory->AddItemToInventoryArray(const_cast<FItemData&>(ItemData), -1);
	// 	}
	// }
}

bool UPlayerInventoryComponent::IsCollidingWithLootBag(FVector Start, FVector End, AActor*& OutDroppedBag)
{
	// FHitResult OutHit;
	// if (UKismetSystemLibrary::LineTraceSingle(this, Start, End,
	//                                           UEngineTypes::ConvertToTraceType(ECC_Visibility),
	//                                           false, TArray<AActor*>(),
	//                                           EDrawDebugTrace::None, OutHit, true))
	// {
	// 	OutDroppedBag = Cast<ALootBag>(OutHit.GetActor());
	// 	return OutDroppedBag ? true : false;
	// }

	OutDroppedBag = nullptr;
	return false;
}

void UPlayerInventoryComponent::ServerSpawnLootBag_Implementation(const FItemData& ItemData)
{
	SpawnLootBagNearThePlayer(ItemData);
}

void UPlayerInventoryComponent::ServerTakeAllItems_Implementation(UInventoryCore* Sender)
{
	TakeAllItems(Sender);
}

void UPlayerInventoryComponent::ServerTakeItem_Implementation(const FItemData& ItemData, UInventoryCore* Sender)
{
	TakeItem(ItemData, Sender);
}

void UPlayerInventoryComponent::ServerDropItemOnTheGround_Implementation(const FItemData& ItemData,
                                                                         EItemDestination Initiator)
{
	DropItemOnTheGround(ItemData, Initiator);
}

void UPlayerInventoryComponent::ServerSpawnInteractable_Implementation(const FItemData& ItemData)
{
	SpawnItemMeshNearThePlayer(ItemData);
}

void UPlayerInventoryComponent::ServerChangeDroppedIndex_Implementation(int NewDroppedIndex)
{
	DroppedIndex = NewDroppedIndex;
}

void UPlayerInventoryComponent::ClientItemLooted_Implementation(const FItemData& ItemData)
{
	OnItemLooted.Broadcast(ItemData);
}
