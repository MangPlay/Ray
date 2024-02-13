// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RayHUD.generated.h"

struct FWidgetControllerParams;
class URayUserWidget;
class URayOverlayWidget;
class UInventoryWidgetController;
class UInteractionWidgetController;

/**
 * 
 */
UCLASS()
class RAY_API ARayHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Ray HUD Event")
	void InitOverlay(APlayerController* PC, APlayerState* PS);

public:
	UFUNCTION(BlueprintPure, Category="Ray HUD Event|Widget Controller")
	UInventoryWidgetController* GetInventoryWidgetController(FWidgetControllerParams WidgetControllerParams);

	UFUNCTION(BlueprintPure, Category="Ray HUD Event|Widget Controller")
	UInteractionWidgetController* GetInteractionWidgetController(FWidgetControllerParams WidgetControllerParams);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray HUD|Setup|Widget")
	TSubclassOf<URayUserWidget> OverlayWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray HUD|Setup|Widget Controller")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray HUD|Setup|Widget Controller")
	TSubclassOf<UInteractionWidgetController> InteractionWidgetControllerClass;

	UPROPERTY(BlueprintReadWrite, Category="Ray HUD|Widget Controller|Reference")
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(BlueprintReadWrite, Category="Ray HUD|Widget Controller|Reference")
	TObjectPtr<UInteractionWidgetController> InteractionWidgetController;

	UPROPERTY(BlueprintReadWrite, Category="Ray HUD|Widget Controller")
	TObjectPtr<URayOverlayWidget> OverlayWidget;
};
