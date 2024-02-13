// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RayWidgetController.generated.h"

class ARayHUD;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ARayHUD> RayHUD;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TObjectPtr<UAttributeSet> AttributeSet = nullptr;

	FWidgetControllerParams()
	{
	}

	FWidgetControllerParams(APlayerController* InPlayerController, APlayerState* InPlayerState, ARayHUD* InRayHUD)
		: PlayerController(InPlayerController), PlayerState(InPlayerState), RayHUD(InRayHUD)
	{
	}
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class RAY_API URayWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Widget Controller Event")
	void SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams);

	UFUNCTION(BlueprintCallable, Category="Widget Controller Event")
	virtual void BroadcastInitialValues();

	UFUNCTION(BlueprintCallable, Category="Widget Controller Event")
	virtual void BindCallbacksToDependencies();

protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<ARayHUD> RayHUD;

	// UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	// TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	// TObjectPtr<UAttributeSet> AttributeSet;
};
