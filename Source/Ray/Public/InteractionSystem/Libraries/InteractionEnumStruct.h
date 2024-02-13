#pragma once

#include "CoreMinimal.h"

#include "InteractionEnumStruct.generated.h"


UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Tap UMETA(DisplayName="Tap"),
	Hold UMETA(DisplayName="Hold"),
	Repeat UMETA(DisplayName="Repeat")
};

UENUM(BlueprintType)
enum class EInteractionResponse : uint8
{
	// 지속성 있는
	Persistent UMETA(DisplayName="Persistent"),
	//한 번만
	OnlyOnce UMETA(DisplayName="Only Once"),
	//일시적인
	Temporary UMETA(DisplayName="Temporary")
};

UENUM(BlueprintType)
enum class EInteractionState : uint8
{
	None UMETA(DisplayName="None"),
	Waiting UMETA(DisplayName="Waiting"),
	Interacting UMETA(DisplayName="Interacting"),
	Done UMETA(DisplayName="Done")
};


UENUM(BlueprintType)
enum class EInteractionNetworkMethod : uint8
{
	// 활성화 상태 유지
	KeepEnabled UMETA(DisplayName="Keep Enabled"),
	//상호작용하는동안 비활성화
	DisableWhileInteracting UMETA(DisplayName="Disable While Interacting")
};


UENUM(BlueprintType)
enum class EInteractionResult : uint8
{
	Completed UMETA(DisplayName="Completed"),
	Canceled UMETA(DisplayName="Canceled")
};


UENUM(BlueprintType)
enum class EInteractionFinishMethod : uint8
{
	// 완료될 때 파괴
	DestroyOnCompleted UMETA(DisplayName="Destroy On Completed"),

	// 취소될 때 파괴	
	DestroyOnCanceled UMETA(DisplayName="Destroy On Canceled"),

	// 완료되거나 취소될 때 파괴
	DestroyOnCompletedOrCanceled UMETA(DisplayName="Destroy On Completed Or Canceled"),

	// 완료 후 일정 시간 후 재활성화
	ReactivateAfterDurationOnCompleted UMETA(DisplayName="Reactivate After Duration On Completed"),

	// 취소 후 일정 시간 후 재활성화
	ReactivateAfterDurationOnCanceled UMETA(DisplayName="Reactivate After Duration On Canceled"),

	// 완료되거나 취소된 후 일정 시간 후 재활성화
	ReactivateAfterDurationOnCompletedOrCanceled
	UMETA(DisplayName="Reactivate After Duration On Completed Or Canceled"),

	// 취소 후 일정 시간 후 재활성화, 완료될 때 파괴
	ReactivateAfterDurationOnCanceledAndDestroyOnCompleted UMETA(
		DisplayName="Reactivate After Duration On Canceled, Destroy On Completed"),

	// 취소 후 일정 시간 후 재활성화, 완료될 때 비활성화
	ReactivateAfterDurationOnCanceledAndDeactivateOnCompleted UMETA(
		DisplayName="Reactivate After Duration On Canceled, Deactivate On Completed"),

	// 완료 후 일정 시간 후 재활성화, 취소될 때 파괴
	ReactivateAfterDurationOnCompletedAndDestroyOnCanceled UMETA(
		DisplayName="Reactivate After Duration On Completed, Destroy On Canceled"),

	// 완료 후 일정 시간 후 재활성화, 취소될 때 비활성화
	ReactivateAfterDurationOnCompletedAndDeactivateOnCanceled UMETA(
		DisplayName="Reactivate After Duration On Completed, Deactivate On Canceled"),

	// 완료될 때 비활성화
	DeactivateOnCompleted UMETA(DisplayName="Deactivate On Completed"),

	// 취소될 때 비활성화
	DeactivateOnCanceled UMETA(DisplayName="Deactivate On Canceled"),

	// 완료되거나 취소될 때 비활성화
	DeactivateOnCompletedOrCanceled UMETA(DisplayName="Deactivate On Completed Or Canceled"),

	// 취소될 때 비활성화, 완료될 때 파괴
	DeactivateOnCanceledAndDestroyOnCompleted UMETA(DisplayName="Deactivate On Canceled, Destroy On Completed"),

	// 취소될 때 비활성화, 완료 후 일정 시간 후 재활성화
	DeactivateOnCanceledAndReactivateAfterDurationOnCompleted UMETA(
		DisplayName="Deactivate On Canceled, Reactivate After Duration On Completed"),

	// 완료될 때 비활성화, 취소될 때 파괴
	DeactivateOnCompletedAndDestroyOnCanceled UMETA(DisplayName="Deactivate On Completed, Destroy On Canceled"),

	// 완료될 때 비활성화, 취소 후 일정 시간 후 재활성화
	DeactivateOnCompletedAndReactivateAfterDurationOnCanceled UMETA(
		DisplayName="Deactivate On Completed, Reactivate After Duration On Canceled")
};


USTRUCT(BlueprintType)
struct FIconSelector
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Icon Selector")
	bool bUseMaterialAsTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Icon Selector")
	UTexture2D* Texture2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Icon Selector")
	UMaterialInstance* Material;
};
