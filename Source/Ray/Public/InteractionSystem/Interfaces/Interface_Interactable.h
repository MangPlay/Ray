#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UInterface_Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RAY_API IInterface_Interactable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interface_Interactable")
	void InitializeInteractable();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interface_Interactable")
	void SetupOverlapAttachComponent(USceneComponent* NewAttachComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interface_Interactable")
	void SetupMarkerTargetComponent(USceneComponent* NewMarkerTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interface_Interactable")
	void SetupHighlightedComponents(const TArray<UPrimitiveComponent*>& NewHighlightComponents);
};
