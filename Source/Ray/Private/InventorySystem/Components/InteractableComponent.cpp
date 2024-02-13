// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/Components/InteractableComponent.h"

#include "Components/WidgetComponent.h"
#include "GameFramework/InputSettings.h"

#include "InventorySystem/Libraries/InventoryFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Libraries/RayTraceType.h"


// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	InteratableTag = FName(TEXT("Interactable"));
	DestroyableTag = FName(TEXT("Destroyable"));
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	EventInitialize();

	GetOwner()->Tags.AddUnique(InteratableTag);

	if (bDestroyAfterPickup)
	{
		GetOwner()->Tags.AddUnique(DestroyableTag);
	}
}


// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractableComponent::EventInitialize()
{
	// if (GetOwner()->GetClass()->ImplementsInterface(UInterface_Interactable::StaticClass()))
	// {
	// 	IInterface_Interactable::Execute_Initialize(GetOwner());
	// }
}

void UInteractableComponent::EventToggleHighlight(bool bHighlight, AActor* InInteractor)
{
	InteractionWidget->SetVisibility(bHighlight, false);
	for (UPrimitiveComponent* HighlightObject : ObjectsToHighlight)
	{
		HighlightObject->SetRenderCustomDepth(bHighlight);
	}

	APlayerController* CastPlayerController = Cast<APlayerController>(InInteractor);
	// SetWidgetLocalOwner(CastPlayerController);
}

void UInteractableComponent::SetupInteractableReferences(UBoxComponent* InInteractableArea, UWidgetComponent* InInteractionWidget, TSet<UPrimitiveComponent*> InHighlightableObjects)
{
	InteractableArea = InInteractableArea;
	InteractionWidget = InInteractionWidget;
	ObjectsToHighlight = InHighlightableObjects.Array();
	for (UPrimitiveComponent* HighlightObject : ObjectsToHighlight)
	{
		HighlightObject->SetCollisionResponseToChannel(RayTraceTypes::ECC_Interactable, ECR_Block);
	}
}

void UInteractableComponent::EventDurationPress()
{
	const FString IsKeyDownString = FString(TEXT("IsKeyDown"));
	KeyDownTimer = UKismetSystemLibrary::K2_SetTimer(this, IsKeyDownString, 0.05f, true);

	FKey InteractionKey;
	if (GetPressedKeyByActionName(FName(TEXT("Interaction")), InteractionKey))
	{
		PressedInteractionKey = InteractionKey;
	}
	else
	{
		PressedInteractionKey = EKeys::E;
	}
}

void UInteractableComponent::IsKeyDown()
{
	// UInteractableComponent* CurrentInteractable = UInventoryFunctionLibrary::GetCurrentInteractableObject(Interactor);
	// if (IsValid(CurrentInteractable) && this)
	// {
	// }

	UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, KeyDownTimer);
}

void UInteractableComponent::EventAssociatedActorInteraction(AActor* InInteractor)
{
	Interactor = InInteractor;

	TArray<AActor*> AssociatedInteractableKeys;
	AssociatedInteractableActors.GetKeys(AssociatedInteractableKeys);

	for (AActor* AssociatedActor : AssociatedInteractableKeys)
	{
		if (IsValid(AssociatedActor))
		{
			if (UInteractableComponent* AssociatedInteractable = Cast<UInteractableComponent>(AssociatedActor->GetComponentByClass(UInteractableComponent::StaticClass())))
			{
				AssociatedInteractable->EventCheckForInteractionWithAssociate(Interactor);
			}
		}
	}
}

void UInteractableComponent::EventInteraction(AActor* InInteractor)
{
	// Interactor = InInteractor;
	// bAlreadyInteracted = true;
	// if (GetOwner()->GetClass()->ImplementsInterface(UInterface_Interactable::StaticClass()))
	// {
	// 	IInterface_Interactable::Execute_Interaction(GetOwner(), Interactor);
	// }
	// RemoveInteractionByResponse();
}

