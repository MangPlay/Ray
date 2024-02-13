// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InteractionWidgetManager.generated.h"

/**
 * 
 */
UCLASS()
class RAY_API UInteractionWidgetManager : public UObject
{
	GENERATED_BODY()
//
// public:
// 	/**
// 	* 위젯 풀에서 현재 사용되지 않는 위젯을 찾는 함수입니다.
// 	*
// 	* 이 함수는 위젯 풀의 모든 위젯을 순회하며, 연결된 상호작용 대상이 없는 위젯을 찾습니다.
// 	* 사용 가능한 위젯이 발견되면 해당 위젯을 반환하고, 그렇지 않으면 nullptr을 반환합니다.
// 	*
// 	* @return UUW_InteractionTarget* - 사용 가능한 위젯 또는 nullptr.
// 	*/
// 	UFUNCTION(BlueprintPure, Category="Interaction Manager|Interactable")
// 	UUW_InteractionTarget* FindEmptyWidget();
//
// 	/**
// 	 * 주어진 상호작용 대상에 대응하는 위젯을 찾습니다.
// 	 * @param InteractionTarget 찾고자 하는 상호작용 대상 컴포넌트.
// 	 * @return UUW_InteractionTarget* 해당 상호작용 대상과 연결된 위젯을 반환합니다. 해당 위젯이 없는 경우 nullptr을 반환합니다.
// 	 */
// 	UFUNCTION(BlueprintPure, Category="Interaction Manager|Interactable")
// 	UUW_InteractionTarget* FindWidgetByInteractionTarget(UInteractionTargetComponent* InteractionTarget);
//
//
// public:
// 	// 상호작용 위젯 클래스.
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Manager|SetUp")
// 	TSubclassOf<UUW_InteractionTarget> WBP_InteractionTargetClass;
};
