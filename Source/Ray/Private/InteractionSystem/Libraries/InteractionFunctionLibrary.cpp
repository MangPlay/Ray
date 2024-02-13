#include "InteractionSystem/Libraries/InteractionFunctionLibrary.h"

#include "InteractionSystem/Components/InteractionManagerComponent.h"
#include "InteractionSystem/Components/InteractionTargetComponent.h"
#include "InventorySystem/Components/InventoryCore.h"


UInteractionManagerComponent* UInteractionFunctionLibrary::GetInteractionManagerComponent(AActor* Target)
{
	UInteractionManagerComponent* TargetInteractionManager = Cast<UInteractionManagerComponent>(Target->GetComponentByClass(UInteractionManagerComponent::StaticClass()));

	if (IsValid(TargetInteractionManager))
	{
		return TargetInteractionManager;
	}

	return nullptr;
}

UInteractionTargetComponent* UInteractionFunctionLibrary::GetCurrentInteractableObject(AActor* Target)
{
	UInteractionManagerComponent* TargetInteractionManager = Cast<UInteractionManagerComponent>(Target->GetComponentByClass(UInteractionManagerComponent::StaticClass()));

	if (IsValid(TargetInteractionManager))
	{
		if (IsValid(TargetInteractionManager->BestInteractionTarget))
		{
			return TargetInteractionManager->BestInteractionTarget;
		}
	}

	return nullptr;
}

bool UInteractionFunctionLibrary::GetInventoryFromInteractable(UInteractionTargetComponent* Interactable, UInventoryCore*& Inventory)
{
	if (IsValid(Interactable))
	{
		UInventoryCore* OwnerInventory = Cast<UInventoryCore>(Interactable->GetOwner()->GetComponentByClass(UInventoryCore::StaticClass()));
		if (IsValid(OwnerInventory))
		{
			Inventory = OwnerInventory;
			return true;
		}
	}

	Inventory = nullptr;
	return false;
}
