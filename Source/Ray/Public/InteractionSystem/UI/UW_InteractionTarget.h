// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionSystem/Libraries/InteractionEnumStruct.h"
#include "UI/Widget/RayUserWidget.h"
#include "UW_InteractionTarget.generated.h"

class UInteractionTargetComponent;
/**
 * 
 */
UCLASS()
class RAY_API UUW_InteractionTarget : public URayUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="UW Interaction Target Event")
	void UpdateInteractionTarget(UInteractionTargetComponent* InteractionTarget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="UW Interaction Target Event")
	void UpdateInteractionText(bool Immediately, EInteractionState State);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="UW Interaction Target Event")
	void PlayInteractionUpdateAnimation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="UW Interaction Target Event")
	void PlayInteractionCompletedAnimation(EInteractionResult InteractionResult);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="UW Interaction Target Event")
	void SetInteractionPercent(double NewPercent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="UW Interaction Target Event")
	void ResetProgress();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="UW Interaction Target Event")
	void OnwidgetNewTarget(bool bIsNewTarget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="UW Interaction Target Event")
	void SetInteractionKeyText(const FKey& key);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="UW Interaction Target Event")
	void UpdateContentState(bool bIsInteraction);


	UFUNCTION(BlueprintCallable, Category="UW Interaction Target Event")
	void UpdatePositionInViewport();
	
	UFUNCTION(BlueprintPure, Category="UW Interaction Target Event")
	bool IsOnScreen(const FMargin& InMargin);

public:
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target")
	double CurrentPercent;
	
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target")
	double ScreenRadiusPercent;
	
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target")
	FVector2D WidgetPositionErrorFactor;
	
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target")
	FVector LastWorldLocation;

	UPROPERTY(BlueprintReadWrite, Category="Interaction Target")
	FMargin ScreenMargin;

	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Reference")
	UInteractionTargetComponent* WidgetInteractionTarget;
};
