#include "InteractionSystem/Components/InteractionManagerComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/PostProcessComponent.h"
#include "Components/SphereComponent.h"
#include "InteractionSystem/Components/InteractionTargetComponent.h"
#include "InteractionSystem/UI/UW_InteractionTarget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetInputLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utilities/RayLog.h"


UInteractionManagerComponent::UInteractionManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	WidgetScreenMargin = FMargin(100.0f);
}

void UInteractionManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	ConstructPlayerEssentials();

	ConstructPooledMarkerWidgets();
}

void UInteractionManagerComponent::ConstructPlayerEssentials()
{
	// 현재 오너를 플레이어 컨트롤러로 캐스팅합니다.
	if (APlayerController* CastPlayerController = Cast<APlayerController>(GetOwner()))
	{
		OwnerController = CastPlayerController;
	}

	// 포스트 프로세스 컴포넌트를 구성합니다.
	ConstructPostProcessComponent();

	// 상호작용 키를 업데이트하기 위한 타이머를 설정합니다.
	const FString UpdateInteractionKeysString = FString(TEXT("UpdateInteractionKeys"));
	BeginUpdateKeysTimerHandle = UKismetSystemLibrary::K2_SetTimer(this, UpdateInteractionKeysString, 0.2f, false);
}

void UInteractionManagerComponent::ConstructPostProcessComponent()
{
	// M_OutlineMaterial이 설정되었는지 확인합니다.
	RAY_IF_CHECK(M_OutlineMaterial, TEXT("M_OutlineMaterial를 설정해주세요"));

	// 오너 컨트롤러와 그 폰이 유효한 경우
	if (OwnerController && OwnerController->GetPawn())
	{
		// 새로운 포스트 프로세스 컴포넌트를 생성합니다.
		PostProcessComponent = NewObject<UPostProcessComponent>(OwnerController->GetPawn(), UPostProcessComponent::StaticClass());
		if (PostProcessComponent)
		{
			// 포스트 프로세스 컴포넌트를 등록합니다.
			PostProcessComponent->RegisterComponent();

			// 윤곽선 머티리얼을 위한 동적 인스턴스를 생성합니다.
			OutlineDynamicMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, M_OutlineMaterial);

			// 포스트 프로세스 설정에 동적 머티리얼을 추가합니다.
			TArray<FWeightedBlendable> WeightedBlendableArray;
			WeightedBlendableArray.Add(FWeightedBlendable(1.0f, OutlineDynamicMaterial));
			PostProcessComponent->Settings.WeightedBlendables.Array = WeightedBlendableArray;
		}
	}
}

void UInteractionManagerComponent::ConstructPooledMarkerWidgets()
{
	// WBP_InteractionTargetClass가 설정되었는지 확인합니다.
	RAY_IF_CHECK(WBP_InteractionTargetClass, TEXT("WB_InteractionTargetClass 설정해주세요"));

	// 로컬 플레이어 컨트롤러인지 확인합니다.
	if (!OwnerController->IsLocalPlayerController()) return;

	// 설정된 위젯 풀 사이즈만큼 위젯을 생성합니다.
	for (int32 i = 0; i < DefaultWidgetPoolSize + 1; ++i)
	{
		// 새로운 상호작용 타겟 위젯을 생성하고 위젯 풀에 추가합니다.
		if (UUW_InteractionTarget* InteractionWidget = CreateWidget<UUW_InteractionTarget>(OwnerController, WBP_InteractionTargetClass))
		{
			WidgetPool.AddUnique(InteractionWidget);
			InteractionWidget->AddToPlayerScreen();
		}
	}
}

void UInteractionManagerComponent::UpdateInteractionKeys()
{
	// 로컬 플레이어 컨트롤러가 아니면 함수를 종료합니다.
	if (!OwnerController->IsLocalPlayerController()) return;

	// 로컬 플레이어의 EnhancedInputSubsystem을 가져옵니다.
	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = OwnerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		// 상호작용 입력 액션에 매핑된 키들을 조회하여 InteractionKeys 배열에 저장합니다.
		InteractionKeys = EnhancedInputSubsystem->QueryKeysMappedToAction(InteractionInputAction);
	}

	// 상호작용 키 업데이트 타이머 핸들을 무효화하고 정리합니다.
	UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, BeginUpdateKeysTimerHandle);
}

void UInteractionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 서버 권한을 가지고 있다면 Tick을 종료합니다.
	if (!GetOwner()->HasAuthority()) return;

	UInteractionTargetComponent* NearInteractable = FindNearInteractable();

	UpdateBestInteractable(NearInteractable);

	DebugFunction();
}

UInteractionTargetComponent* UInteractionManagerComponent::FindNearInteractable()
{
	float BestDotProduct = 0.0f;
	UInteractionTargetComponent* TargetInteraction = nullptr;
	for (UInteractionTargetComponent* CurrentTarget : InteractionTargets)
	{
		// 유효하지 않거나 상호작용할 수 없는 대상은 건너뜁니다.
		if (!IsValid(CurrentTarget) || !IsInteractable(CurrentTarget)) continue;

		// 대상의 상대적 위치를 계산합니다.
		FVector TargetOffsetLocation =
			CurrentTarget->MarkerTargetComponent->GetComponentRotation().RotateVector(CurrentTarget->MarkerOffset);

		// 대상의 월드 위치를 계산합니다.
		FVector TargetWorldPosition =
			CurrentTarget->MarkerTargetComponent->GetComponentLocation() + TargetOffsetLocation;

		// 플레이어 카메라 매니저를 참조합니다.
		const TObjectPtr<APlayerCameraManager> OwnerCameraManager = OwnerController->PlayerCameraManager;

		// 대상까지의 방향을 계산합니다.
		FVector DirectionToTarget =
			UKismetMathLibrary::Normal(TargetWorldPosition - OwnerCameraManager->GetCameraLocation());

		// 카메라의 방향을 가져옵니다.
		FVector CameraDirection = OwnerCameraManager->GetCameraRotation().Vector();

		// 방향 벡터의 내적을 계산합니다.
		const double CurrentDotProduct = FVector::DotProduct(DirectionToTarget, CameraDirection);

		// 내적 값이 기준치보다 크고 이전 최고 값보다 큰 경우 업데이트합니다
		if (CurrentDotProduct > 0.5f && CurrentDotProduct > BestDotProduct)
		{
			BestDotProduct = CurrentDotProduct;
			TargetInteraction = CurrentTarget;
		}
	}
	return TargetInteraction;
}

