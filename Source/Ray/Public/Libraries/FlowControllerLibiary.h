#pragma once

#include "CoreMinimal.h"
#include "FlowControllerLibiary.generated.h"


USTRUCT(BlueprintType)
struct FDoOnce
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	bool bDonOnce;

	FORCEINLINE FDoOnce();
	explicit FORCEINLINE FDoOnce(bool bStartClosed);


	FORCEINLINE void Reset() { bDonOnce = true; }

	FORCEINLINE bool Execute()
	{
		if (bDonOnce)
		{
			bDonOnce = false;
			return true;
		}
		return false;
	}
};

FORCEINLINE FDoOnce::FDoOnce() : bDonOnce(true)
{
}

FORCEINLINE FDoOnce::FDoOnce(const bool bStartClosed) : bDonOnce(!bStartClosed)
{
}


USTRUCT(BlueprintType)
struct FGate
{
	GENERATED_BODY()

public:
	FORCEINLINE FGate();

	explicit FORCEINLINE FGate(bool bStartClosed);


	FORCEINLINE void Open() { bGateOpen = true; }

	FORCEINLINE void Close() { bGateOpen = false; }

	FORCEINLINE void Toggle() { bGateOpen = !bGateOpen; }

	FORCEINLINE bool IsOpen() const { return bGateOpen; }

private:
	UPROPERTY(VisibleAnywhere)
	bool bGateOpen;
};

FORCEINLINE FGate::FGate() : bGateOpen(true)
{
}

FORCEINLINE FGate::FGate(const bool bStartClosed) : bGateOpen(!bStartClosed)
{
}