void UInteractableComponent::EventCheckForInteractionWithAssociate(AActor* InInteractor)
{
	Interactor = InInteractor;

	if (bCheckForAssociatedActors)
	{
		if (!IsTargetInteractableValue())
		{
			return;
		}
	}

	EventInteraction(Interactor);

	// if (bRemoveAssociatedInteractablesOnComplete)
	// {
	// 	for (TTuple<AActor*, int32> AssociatedInteractableActor : AssociatedInteractableActors)
	// 	{
	// 		UInteractableComponent* AssociatedInteractable = GetInteractableComponent(AssociatedInteractableActor.Key);
	// 		AssociatedInteractable->EventRemoveInteraction();
	// 		AssociatedInteractable->InteractableResponse = EInteractionResponse::OnlyOnce;
	// 	}
	// }
	// else
	// {
	// 	if (InteractableResponse == EInteractionResponse::OnlyOnce)
	// 	{
	// 		for (TTuple<AActor*, int32> AssociatedInteractableActor : AssociatedInteractableActors)
	// 		{
	// 			UInteractableComponent* AssociatedInteractable = GetInteractableComponent(AssociatedInteractableActor.Key);
	// 			AssociatedInteractable->EventRemoveInteraction();
	// 			AssociatedInteractable->InteractableResponse = EInteractionResponse::OnlyOnce;
	// 		}
	// 	}
	// 	else if (InteractableResponse == EInteractionResponse::Temporary)
	// 	{
	// 		for (TTuple<AActor*, int32> AssociatedInteractableActor : AssociatedInteractableActors)
	// 		{
	// 			UInteractableComponent* AssociatedInteractable = GetInteractableComponent(AssociatedInteractableActor.Key);
	// 			AssociatedInteractable->EventRemoveInteraction();
	// 			AssociatedInteractable->ToggleCanBeReInitialized(false);
	// 		}
	// 	}
	// }
}

void UInteractableComponent::EventEndInteraction(APlayerController* Controller)
{
}

void UInteractableComponent::RemoveInteractionByResponse()
{
	// if (InteractableResponse == EInteractionResponse::OnlyOnce || InteractableResponse == EInteractionResponse::Temporary)
	// {
	// 	EventRemoveInteraction();
	// }
}

void UInteractableComponent::EventRemoveInteraction_Implementation()
{
	// if (GetOwner()->GetClass()->ImplementsInterface(UInterface_Interactable::StaticClass()))
	// {
	// 	IInterface_Interactable::Execute_RemoveInteraction(GetOwner());
	// }
	// bIsInteractable = false;
	// InteractableArea = nullptr;
	// if (bDestroyAfterPickup)
	// {
	// 	GetOwner()->Destroy();
	// }
}

void UInteractableComponent::ToggleCanBeReInitialized(bool bCondition)
{
}

bool UInteractableComponent::IsTargetInteractableValue()
{
	bool bSameInteractableValue = false;
	for (TTuple<AActor*, int32> AssociatedInteractableActor : AssociatedInteractableActors)
	{
		UInteractableComponent* AssociatedInteractable = Cast<UInteractableComponent>(AssociatedInteractableActor.Key->GetComponentByClass(UInteractableComponent::StaticClass()));

		if (AssociatedInteractableActor.Value == AssociatedInteractable->InteractableValue)
		{
			bSameInteractableValue = true;
		}
		else
		{
			bSameInteractableValue = false;
			break;
		}
	}

	return bSameInteractableValue;
}

bool UInteractableComponent::HoldingInput()
{
	// APlayerController* InteractorController = Cast<APlayerController>(Interactor);
	//
	// auto PressedTimeDown = InteractorController->GetInputKeyTimeDown(PressedInteractionKey);
	// FillInteractionWidgetBorder(PressedTimeDown);
	//
	// if (PressedTimeDown > MaxKeyTimeDown)
	// {
	// 	InteractionWidget	
	// }
	// else
	// {
	// 	
	// }
	return true;
}

bool UInteractableComponent::GetPressedKeyByActionName(FName InActionName, FKey& OutKey)
{
	UInputSettings* InputSettings = UInputSettings::GetInputSettings();

	TArray<FInputActionKeyMapping> ActionMapping;
	InputSettings->GetActionMappingByName(InActionName, ActionMapping);

	APlayerController* InteractorPlayerControler = Cast<APlayerController>(Interactor);
	if (!InteractorPlayerControler) return false;

	for (int32 i = 0; i < ActionMapping.Num(); ++i)
	{
		if (InteractorPlayerControler->WasInputKeyJustPressed(ActionMapping[i].Key))
		{
			OutKey = ActionMapping[i].Key;
			return true;
		}
	}

	return false;
}

UInteractableComponent* UInteractableComponent::GetInteractableComponent(AActor* Target)
{
	return Cast<UInteractableComponent>(Target->GetComponentByClass(UInteractableComponent::StaticClass()));
}
