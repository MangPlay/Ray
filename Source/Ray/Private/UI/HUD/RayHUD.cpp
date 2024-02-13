// RAI Project 개발자 : Nam Chan woo


#include "UI/HUD/RayHUD.h"

#include "UI/Widget/RayUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/RayOverlayWidget.h"
#include "UI/WidgetController/InteractionWidgetController.h"

#include "UI/WidgetController/InventoryWidgetController.h"
#include "Utilities/RayLog.h"


void ARayHUD::InitOverlay(APlayerController* PC, APlayerState* PS)
{
	RAY_IF_CHECK(InventoryWidgetControllerClass, TEXT("InventoryWidgetControllerClass를 초기화 해주세요"));

	RAY_IF_CHECK(InteractionWidgetControllerClass, TEXT("InteractionWidgetControllerClass 초기화 해주세요"));

	RAY_IF_CHECK(OverlayWidgetClass, TEXT("OverlayWidgetClass를 초기화 해주세요"));


	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<URayOverlayWidget>(Widget);


	const FWidgetControllerParams WidgetControllerParams(PC, PS, this);
	InventoryWidgetController = GetInventoryWidgetController(WidgetControllerParams);
	InteractionWidgetController = GetInteractionWidgetController(WidgetControllerParams);


	OverlayWidget->SetWidgetController(InventoryWidgetController);

	InventoryWidgetController->BroadcastInitialValues();
	InteractionWidgetController->BroadcastInitialValues();
	
	OverlayWidget->AddToViewport();
	
}

UInventoryWidgetController* ARayHUD::GetInventoryWidgetController(FWidgetControllerParams WidgetControllerParams)
{
	if (InventoryWidgetController == nullptr)
	{
		InventoryWidgetController = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);
		InventoryWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		InventoryWidgetController->BindCallbacksToDependencies();
	}
	return InventoryWidgetController;
}

UInteractionWidgetController* ARayHUD::GetInteractionWidgetController(FWidgetControllerParams WidgetControllerParams)
{
	if (InteractionWidgetController == nullptr)
	{
		InteractionWidgetController = NewObject<UInteractionWidgetController>(this, InteractionWidgetControllerClass);
		InteractionWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		InteractionWidgetController->BindCallbacksToDependencies();
	}
	return InteractionWidgetController;
}