void UInteractionManagerComponent::UpdateBestInteractable(UInteractionTargetComponent* NewTarget)
{
	// 새로운 대상이 유효한 경우
	if (IsValid(NewTarget))
	{
		// 현재 최적의 상호작용 대상이 새 대상과 다를 경우
		if (BestInteractionTarget != NewTarget)
		{
			// 기존의 최적 대상이 유효한 경우, 해당 대상에 대한 상호작용을 종료합니다.
			if (IsValid(BestInteractionTarget))
			{
				ClientSetNewTarget(false, BestInteractionTarget);
				if (bIsInteracting)
				{
					bIsInteracting = false;
					BestInteractionTarget->OnInteractionEnd.Broadcast(EInteractionResult::Canceled, OwnerController->GetPawn());
					ClientResetData();
				}
			}

			// 새로운 대상을 최적 대상으로 설정합니다.
			BestInteractionTarget = NewTarget;
			ClientSetNewTarget(true, BestInteractionTarget);
		}
	}
	// 새로운 대상이 유효하지 않은 경우
	else
	{
		// 현재 최적 대상이 유효한 경우, 해당 대상에 대한 상호작용을 종료합니다.
		if (IsValid(BestInteractionTarget))
		{
			ClientSetNewTarget(false, BestInteractionTarget);
			if (bIsInteracting)
			{
				bIsInteracting = false;
				BestInteractionTarget->OnInteractionEnd.Broadcast(EInteractionResult::Canceled, OwnerController->GetPawn());
				ClientResetData();
			}

			// 최적 대상을 초기화합니다.
			BestInteractionTarget = nullptr;
			ClientSetNewTarget(false, nullptr);
		}
	}

	// 현재 최적의 상호작용 대상이 유효한 경우, 키 입력 상태를 확인합니다.
	if (IsValid(BestInteractionTarget))
		ClientCheckPressedKey();
}

void UInteractionManagerComponent::DebugFunction()
{
	if (!bDebug) return;

	if (!GetOwner()->HasAuthority()) return;

	RAY_LOG_SCREEN(TEXT("Interaction Targets: %d"), InteractionTargets.Num());
	RAY_LOG_SCREEN(TEXT("Point Of Interests: %d"), PointOfInterests.Num());
	RAY_LOG_SCREEN(TEXT("Pending Targets: %d"), PendingTargets.Num());
	RAY_LOG_SCREEN(TEXT("Marker: %s"), *CurrentInteractionMarker.GetName());
}

UUW_InteractionTarget* UInteractionManagerComponent::FindEmptyWidget()
{
	// 위젯 풀의 모든 위젯을 순회합니다.
	for (UUW_InteractionTarget* InteractionWidget : WidgetPool)
	{
		// 현재 위젯이 상호작용 대상과 연결되어 있지 않다면,
		if (InteractionWidget->WidgetInteractionTarget == nullptr)
		{
			// 해당 위젯을 반환합니다.
			return InteractionWidget;
		}
	}
	// 사용 가능한 위젯이 없다면 nullptr을 반환합니다.
	return nullptr;
}

UUW_InteractionTarget* UInteractionManagerComponent::FindWidgetByInteractionTarget(UInteractionTargetComponent* InteractionTarget)
{
	// 위젯 풀을 순회하며 해당 상호작용 대상과 연결된 위젯을 찾습니다.
	for (UUW_InteractionTarget* WBP_InteractionTarget : WidgetPool)
	{
		// 위젯의 상호작용 대상이 주어진 대상과 일치하는 경우 해당 위젯을 반환합니다.
		if (WBP_InteractionTarget->WidgetInteractionTarget == InteractionTarget)
		{
			return WBP_InteractionTarget;
		}
	}
	// 일치하는 위젯이 없는 경우 nullptr을 반환합니다.
	return nullptr;
}

bool UInteractionManagerComponent::IsInteractable(UInteractionTargetComponent* ItemToFind)
{
	return ItemToFind->IsInteractionEnabled() && (!(PendingTargets.Contains(ItemToFind) || DeactivatedTargets.Contains(ItemToFind)));
}

bool UInteractionManagerComponent::GetInteractionKeys(TArray<FKey>& OutKeys)
{
	// 현재 최적의 상호작용 대상이 유효한지 확인합니다.
	if (IsValid(BestInteractionTarget))
	{
		// 커스텀 키 사용 여부를 확인합니다.
		if (BestInteractionTarget->bUseCustomKeys)
		{
			// 커스텀 키가 있으면 해당 키를 반환합니다
			OutKeys = BestInteractionTarget->CustomKeys.IsValidIndex(0) ? BestInteractionTarget->CustomKeys : TArray<FKey>();

			// 커스텀 키가 유효한지 여부를 반환합니다.
			return BestInteractionTarget->CustomKeys.IsValidIndex(0);
		}
	}

	// 커스텀 키가 없는 경우, 일반 상호작용 키를 반환합니다.
	OutKeys = InteractionKeys.IsValidIndex(0) ? InteractionKeys : TArray<FKey>();
	return InteractionKeys.IsValidIndex(0);
}

