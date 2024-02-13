// RAI Project 개발자 : Nam Chan woo


#include "UI/WidgetSystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Player/RayPlayerState.h"
#include "UI/HUD/RayHUD.h"
#include "UI/WidgetController/RayWidgetController.h"

UInventoryWidgetController* UWidgetSystemLibrary::GetInventoryWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (ARayHUD* RayHUD = Cast<ARayHUD>(PlayerController->GetHUD()))
		{
			ARayPlayerState* PlayerState = PlayerController->GetPlayerState<ARayPlayerState>();
			const FWidgetControllerParams WidgetControllerParams(PlayerController, PlayerState, RayHUD);
			return RayHUD->GetInventoryWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UInteractionWidgetController* UWidgetSystemLibrary::GetInteractionWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (ARayHUD* RayHUD = Cast<ARayHUD>(PlayerController->GetHUD()))
		{
			ARayPlayerState* PlayerState = PlayerController->GetPlayerState<ARayPlayerState>();
			const FWidgetControllerParams WidgetControllerParams(PlayerController, PlayerState, RayHUD);
			return RayHUD->GetInteractionWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}
