

#include "Utilities/DelayHelper.h"

#include "..\..\Public\Utilities\RayLog.h"


UDelayHelper::UDelayHelper()
{
}

void UDelayHelper::DelayWhile(TFunction<bool()> IsConditionMet, TFunction<void()> OnDelayFinished, float DelayTime/*=0.1f*/)
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		RAY_LOG_ERROR(TEXT("DelayWhile: Invalid world!"));
		return;
	}

	// 지연 루프를 처리하도록 람다 함수 설정
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, IsConditionMet, OnDelayFinished, World]() // Capture 'World' here
	{
		if (IsConditionMet())
		{
			// 조건이 참이면 OnDelayFinished 함수를 호출하고 지연 루프를 종료합니다.
			OnDelayFinished();

			// 반복을 중지하려면 타이머를 지우십시오.
			World->GetTimerManager().ClearTimer(TimerHandle);
		}
	});

	// 루핑으로 지연 타이머 시작
	World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayTime, true);
}

void UDelayHelper::WhileLoopDelayed(TFunction<bool()> Condition, float Delay, TFunction<void()> LoopBody, TFunction<void()> Completed,
                                    int32& OutLoopIndex)
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		RAY_LOG_ERROR(TEXT("LoopWithDelay: 유효하지 않은 세계!"));
		return;
	}


	OutLoopIndex = 0;

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, Condition, LoopBody, Completed, &OutLoopIndex]()
	{
		OnTick(Condition, LoopBody, Completed, OutLoopIndex);
	});

	World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Delay, true, -1.f);
}

void UDelayHelper::OnTick(TFunction<bool()> Condition, TFunction<void()> LoopBody, TFunction<void()> Completed, int32& OutLoopIndex)
{
	if (Condition())
	{
		LoopBody();
		OutLoopIndex++;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		Completed();
	}
}