void UInteractionManagerComponent::TryTakeInteraction()
{
	// 상호작용에 사용할 키를 가져옵니다.
	TArray<FKey> InteractionTargetKeys;
	if (!GetInteractionKeys(InteractionTargetKeys)) return;

	// 상호작용 키들을 순회하며 입력된 키가 있는지 확인합니다.
	for (const FKey& InteractionTargetKey : InteractionTargetKeys)
	{
		if (OwnerController->WasInputKeyJustPressed(InteractionTargetKey))
		{
			LastPressedKey = InteractionTargetKey;
			break;
		}
	}

	// 상호작용 유형에 따라 적절한 상호작용 함수를 호출합니다.
	if (BestInteractionTarget->InteractionType == EInteractionType::Tap)
	{
		TapInteraction();
	}
	else if (BestInteractionTarget->InteractionType == EInteractionType::Hold)
	{
		HoldInteraction();
	}
	else if (BestInteractionTarget->InteractionType == EInteractionType::Repeat)
	{
		RepeatInteraction();
	}

	// 마지막으로 눌린 키가 방금 눌린 경우와 방금 놓인 경우를 감지하여 상태를 업데이트합니다.
	if (OwnerController->WasInputKeyJustPressed(LastPressedKey))
		bKeyJustPressed = true;

	if (OwnerController->WasInputKeyJustReleased(LastPressedKey))
		bKeyJustPressed = false;
}

void UInteractionManagerComponent::ApplyFinishMethod(UInteractionTargetComponent* InteractionTarget, EInteractionResult InteractionResult)
{
	// 상호작용 중 플래그를 false로 설정합니다.
	bIsInteracting = false;

	// 클라이언트의 데이터를 초기화합니다.
	ClientResetData();

	// 상호작용 종료 이벤트를 방송합니다.
	InteractionTarget->OnInteractionEnd.Broadcast(InteractionResult, OwnerController->GetPawn());

	// 상호작용 결과에 따라 다른 처리를 수행합니다.
	if (InteractionResult == EInteractionResult::Completed)
	{
		switch (InteractionTarget->FinishMethod)
		{
		// 완료 시 대상을 파괴하는 경우
		case EInteractionFinishMethod::DestroyOnCompleted: //#1
		case EInteractionFinishMethod::DestroyOnCompletedOrCanceled: //#3
		case EInteractionFinishMethod::ReactivateAfterDurationOnCanceledAndDestroyOnCompleted: //#7
		case EInteractionFinishMethod::DeactivateOnCanceledAndDestroyOnCompleted: //#14
			InteractionTarget->OwnerReference->Destroy();
			break;

		// 완료 시 대상을 일정 시간 후에 재활성화하는 경우
		case EInteractionFinishMethod::ReactivateAfterDurationOnCompleted: //#4
		case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedOrCanceled: //#6
		case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedAndDestroyOnCanceled: //#9
		case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedAndDeactivateOnCanceled: //#10
		case EInteractionFinishMethod::DeactivateOnCanceledAndReactivateAfterDurationOnCompleted: //#15
			InteractionTarget->OnAddedToPendingTarget();
			break;

		// 완료 시 대상을 비활성화하는 경우
		case EInteractionFinishMethod::ReactivateAfterDurationOnCanceledAndDeactivateOnCompleted: //#8
		case EInteractionFinishMethod::DeactivateOnCompleted: //#11
		case EInteractionFinishMethod::DeactivateOnCompletedOrCanceled: //#13
		case EInteractionFinishMethod::DeactivateOnCompletedAndDestroyOnCanceled: //#16
		case EInteractionFinishMethod::DeactivateOnCompletedAndReactivateAfterDurationOnCanceled: //#17
			AddToDeactivatedTargets(InteractionTarget);
			break;

		default: break;;
		}
	}

	else if (InteractionResult == EInteractionResult::Canceled)
	{
		switch (InteractionTarget->FinishMethod)
		{
		// 취소 시 대상을 파괴하는 경우
		case EInteractionFinishMethod::DestroyOnCanceled:
		case EInteractionFinishMethod::DestroyOnCompletedOrCanceled:
		case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedAndDestroyOnCanceled:
		case EInteractionFinishMethod::DeactivateOnCompletedAndDestroyOnCanceled:
			InteractionTarget->OwnerReference->Destroy();
			break;

		// 취소 시 대상을 일정 시간 후에 재활성화하는 경우
		case EInteractionFinishMethod::ReactivateAfterDurationOnCanceled:
		case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedOrCanceled:
		case EInteractionFinishMethod::ReactivateAfterDurationOnCanceledAndDestroyOnCompleted:
		case EInteractionFinishMethod::ReactivateAfterDurationOnCanceledAndDeactivateOnCompleted:
		case EInteractionFinishMethod::DeactivateOnCompletedAndReactivateAfterDurationOnCanceled:
			InteractionTarget->OnAddedToPendingTarget();
			break;

		// 취소 시 대상을 비활성화하는 경우
		case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedAndDeactivateOnCanceled:
		case EInteractionFinishMethod::DeactivateOnCanceled:
		case EInteractionFinishMethod::DeactivateOnCompletedOrCanceled:
		case EInteractionFinishMethod::DeactivateOnCanceledAndDestroyOnCompleted:
		case EInteractionFinishMethod::DeactivateOnCanceledAndReactivateAfterDurationOnCompleted:
			AddToDeactivatedTargets(InteractionTarget);
			break;

		default: break;;
		}
	}
}

void UInteractionManagerComponent::ReceiveAnyKey(FKey Key)
{
	// 이것은 단지 아이디어일 뿐이며 컨트롤러 AnyKey 이벤트에서 호출할 수 있습니다.
	// C++에는 이 작업에 대한 좋은 솔루션이 있습니다. 아이디어는 컨트롤러를 기반으로 위젯을 동적으로 변경하는 것입니다.
	bIsGamepad = UKismetInputLibrary::Key_IsGamepadKey(Key);
}

