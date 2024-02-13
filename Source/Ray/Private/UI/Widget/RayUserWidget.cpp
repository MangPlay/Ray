// RAI Project 개발자 : Nam Chan woo


#include "UI/Widget/RayUserWidget.h"

void URayUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
