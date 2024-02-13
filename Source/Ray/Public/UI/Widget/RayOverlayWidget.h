// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "RayUserWidget.h"
#include "RayOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class RAY_API URayOverlayWidget : public URayUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Overlay Widget Event")
	void ShowLootBar();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Overlay Widget Event")
	void HideLootBar();

public:
	UPROPERTY(BlueprintReadWrite, Category="Ray Overlay Widget")
	TObjectPtr<URayUserWidget> UW_LootBar;
};