void UInteractionManagerComponent::TapInteraction()
{
	// 키가 방금 눌렸는지 확인합니다. 눌리지 않았다면 함수를 종료합니다.
	if (!bKeyJustPressed) return;

	// 서버에 상호작용 시작을 알립니다.
	ServerOnInteractionBegin(BestInteractionTarget);

	// 현재 상호작용 마커 위젯의 텍스트를 업데이트합니다.
	CurrentInteractionMarker->UpdateInteractionText(true, EInteractionState::Done);

	// 상호작용 완료 애니메이션을 재생합니다.
	CurrentInteractionMarker->PlayInteractionCompletedAnimation(EInteractionResult::Completed);

	if (BestInteractionTarget->InteractionResponse == EInteractionResponse::Persistent) return;

	// 서버에 상호작용 완료를 알립니다.
	ServerOnInteractionFinished(BestInteractionTarget, EInteractionResult::Completed);
}

void UInteractionManagerComponent::HoldInteraction()
{
	// 상호작용 키가 눌리고 있는 상태이고, 최근에 키가 눌렸다면
	if (OwnerController->IsInputKeyDown(LastPressedKey) && bKeyJustPressed)
	{
		// 현재 유지 시간이 0초라면 상호작용을 시작합니다.
		if (CurrentHoldTime == 0.0f)
		{
			// 상호작용 마커의 텍스트를 '상호작용 중' 상태로 업데이트합니다.
			CurrentInteractionMarker->UpdateInteractionText(false, EInteractionState::Interacting);

			// 서버에 상호작용 시작을 알립니다.
			ServerOnInteractionBegin(BestInteractionTarget);
		}

		// 현재 유지 시간을 델타 시간만큼 증가시킵니다.
		CurrentHoldTime = UGameplayStatics::GetWorldDeltaSeconds(this) + CurrentHoldTime;

		// 현재 유지 시간을 상호작용이 필요한 총 시간에 대해 정규화합니다.
		double HoldDurationRatio = UKismetMathLibrary::NormalizeToRange(CurrentHoldTime, 0.0f, BestInteractionTarget->HoldSeconds);

		// 상호작용 비율을 0과 1 사이로 제한합니다.
		HoldDurationRatio = FMath::Min(HoldDurationRatio, 1.0f);

		// 상호작용 마커에 상호작용 비율을 설정합니다.
		CurrentInteractionMarker->SetInteractionPercent(HoldDurationRatio);

		// 상호작용 업데이트 이벤트를 방송합니다.
		OnInteractionUpdated(BestInteractionTarget, HoldDurationRatio, 0);

		// 상호작용 비율이 100%에 도달하면 상호작용을 완료 처리합니다.
		if (HoldDurationRatio == 1.0f)
		{
			// 현재 유지 시간을 0으로 리셋합니다.
			CurrentHoldTime = 0.0f;
			// 상호작용 완료 애니메이션을 재생합니다.
			CurrentInteractionMarker->PlayInteractionCompletedAnimation(EInteractionResult::Completed);
			// 서버에 상호작용 완료를 알립니다.
			ServerOnInteractionFinished(BestInteractionTarget, EInteractionResult::Completed);
		}
	}
	else
	{
		// 현재 유지 시간이 0이 아니면 쿨다운 로직을 처리합니다.
		if (!(CurrentHoldTime != 0.0f)) return;

		// 쿨다운이 활성화되어 있으면
		if (BestInteractionTarget->bCooldownEnabled)
		{
			// 현재 유지 시간에서 델타 시간을 뺀 후, 0보다 작지 않게 합니다.
			CurrentHoldTime = FMath::Max(CurrentHoldTime - UGameplayStatics::GetWorldDeltaSeconds(this), 0.0f);

			// 남은 유지 시간을 정규화합니다.
			const double RemainingHoldDuration = FMath::Max(UKismetMathLibrary::NormalizeToRange(CurrentHoldTime, 0.0f, BestInteractionTarget->HoldSeconds), 0.0f);

			// 상호작용 마커에 남은 유지 시간 비율을 설정합니다.
			CurrentInteractionMarker->SetInteractionPercent(RemainingHoldDuration);

			// 남은 유지 시간이 0이면 상호작용을 취소 처리합니다.
			if (CurrentHoldTime == 0.0f)
			{
				// 상호작용 취소 애니메이션을 재생합니다.
				CurrentInteractionMarker->PlayInteractionCompletedAnimation(EInteractionResult::Canceled);
				// 서버에 상호작용 취소를 알립니다.
				ServerOnInteractionFinished(BestInteractionTarget, EInteractionResult::Canceled);
			}
			else
			{
				// 상호작용 업데이트 이벤트를 방송합니다.
				OnInteractionUpdated(BestInteractionTarget, RemainingHoldDuration, 0);
				// 키 릴리스 시 상호작용을 취소해야 한다면
				if (BestInteractionTarget->CancelOnRelease())
				{
					// 서버에 상호작용 취소를 알립니다.
					ServerOnInteractionFinished(BestInteractionTarget, EInteractionResult::Canceled);
				}
			}
		}
		else
		{
			// 쿨다운이 없으면 즉시 상호작용을 취소합니다.
			CurrentHoldTime = 0.0f;
			// 상호작용 마커의 상호작용 비율을 0으로 설정합니다.
			CurrentInteractionMarker->SetInteractionPercent(0.0f);
			// 상호작용 취소 애니메이션을 재생합니다.
			CurrentInteractionMarker->PlayInteractionCompletedAnimation(EInteractionResult::Canceled);
			// 서버에 상호작용 취소를 알립니다.
			ServerOnInteractionFinished(BestInteractionTarget, EInteractionResult::Canceled);
		}
	}
}

