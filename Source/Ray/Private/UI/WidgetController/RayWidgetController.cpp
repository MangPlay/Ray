// RAI Project 개발자 : Nam Chan woo


#include "UI/WidgetController/RayWidgetController.h"

void URayWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	RayHUD = WidgetControllerParams.RayHUD;
}

void URayWidgetController::BroadcastInitialValues()
{
}

void URayWidgetController::BindCallbacksToDependencies()
{
}
