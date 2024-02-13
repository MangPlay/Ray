#include "InteractionSystem/UI/UW_InteractionTarget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "InteractionSystem/Components/InteractionTargetComponent.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utilities/RayLog.h"

void UUW_InteractionTarget::UpdatePositionInViewport()
{
	// 뷰포트의 크기를 가져옵니다.
	
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	// 뷰포트의 중앙을 계산합니다.
	FVector2D ViewportHalfSize = ViewportSize * 0.5f;

	// 화면의 중앙에서 월드 좌표를 역산합니다.
	FVector CameraWorldLocation, CameraWorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(GetOwningPlayer(), ViewportHalfSize, CameraWorldLocation, CameraWorldDirection);


	// 인터랙션 대상의 월드 위치를 계산합니다.
	if (IsValid(WidgetInteractionTarget))
	{
		LastWorldLocation =
			WidgetInteractionTarget->MarkerTargetComponent->GetComponentLocation() +
			WidgetInteractionTarget->MarkerTargetComponent->GetComponentRotation().RotateVector(WidgetInteractionTarget->MarkerOffset);
	}

	// 카메라의 회전을 가져옵니다.
	FRotator CameraRotation = GetOwningPlayer()->PlayerCameraManager->GetCameraRotation();
	// 카메라 위치에서 마지막 월드 위치까지의 회전을 계산합니다.
	FRotator CameraDirection = UKismetMathLibrary::FindLookAtRotation(CameraWorldLocation, LastWorldLocation);

	// 카메라 방향과 대상 방향의 내적을 계산합니다.
	double CameraToTargetDotProduct = FVector::DotProduct(CameraRotation.Vector(), CameraDirection.Vector());
	// 내적 값을 클램핑합니다.
	double ClampedDotProductValue = UKismetMathLibrary::MapRangeClamped(CameraToTargetDotProduct, 0.0f, -1.0f, 0.0f, -1.0f);
	// 카메라 위 방향 벡터를 계산합니다.
	FVector CameraUpVector = UKismetMathLibrary::GetUpVector(CameraDirection) * ClampedDotProductValue;

	// 위젯의 월드 위치를 계산합니다.
	FVector WidgetWorldLocation = CameraDirection.Vector() + CameraWorldLocation + CameraUpVector + UKismetMathLibrary::GetRightVector(CameraDirection);

	// 위젯의 월드 위치를 화면 위치로 변환합니다.
	FVector2D ScreenLocation;
	UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), WidgetWorldLocation, ScreenLocation, true);

	// 스크린 위치를 뷰포트 기준으로 변환합니다.
	FVector2D ScreenViewportLocation2D = FVector2D(ScreenLocation.X - ViewportHalfSize.X, ScreenLocation.Y - ViewportHalfSize.Y);
	// 화면 위치의 정규화된 벡터를 계산합니다.
	FVector2D ScreenViewportNormal = UKismetMathLibrary::Normal2D(ScreenViewportLocation2D);

	// X축 각도에 따른 코사인 값을 계산합니다.
	double COSdegX = UKismetMathLibrary::DegCos(UKismetMathLibrary::DegAcos(ScreenViewportNormal.X));
	// 화면 반지름 범위 값을 계산합니다.
	double ScreenRadiusRangeValue = UKismetMathLibrary::MapRangeClamped(ScreenRadiusPercent, 0.0f, 1.0f, 0.0f, ViewportSize.Y);

	// Y축 각도에 따른 코사인 값을 계산합니다.
	double COSdegY = UKismetMathLibrary::DegCos(UKismetMathLibrary::DegSin(ScreenViewportNormal.X));
	// Y축 정규값을 계산합니다.
	double NormalYValue = ScreenViewportNormal.Y <= 0.0f ? -1.0f : 1.0f;

	// 계산된 화면 위치를 계산합니다.
	FVector2D CalculatedScreenPosition;
	CalculatedScreenPosition.X = (COSdegX * ScreenRadiusRangeValue) + ViewportHalfSize.X;
	CalculatedScreenPosition.Y = (COSdegY * ScreenRadiusRangeValue * NormalYValue) + ViewportHalfSize.Y;

	// 위젯의 원하는 크기를 가져옵니다.
	FVector2D WidgetDesiredSize = GetDesiredSize();

	// X축에서 화면 밖으로 나가지 않도록 범위를 제한합니다.
	FVector2D NotScreenVector2D;
	double MinX = WidgetDesiredSize.X + FMath::Abs(ScreenMargin.Left);
	double MaxX = ViewportSize.X - WidgetDesiredSize.X - FMath::Abs(ScreenMargin.Top);
	NotScreenVector2D.X = FMath::Clamp(CalculatedScreenPosition.X, MinX, MaxX);

	// Y축에서 화면 밖으로 나가지 않도록 범위를 제한합니다.
	double MinY = WidgetDesiredSize.X + FMath::Abs(ScreenMargin.Right);
	double MaxY = ViewportSize.Y - WidgetDesiredSize.Y - FMath::Abs(ScreenMargin.Bottom);
	NotScreenVector2D.Y = FMath::Clamp(CalculatedScreenPosition.Y, MinY, MaxY);

	// 마지막 월드 위치를 스크린 위치로 변환합니다.
	FVector2D ScreenVector2D;
	UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), LastWorldLocation, ScreenVector2D, true);

	// 화면 안에 있으면 스크린 위치를, 그렇지 않으면 계산된 위치를 사용합니다.
	FVector2D SelectScree2D = IsOnScreen(ScreenMargin) ? ScreenVector2D : NotScreenVector2D;

	// 위젯의 최종 화면 위치를 계산합니다.
	// FVector2D ScreenPosition = SelectScree2D - WidgetPositionErrorFactor - ViewportHalfSize; //5.1
	FVector2D ScreenPosition = SelectScree2D - WidgetPositionErrorFactor;

	// 스크린 위치를 위젯 로컬 좌표로 변환합니다.
	auto WidgetGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
	FVector2D LocalCoordinate;
	USlateBlueprintLibrary::ScreenToWidgetLocal(this, WidgetGeometry, ScreenPosition, LocalCoordinate, false);

	// 위젯의 렌더링 위치를 설정합니다.
	SetRenderTranslation(LocalCoordinate);
}

bool UUW_InteractionTarget::IsOnScreen(const FMargin& InMargin)
{
	FVector2D ScreenPosition;
	UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), LastWorldLocation, ScreenPosition, true);

	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	FVector2D ViewportHalfSize = FVector2D(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f);

	float MinX = ScreenPosition.X < ViewportHalfSize.X ? InMargin.Left : InMargin.Right;
	double MaxX = ViewportSize.X - MinX;
	bool IsOnScreenX = UKismetMathLibrary::InRange_FloatFloat(ScreenPosition.X, MinX, MaxX);

	float MinY = ScreenPosition.Y < ViewportHalfSize.Y ? InMargin.Top : InMargin.Bottom;
	double MaxY = ViewportSize.Y - MinY;
	bool IsOnScreenY = UKismetMathLibrary::InRange_FloatFloat(ScreenPosition.Y, MinY, MaxY);


	return IsOnScreenX && IsOnScreenY;
}