void UInteractionManagerComponent::RepeatInteraction()
{
	// 마지막으로 눌린 키가 방금 눌렸는지 확인합니다.
	if (OwnerController->WasInputKeyJustPressed(LastPressedKey))
	{
		// 반복 상호작용의 쿨다운 시간을 설정합니다.
		RepeatCoolDown = BestInteractionTarget->RepeatCoolDown;

		// 반복된 횟수가 0이면 상호작용을 시작합니다.
		if (Repeated == 0)
		{
			CurrentInteractionMarker->UpdateInteractionText(false, EInteractionState::Interacting);
			ServerOnInteractionBegin(BestInteractionTarget);
		}

		// 반복된 횟수를 1 증가시킵니다.
		Repeated = Repeated + 1;

		// 반복된 횟수의 비율을 계산합니다.
		float RepeatedRatio = UKismetMathLibrary::NormalizeToRange(Repeated, 0.0f, BestInteractionTarget->RepeatCount);
		RepeatedRatio = FMath::Min(RepeatedRatio, 1.0f);

		// 상호작용 업데이트 이벤트를 방송합니다.
		OnInteractionUpdated(BestInteractionTarget, RepeatedRatio, Repeated);

		// 상호작용 마커의 진행률을 업데이트합니다.
		CurrentInteractionMarker->SetInteractionPercent(RepeatedRatio);

		// 반복된 횟수가 최대값에 도달하면 상호작용을 완료 처리합니다.
		if (Repeated == BestInteractionTarget->RepeatCount)
		{
			Repeated = 0;
			// 상호작용 완료 애니메이션을 재생합니다.
			CurrentInteractionMarker->PlayInteractionCompletedAnimation(EInteractionResult::Completed);
			// 서버에 상호작용 완료를 알립니다.
			ServerOnInteractionFinished(BestInteractionTarget, EInteractionResult::Completed);
		}
		else
		{
			// 상호작용 업데이트 애니메이션을 재생합니다.
			CurrentInteractionMarker->PlayInteractionUpdateAnimation();
		}
	}

	// 최적의 상호작용 대상이 유효하면
	if (IsValid(BestInteractionTarget))
	{
		// 반복된 횟수가 0이 아니고 쿨다운이 활성화되어 있으면
		if (!(Repeated != 0 && BestInteractionTarget->bCooldownEnabled)) return;

		// 쿨다운 시간을 감소시킵니다.
		RepeatCoolDown = FMath::Max(RepeatCoolDown - UGameplayStatics::GetWorldDeltaSeconds(this), 0.0f);

		// 쿨다운이 끝나면 상호작용을 취소 처리합니다.
		if (RepeatCoolDown == 0.0f)
		{
			// 상호작용 취소 애니메이션을 재생합니다.
			CurrentInteractionMarker->PlayInteractionCompletedAnimation(EInteractionResult::Canceled);
			Repeated = Repeated - 1;

			// 반복된 횟수가 0이 되면
			if (Repeated == 0.0f)
			{
				// 상호작용 마커의 진행률을 0으로 설정합니다.
				CurrentInteractionMarker->SetInteractionPercent(0.0f);
				// 상호작용 취소 애니메이션을 재생합니다.
				CurrentInteractionMarker->PlayInteractionCompletedAnimation(EInteractionResult::Canceled);
				// 서버에 상호작용 취소를 알립니다.
				ServerOnInteractionFinished(BestInteractionTarget, EInteractionResult::Canceled);
				// 상호작용 마커의 진행 상태를 리셋합니다.
				CurrentInteractionMarker->ResetProgress();
				// 상호작용 대기 상태로 텍스트를 업데이트합니다.
				CurrentInteractionMarker->UpdateInteractionText(false, EInteractionState::Waiting);
			}
			else
			{
				// 쿨다운 시간을 재설정합니다.
				RepeatCoolDown = BestInteractionTarget->RepeatCoolDown;
			}

			// 상호작용 업데이트 이벤트를 방송합니다.
			OnInteractionUpdated(BestInteractionTarget, 0.0f, Repeated);
		}
		else
		{
			// 반복 상호작용의 쿨다운 시간과 진행률을 계산합니다.
			float TotalRepeatCooldown = BestInteractionTarget->RepeatCoolDown * Repeated;
			float RepeatCooldownRatio = FMath::Min(UKismetMathLibrary::NormalizeToRange(Repeated, 0.0f, BestInteractionTarget->RepeatCount), 1.0f);
			// 쿨다운 시간에 따른 진행률 감소를 계산합니다.
			float CooldownDeltaPercent = UGameplayStatics::GetWorldDeltaSeconds(this) * (1.0f / (TotalRepeatCooldown / RepeatCooldownRatio));
			// 상호작용 마커의 진행률을 업데이트합니다.
			CurrentInteractionMarker->SetInteractionPercent(CurrentInteractionMarker->CurrentPercent - CooldownDeltaPercent);
		}
	}
}

void UInteractionManagerComponent::OnInteractionTargetDestroyed(AActor* DestroyedActor)
{
	// 파괴된 액터에서 상호작용 대상 컴포넌트를 찾습니다.
	if (UInteractionTargetComponent* DestroyedInteractionTarget = Cast<UInteractionTargetComponent>(DestroyedActor->GetComponentByClass(UInteractionTargetComponent::StaticClass())))
	{
		// 상호작용 대상 목록에서 해당 대상을 제거합니다.
		InteractionTargets.Remove(DestroyedInteractionTarget);

		// 관심 지점 목록에서 해당 대상을 제거합니다.
		PointOfInterests.Remove(DestroyedInteractionTarget);

		// 클라이언트 측 처리를 위해 해당 함수를 호출합니다.
		ClientOnInteractionTargetDestroyed(DestroyedInteractionTarget);
	}

	// 파괴된 액터의 OnDestroyed 이벤트를 정리합니다.
	DestroyedActor->OnDestroyed.Clear();
}

