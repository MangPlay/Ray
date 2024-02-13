
#include "UI/WidgetController/InteractionWidgetController.h"

UInteractionWidgetController::UInteractionWidgetController()
{
	for (EKeyBoardMouseInputs Input = EKeyBoardMouseInputs::A; 
		Input <= EKeyBoardMouseInputs::MouseMiddleClick; 
		Input = static_cast<EKeyBoardMouseInputs>(static_cast<uint8>(Input) + 1))
	{
		GamepadKeyboardIcons.Add(Input);
	}
}

void UInteractionWidgetController::BroadcastInitialValues()
{
	
}

void UInteractionWidgetController::BindCallbacksToDependencies()
{
	
}

UTexture2D* UInteractionWidgetController::GetGamepadIcon(EGamepadInputs GamepadInput)
{
	if (GamepadInputIcons.Contains(GamepadInput))
	{
		return *GamepadInputIcons.Find(GamepadInput);
	}
	
	return nullptr;
}

UTexture2D* UInteractionWidgetController::GetKeyboardMouseIcon(EKeyBoardMouseInputs KeyBoardInput)
{
	if (GamepadKeyboardIcons.Contains(KeyBoardInput))
	{
		return *GamepadKeyboardIcons.Find(KeyBoardInput);
	}
	return nullptr;
}
