#pragma once

#include "CoreMinimal.h"
#include "RayWidgetController.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "InteractionWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class RAY_API UInteractionWidgetController : public URayWidgetController
{
	GENERATED_BODY()

public:
	UInteractionWidgetController();
	
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

	UFUNCTION(BlueprintPure, Category="Interaction Widget Controller Event")
	UTexture2D* GetGamepadIcon(EGamepadInputs GamepadInput);

	UFUNCTION(BlueprintPure, Category="Interaction Widget Controller Event")
	UTexture2D* GetKeyboardMouseIcon(EKeyBoardMouseInputs KeyBoardInput);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction Widget Controller|Image")
	TMap<EGamepadInputs, UTexture2D*> GamepadInputIcons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction Widget Controller|Image")
	TMap<EKeyBoardMouseInputs, UTexture2D*> GamepadKeyboardIcons;
};