void UInteractionManagerComponent::AddToPendingTargets(UInteractionTargetComponent* InteractionTarget)
{
	// 대기 중인 대상 목록에 현재 대상을 추가합니다. 이 때, 중복을 피하기 위해 AddUnique를 사용합니다.
	PendingTargets.AddUnique(InteractionTarget);

	// 서버에 현재 대상을 관심 지점 목록에서 제거하도록 요청합니다.
	ServerUpdatePointOfInterests(false, InteractionTarget);

	// 서버에 현재 대상을 상호작용 대상 목록에서 제거하도록 요청합니다.
	ServerUpdateInteractionTargets(false, InteractionTarget);

	// 대기 중인 상호작용 대상을 주기적으로 체크하기 위한 타이머가 아직 설정되지 않았다면, 새 타이머를 설정합니다.
	if (!UKismetSystemLibrary::K2_IsValidTimerHandle(PendingTargetTimerHandle))
	{
		const FString FuncString_CheckForPendingTargets = FString(TEXT("CheckForPendingTargets"));
		PendingTargetTimerHandle = UKismetSystemLibrary::K2_SetTimer(this, FuncString_CheckForPendingTargets, PendingTargetCheckInterval, true);
	}
}

void UInteractionManagerComponent::OnInteractionTargetReactivated(UInteractionTargetComponent* InteractionTarget)
{
	// 대기 목록에서 현재 대상을 제거합니다.
	PendingTargets.Remove(InteractionTarget);

	// 상호작용 대상을 활성화 상태로 설정합니다.
	InteractionTarget->bInteractionEnabled = true;

	// 현재 플레이어 컨트롤러가 대상에 할당된 상호작용자 목록에 포함되어 있는지 확인합니다.
	if (InteractionTarget->AssignedInteractors.Contains(OwnerController))
	{
		// 플레이어 컨트롤러의 폰과 중첩되는 컴포넌트 목록을 가져옵니다.
		TArray<UPrimitiveComponent*> OverlappingComponents;
		OwnerController->GetPawn()->GetOverlappingComponents(OverlappingComponents);

		// 폰이 대상의 내부 영역에 있는 경우
		if (OverlappingComponents.Contains(InteractionTarget->InnerZone))
		{
			// 서버에 상호작용 대상과 관심 지점을 업데이트하라고 요청합니다.
			ServerUpdatePointOfInterests(true, InteractionTarget);
			ServerUpdateInteractionTargets(true, InteractionTarget);
		}
		// 폰이 대상의 외부 영역에만 있는 경우
		else if (OverlappingComponents.Contains(InteractionTarget->OuterZone))
		{
			// 서버에 관심 지점을 업데이트하라고 요청합니다.
			ServerUpdatePointOfInterests(true, InteractionTarget);
		}
	}

	// 상호작용 재활성화 이벤트를 방송합니다.
	InteractionTarget->OnInteractionReactivated.Broadcast(OwnerController->GetPawn());
}

void UInteractionManagerComponent::AddToDeactivatedTargets(UInteractionTargetComponent* InteractionTarget)
{
	// 비활성화된 대상 목록에 현재 대상을 추가합니다.
	DeactivatedTargets.AddUnique(InteractionTarget);

	// 서버에 관심 지점 목록에서 현재 대상을 제거하도록 요청합니다.
	ServerUpdatePointOfInterests(false, InteractionTarget);

	// 서버에 상호작용 대상 목록에서 현재 대상을 제거하도록 요청합니다.
	ServerUpdateInteractionTargets(false, InteractionTarget);

	// 상호작용 대상 컴포넌트에 비활성화 이벤트를 호출합니다.
	InteractionTarget->OnDeactivated();
}

void UInteractionManagerComponent::CheckForPendingTargets()
{
	// 대기 중인 대상 목록이 비어있지 않은 경우
	if (!PendingTargets.IsEmpty())
	{
		// 대기 중인 모든 대상들을 순회합니다.
		for (UInteractionTargetComponent* PendingInteractTarget : PendingTargets)
		{
			// 마지막 상호작용 시간과 현재 시간의 차이를 계산합니다.
			const float TimeSinceLastInteraction = FMath::Abs(PendingInteractTarget->LastInteractedTime - UGameplayStatics::GetWorldDeltaSeconds(this));

			// 이 차이가 재활성화 기간보다 크거나 같다면, 대상을 재활성화합니다.
			if (TimeSinceLastInteraction >= PendingInteractTarget->ReactivationDuration)
			{
				OnInteractionTargetReactivated(PendingInteractTarget);
			}
		}
	}
	// 대기 중인 대상 목록이 비어있다면,
	else
	{
		// 대상 체크를 위해 설정된 타이머를 무효화합니다.
		UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, PendingTargetTimerHandle);
	}
}

void UInteractionManagerComponent::RemoveFromDeactivatedTargets(UInteractionTargetComponent* InteractionTarget)
{
	// 비활성화된 상호작용 대상 목록에서 지정된 대상을 제거합니다.
	DeactivatedTargets.Remove(InteractionTarget);
}

