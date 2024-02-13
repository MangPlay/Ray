#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InteractionFunctionLibrary.generated.h"

class UInteractionManagerComponent;
class UInteractionTargetComponent;
/**
 * 
 */
UCLASS()
class RAY_API UInteractionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Interaction Function Library|Interaction System")
	UInteractionManagerComponent* GetInteractionManagerComponent(AActor* Target);
	
	UFUNCTION(BlueprintPure, Category="Interaction Function Library|Interaction System")
	static UInteractionTargetComponent* GetCurrentInteractableObject(AActor* Target);

	UFUNCTION(BlueprintPure, Category="Interaction Function Library|Interaction System")
	static bool GetInventoryFromInteractable(UInteractionTargetComponent* Interactable, UInventoryCore*& Inventory);
};
