// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WidgetSystemLibrary.generated.h"

class UInteractionWidgetController;
class UInventoryWidgetController;
/**
 * 
 */
UCLASS()
class RAY_API UWidgetSystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category="Widget System Function Library|WidgetController", meta=(WorldContext="WorldContextObject"))
	static UInventoryWidgetController* GetInventoryWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="Widget System Function Library|WidgetController", meta=(WorldContext="WorldContextObject"))
	static UInteractionWidgetController* GetInteractionWidgetController(const UObject* WorldContextObject);
};