void UInteractionManagerComponent::OnNewTargetSelectedClientSide(bool bIsSelected, UInteractionTargetComponent* NewTarget)
{
	// 새로운 대상을 최적 대상으로 설정합니다.
	BestInteractionTarget = NewTarget;

	// 대상이 선택된 경우
	if (bIsSelected)
	{
		// 선택된 대상을 강조 표시합니다.
		SetTargetHighlighted(BestInteractionTarget, true);

		// 대상에 해당하는 위젯을 찾습니다.
		CurrentInteractionMarker = FindWidgetByInteractionTarget(BestInteractionTarget);

		// 상호작용 키를 가져옵니다.
		TArray<FKey> LocInteractionKeys;
		GetInteractionKeys(LocInteractionKeys);

		// 위젯에 상호작용 키를 설정합니다.
		CurrentInteractionMarker->SetInteractionKeyText(LocInteractionKeys[0]);

		// 위젯에 새 대상이 선택되었음을 알립니다.
		CurrentInteractionMarker->OnwidgetNewTarget(true);
	}
	//대상이 선택되지 않은 경우
	else
	{
		// 현재 위젯이 유효한 경우
		if (IsValid(CurrentInteractionMarker))
		{
			// 위젯에 대상 선택이 해제되었음을 알립니다.
			CurrentInteractionMarker->OnwidgetNewTarget(false);

			// 현재 위젯을 초기화합니다.
			CurrentInteractionMarker = nullptr;

			// 선택되었던 대상의 강조 표시를 해제합니다.
			SetTargetHighlighted(BestInteractionTarget, false);
		}
	}
}

void UInteractionManagerComponent::SetTargetHighlighted(UInteractionTargetComponent* InteractionTarget, bool bIsHighlighted)
{
	// 상호작용 대상의 하이라이트 색상을 동적 머티리얼의 파라미터로 설정합니다.
	OutlineDynamicMaterial->SetVectorParameterValue(FName(TEXT("Outline Color")), InteractionTarget->HighlightColor);

	// 상호작용 대상 컴포넌트의 하이라이트 상태를 설정합니다.
	InteractionTarget->SetHighlight(bIsHighlighted);
}

void UInteractionManagerComponent::OnPointOfInterestUpdatedServerSide(bool bAdd, UInteractionTargetComponent* InteractionTarget)
{
	// 관심 지점을 추가하는 경우
	if (bAdd)
	{
		// 상호작용이 가능한 대상인지 확인합니다.
		if (IsInteractable(InteractionTarget))
		{
			// 관심 지점 목록에 대상을 추가합니다.
			PointOfInterests.AddUnique(InteractionTarget);

			// 대상이 파괴될 때 호출될 이벤트를 등록합니다.
			InteractionTarget->OwnerReference->OnDestroyed.AddDynamic(this, &UInteractionManagerComponent::OnInteractionTargetDestroyed);

			// 클라이언트에 관심 지점 목록을 업데이트합니다.
			ClientUpdatePointOfInterests(true, InteractionTarget);
		}
		else
		{
			// 재활성화가 가능한 경우 대기 목록에 추가합니다.
			if (InteractionTarget->IsReactivationEnabled())
			{
				AddToPendingTargets(InteractionTarget);
			}
		}
	}
	// 관심 지점을 제거하는 경우
	else
	{
		// 대상이 유효한 경우
		if (IsValid(InteractionTarget))
		{
			// 관심 지점 목록에서 대상을 제거합니다.
			PointOfInterests.Remove(InteractionTarget);
			// 클라이언트에 관심 지점 목록을 업데이트합니다.
			ClientUpdatePointOfInterests(false, InteractionTarget);
		}
	}
}

void UInteractionManagerComponent::OnPointOfInterestUpdatedClientSide(bool bAdd, UInteractionTargetComponent* InteractionTarget)
{
	// 관심 지점을 추가하는 경우
	if (bAdd)
	{
		// 상호작용 대상의 위젯 정보를 업데이트합니다.
		InteractionTarget->UpdateWidgetInfo(WidgetScreenMargin, ScreenRadiusPercent);

		// 빈 위젯을 찾습니다.
		UUW_InteractionTarget* EmptyWidget = FindEmptyWidget();
		if (IsValid(EmptyWidget))
		{
			// 빈 위젯에 상호작용 대상을 업데이트합니다.
			EmptyWidget->UpdateInteractionTarget(InteractionTarget);
		}
		else
		{
			// 로컬 플레이어 컨트롤러가 있는 경우 새로운 상호작용 위젯을 생성합니다.
			if (OwnerController->IsLocalPlayerController())
			{
				// 생성된 위젯을 위젯 풀에 추가하고 화면에 추가합니다.
				UUW_InteractionTarget* CreateInteractionTargetWidget = CreateWidget<UUW_InteractionTarget>(OwnerController, WBP_InteractionTargetClass);
				WidgetPool.AddUnique(CreateInteractionTargetWidget);
				CreateInteractionTargetWidget->AddToPlayerScreen();
				CreateInteractionTargetWidget->UpdateInteractionTarget(InteractionTarget);
			}
		}
	}
	// 관심 지점을 제거하는 경우
	else
	{
		// 관련 위젯을 찾습니다.
		UUW_InteractionTarget* InteractionTargetWidget = FindWidgetByInteractionTarget(InteractionTarget);
		if (IsValid(InteractionTargetWidget))
		{
			// 위젯의 상호작용 대상을 초기화합니다.
			InteractionTargetWidget->UpdateInteractionTarget(nullptr);
		}
	}
}

void UInteractionManagerComponent::OnInteractionTargetUpdatedServerSide(bool bAdd, UInteractionTargetComponent* InteractionTarget)
{
	// 상호작용 대상을 추가하는 경우
	if (bAdd)
	{
		// 상호작용이 가능한 대상인지 확인합니다.
		if (IsInteractable(InteractionTarget))
		{
			// 클라이언트에 상호작용 대상 목록을 업데이트합니다.
			ClientUpdateInteractionTargets(true, InteractionTarget);
			// 상호작용 대상 목록에 대상을 추가합니다.
			InteractionTargets.AddUnique(InteractionTarget);
		}
	}
	// 상호작용 대상을 제거하는 경우
	else
	{
		// 상호작용 대상 목록에서 대상을 제거합니다.
		InteractionTargets.Remove(InteractionTarget);
		// 클라이언트에 상호작용 대상 목록을 업데이트합니다.
		ClientUpdateInteractionTargets(false, InteractionTarget);
	}
}

