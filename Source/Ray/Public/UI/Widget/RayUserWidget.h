// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RayUserWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RAY_API URayUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Ray User Widget Event")
	void SetWidgetController(UObject* InWidgetController);

	UFUNCTION(BlueprintImplementableEvent, Category="Ray User Widget Event")
	void WidgetControllerSet();

public:
	UPROPERTY(BlueprintReadWrite, Category="Ray User Widget")
	TObjectPtr<UObject> WidgetController;
};
