#pragma once

#include "CoreMinimal.h"
#include "DelayHelper.generated.h"

/**
 * 
 */
UCLASS()
class RAY_API UDelayHelper : public UObject
{
	GENERATED_BODY()

public:
	UDelayHelper();

	/**
	@brief 지정된 조건이 충족되는지 반복적으로 확인하여 조건이 true가 될 때까지 OnDelayFinished 함수의 실행을 지연시킵니다.
	@param IsConditionMet 확인할 조건 함수입니다. true를 반환하면 OnDelayFinished 함수가 실행됩니다.
	@param DelayTime 조건 기능 확인 사이의 지연 시간(초)입니다.
	@param OnDelayFinished IsConditionMet 함수가 true를 반환하면 호출할 함수입니다.
	*/
	void DelayWhile(TFunction<bool()> IsConditionMet, TFunction<void()> OnDelayFinished, float DelayTime = 0.1f);

	/**
	@brief Condition 함수가 true를 반환하는 동안 LoopBody 함수를 반복 실행합니다. 지정된 지연 시간만큼 실행이 지연됩니다.
	@param Condition 확인할 조건 함수입니다. true를 반환하면 LoopBody 함수가 실행됩니다.
	@param Delay LoopBody 함수 실행 사이의 지연 시간(초)입니다.
	@param LoopBody Condition 함수가 true를 반환하는 동안 실행할 함수입니다.
	@param Completed Condition 함수가 false를 반환하면 호출할 함수입니다.
	@param OutLoopIndex LoopBody 함수가 실행된 횟수를 반환하는 출력 매개변수입니다.
	*/
	void WhileLoopDelayed(TFunction<bool()> Condition, float Delay, TFunction<void()> LoopBody,
	                      TFunction<void()> Completed, int32& OutLoopIndex);

private:
	FTimerHandle TimerHandle;

	void OnTick(TFunction<bool()> Condition, TFunction<void()> LoopBody, TFunction<void()> Completed,
	            int32& OutLoopIndex);
};