void UInteractionManagerComponent::OnInteractionTargetUpdatedClientSide(bool bAdd, UInteractionTargetComponent* InteractionTarget)
{
	// 상호작용 대상과 연결된 위젯을 찾습니다.
	UUW_InteractionTarget* InteractionTargetWidget = FindWidgetByInteractionTarget(InteractionTarget);
	if (IsValid(InteractionTargetWidget))
	{
		// 찾은 위젯의 내용 상태를 업데이트합니다.
		InteractionTargetWidget->UpdateContentState(bAdd);
	}
}

void UInteractionManagerComponent::ClientOnInteractionTargetDestroyed_Implementation(UInteractionTargetComponent* InteractionTarget)
{
	// 현재 상호작용 마커 위젯의 대상을 null로 설정합니다.
	// 이는 상호작용 대상이 파괴되어 더 이상 유효하지 않음을 나타냅니다.
	CurrentInteractionMarker->UpdateInteractionTarget(nullptr);
}

void UInteractionManagerComponent::OnInteractionUpdated(UInteractionTargetComponent* InteractionTarget, double Alpha, int32 InRepeated)
{
	// 상호작용 대상의 업데이트 이벤트를 방송합니다.
	InteractionTarget->OnInteractionUpdated.Broadcast(Alpha, InRepeated, OwnerController->GetPawn());

	// 게임이 단독 모드(예: 에디터)가 아닌 경우 서버에 상호작용 업데이트를 알립니다.
	if (!UKismetSystemLibrary::IsStandalone(this))
	{
		ServerOnInteractionUpdated(InteractionTarget, Alpha, InRepeated, OwnerController->GetPawn());
	}
}

void UInteractionManagerComponent::ServerOnInteractionUpdated_Implementation(UInteractionTargetComponent* InteractionTarget, float Alpha, int32 InRepeated, APawn* InteractorPawn)
{
	// 상호작용 대상의 업데이트 이벤트를 방송합니다.
	InteractionTarget->OnInteractionUpdated.Broadcast(Alpha, InRepeated, InteractorPawn);
}

void UInteractionManagerComponent::ServerOnInteractionBegin_Implementation(UInteractionTargetComponent* InteractionTarget)
{
	// 상호작용 대상의 상호작용 시작 이벤트를 방송합니다.
	InteractionTarget->OnInteractionBegin.Broadcast(OwnerController->GetPawn());

	// 상호작용 중임을 나타내는 플래그를 true로 설정합니다.
	bIsInteracting = true;
}

void UInteractionManagerComponent::ClientUpdateInteractionTargets_Implementation(bool bAdd, UInteractionTargetComponent* InteractionTarget)
{
	// 클라이언트 측에서 상호작용 대상 목록을 업데이트합니다.
	OnInteractionTargetUpdatedClientSide(bAdd, InteractionTarget);
}

void UInteractionManagerComponent::ClientUpdatePointOfInterests_Implementation(bool bAdd, UInteractionTargetComponent* InteractionTarget)
{
	// 클라이언트 측에서 관심 지점 목록을 업데이트합니다.
	OnPointOfInterestUpdatedClientSide(bAdd, InteractionTarget);
}

void UInteractionManagerComponent::ServerRequestAssignInteractor_Implementation(bool bAdd, UInteractionTargetComponent* InteractionTarget)
{
	// 상호작용 대상이 유효한 경우
	if (IsValid(InteractionTarget))
	{
		// 상호작용 대상에 인터랙터를 할당하거나 해제합니다.
		InteractionTarget->AssignInteractor(bAdd, OwnerController);
	}
}

void UInteractionManagerComponent::ClientResetData_Implementation()
{
	// 키가 방금 눌렸는지를 나타내는 플래그를 false로 설정합니다.
	bKeyJustPressed = false;

	// 마지막으로 눌린 키를 초기화합니다.
	LastPressedKey = FKey();

	// 현재 홀드 상호작용 시간을 0으로 재설정합니다.
	CurrentHoldTime = 0.0f;

	// 반복된 상호작용 횟수를 0으로 재설정합니다.
	Repeated = 0;
}

void UInteractionManagerComponent::ServerUpdatePointOfInterests_Implementation(bool bAdd, UInteractionTargetComponent* InteractionTarget)
{
	// 서버 측에서 관심 지점 목록을 업데이트합니다.
	OnPointOfInterestUpdatedServerSide(bAdd, InteractionTarget);
}

void UInteractionManagerComponent::ServerUpdateInteractionTargets_Implementation(bool bAdd, UInteractionTargetComponent* InteractionTarget)
{
	// 서버 측에서 상호작용 대상 목록을 업데이트합니다.
	OnInteractionTargetUpdatedServerSide(bAdd, InteractionTarget);
}

void UInteractionManagerComponent::ServerOnInteractionFinished_Implementation(UInteractionTargetComponent* InteractionTarget, EInteractionResult InteractionResult)
{
	// 상호작용 완료 후 적용할 메소드를 호출합니다.
	ApplyFinishMethod(InteractionTarget, InteractionResult);
}

void UInteractionManagerComponent::ClientSetNewTarget_Implementation(bool bIsSelected, UInteractionTargetComponent* NewTarget)
{
	OnNewTargetSelectedClientSide(bIsSelected, NewTarget);
}

void UInteractionManagerComponent::ClientCheckPressedKey_Implementation()
{
	// 상호작용 시도를 합니다.
	TryTakeInteraction();
}
