#include "InteractionSystem/Components/InteractionTargetComponent.h"

#include "Components/SphereComponent.h"
#include "InteractionSystem/Components/InteractionManagerComponent.h"
#include "InteractionSystem/Interfaces/Interface_Interactable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utilities/AssetTableRef.h"


UInteractionTargetComponent::UInteractionTargetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	InteractionText.Add(EInteractionState::Waiting, FText::FromString(TEXT("Interact")));
	InteractionText.Add(EInteractionState::Interacting, FText::FromString(TEXT("Interacting")));
	InteractionText.Add(EInteractionState::Done, FText::FromString(TEXT("Completed")));

	TargetIconColor = FLinearColor(0.78, 0.78, 0.78, 1.0f);
	POIIconColor = FLinearColor(0.49, 0.49, 0.49, 1.0f);
	InteractionIconBackgroundColor = FLinearColor(0.19, 0.19, 0.19, 1.0f);
	InteractionIconForegroundColor = FLinearColor(1.0, 1.0, 1.0, 1.0f);

	if (UTexture2D* T_CircleRef = FAssetReferenceUtility::LoadAssetFromDataTable<UTexture2D>(AssetRefPath::TexturesPath, FName(TEXT("T_Circle"))))
	{
		POIIcon.Texture2D = T_CircleRef;
		InteractionIconBackground.Texture2D = T_CircleRef;
	}

	if (UMaterialInstance* MI_CircularProgressBarRef = FAssetReferenceUtility::LoadAssetFromDataTable<UMaterialInstance>(AssetRefPath::MaterialPath, FName(TEXT("MI_CircularProgressBar"))))
	{
		InteractionIconForeground.bUseMaterialAsTexture = true;
		InteractionIconForeground.Material = MI_CircularProgressBarRef;
	}

	HighlightColor = FLinearColor(1.0, 1.0, 1.0, 1.0f);
}

void UInteractionTargetComponent::BeginPlay()
{
	Super::BeginPlay();


	IInterface_Interactable::Execute_InitializeInteractable(GetOwner());

	ConstructOwnerEssentials();

	ConstructHighlightedComponents();

	ConstructOverlapZones();

	// SelectMarkerComponent(MarkerComponentName);

	OnInteractionBegin.AddDynamic(this, &UInteractionTargetComponent::OnInteractionBeginEvent);
	OnInteractionEnd.AddDynamic(this, &UInteractionTargetComponent::OnInteractionEndEvent);
}

void UInteractionTargetComponent::ConstructOwnerEssentials()
{
	// 이 컴포넌트를 소유한 액터에 대한 참조를 설정합니다.
	OwnerReference = GetOwner();
}

void UInteractionTargetComponent::ConstructOverlapZones()
{
	// 소유 액터의 모든 씬 컴포넌트를 가져옵니다.
	TArray<UActorComponent*> SceneComponents;
	OwnerReference->GetComponents(USceneComponent::StaticClass(), SceneComponents);

	// // 상호작용 영역에 첨부할 컴포넌트를 찾습니다.
	// USceneComponent* ComponentToAttach = nullptr;
	// for (UActorComponent* SceneComponent : SceneComponents)
	// {
	// 	if (FCString::Stricmp(*GetNameSafe(SceneComponent), *InteractionZoneComponentToAttach) == 0)
	// 	{
	// 		ComponentToAttach = Cast<USceneComponent>(SceneComponent);
	// 		break;
	// 	}
	// }

	// 내부 영역을 정의하는 구 컴포넌트를 생성하고 설정합니다.
	InnerZone = NewObject<USphereComponent>(this, USphereComponent::StaticClass());
	if (InnerZone)
	{
		InnerZone->AttachToComponent(ComponentToAttach ? ComponentToAttach : GetOwner()->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		InnerZone->SetHiddenInGame(!bEnableDebug);
		InnerZone->SetSphereRadius(InnerZoneRadius);
		InnerZone->RegisterComponent();
		InnerZone->OnComponentBeginOverlap.AddDynamic(this, &UInteractionTargetComponent::OnInnerZoneBeginOverlap);
		InnerZone->OnComponentEndOverlap.AddDynamic(this, &UInteractionTargetComponent::OnInnerZoneEndOverlap);
	}

	// 외부 영역을 정의하는 구 컴포넌트를 생성하고 설정합니다.
	OuterZone = NewObject<USphereComponent>(this, USphereComponent::StaticClass());
	if (OuterZone)
	{
		OuterZone->AttachToComponent(InnerZone ? InnerZone : GetOwner()->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		OuterZone->SetHiddenInGame(!bEnableDebug);
		OuterZone->SetSphereRadius(InnerZoneRadius + FMath::Max(OuterZoneExtent, 10.0f));
		OuterZone->RegisterComponent();
		OuterZone->OnComponentBeginOverlap.AddDynamic(this, &UInteractionTargetComponent::OnOuterZoneBeginOverlap);
		OuterZone->OnComponentEndOverlap.AddDynamic(this, &UInteractionTargetComponent::OnOuterZoneEndOverlap);
	}
}

void UInteractionTargetComponent::ConstructHighlightedComponents()
{
	// // 소유 액터의 루트 컴포넌트를 참조합니다.
	// USceneComponent* OwnerRootComponent = GetOwner()->GetRootComponent();
	//
	// // 루트 컴포넌트가 하이라이트 대상 목록에 포함되어 있는 경우, 하이라이트 컴포넌트로 추가합니다.
	// if (UPrimitiveComponent* OwnerPrimitiveComp = Cast<UPrimitiveComponent>(OwnerRootComponent))
	// {
	// 	if (ComponentsToHighlight.Contains(GetNameSafe(OwnerPrimitiveComp)))
	// 	{
	// 		HighlightedComponents.AddUnique(OwnerPrimitiveComp);
	// 	}
	// }
	//
	// // 소유 액터의 모든 자식 컴포넌트를 순회하며 하이라이트 대상 목록에 포함되어 있는 컴포넌트를 찾아 추가합니다.
	// TArray<USceneComponent*> ChildrenComponents;
	// OwnerRootComponent->GetChildrenComponents(true, ChildrenComponents);
	// for (USceneComponent* ChildrenComp : ChildrenComponents)
	// {
	// 	if (UPrimitiveComponent* ChildrenPrimitiveComp = Cast<UPrimitiveComponent>(ChildrenComp))
	// 	{
	// 		if (ComponentsToHighlight.Contains(GetNameSafe(ChildrenPrimitiveComp)))
	// 		{
	// 			HighlightedComponents.AddUnique(ChildrenPrimitiveComp);
	// 		}
	// 	}
	// }
}

// void UInteractionTargetComponent::SelectMarkerComponent(const FString& InComponentName)
// {
// 	USceneComponent* OwnerRootComp = GetOwner()->GetRootComponent();
//
// 	TArray<USceneComponent*> ChildrenComps;
// 	OwnerRootComp->GetChildrenComponents(true, ChildrenComps);
//
// 	ChildrenComps.Add(OwnerRootComp);
//
// 	for (USceneComponent* ChildrenComp : ChildrenComps)
// 	{
// 		if (FCString::Stricmp(*InComponentName, *GetNameSafe(ChildrenComp)) == 0)
// 		{
// 			MarkerTargetComponent = ChildrenComp;
// 			break;
// 		}
// 	}
//
// 	if (!MarkerTargetComponent)
// 		MarkerTargetComponent = OwnerRootComp;
// }

void UInteractionTargetComponent::OnInnerZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                          bool bFromWeep, const FHitResult& SweepResult)
{
	AController* OtherController = GetOtherController(OtherActor);
	if (!OtherController) return;

	UInteractionManagerComponent* OtherInteractionManager = GetInteractionManager(OtherController);
	if (!OtherInteractionManager) return;

	OtherInteractionManager->ServerUpdateInteractionTargets(true, this);
}

void UInteractionTargetComponent::OnInnerZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AController* OtherController = GetOtherController(OtherActor);
	if (!OtherController) return;

	UInteractionManagerComponent* OtherInteractionManager = GetInteractionManager(OtherController);
	if (!OtherInteractionManager) return;

	OtherInteractionManager->ServerUpdateInteractionTargets(false, this);
}

void UInteractionTargetComponent::OnOuterZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                          int32 OtherBodyIndex, bool bFromWeep, const FHitResult& SweepResult)
{
	AController* OtherController = GetOtherController(OtherActor);
	if (!OtherController) return;

	UInteractionManagerComponent* OtherInteractionManager = GetInteractionManager(OtherController);
	if (!OtherInteractionManager) return;

	OtherInteractionManager->ServerUpdatePointOfInterests(true, this);
	OtherInteractionManager->ServerRequestAssignInteractor(true, this);
}

void UInteractionTargetComponent::OnOuterZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AController* OtherController = GetOtherController(OtherActor);
	if (!OtherController) return;

	UInteractionManagerComponent* OtherInteractionManager = GetInteractionManager(OtherController);
	if (!OtherInteractionManager) return;

	OtherInteractionManager->ServerUpdatePointOfInterests(false, this);
	OtherInteractionManager->ServerRequestAssignInteractor(false, this);
}

void UInteractionTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractionTargetComponent::OnInteractionBeginEvent(APawn* InteractorPawn)
{
	// 오너가 서버 권한을 가지고 있는지 확인합니다.
	if (!GetOwner()->HasAuthority()) return;

	// 네트워크 처리 방식이 상호작용 중 비활성화로 설정된 경우
	if (NetworkHandleMethod == EInteractionNetworkMethod::DisableWhileInteracting)
	{
		// 할당된 모든 인터랙터들을 순회합니다.
		for (AController* AssignedInteractor : AssignedInteractors)
		{
			// 현재 상호작용을 시작한 폰과 다른 폰인 경우
			if (AssignedInteractor->GetPawn() != InteractorPawn)
			{
				// 해당 인터랙터의 상호작용 매니저 컴포넌트를 가져옵니다.
				if (UInteractionManagerComponent* AssignedInteractorManager = GetInteractionManager(AssignedInteractor))
				{
					// 상호작용 매니저에 현재 대상을 비활성화된 대상 목록에 추가합니다.
					AssignedInteractorManager->AddToDeactivatedTargets(this);
				}
			}
		}
	}
}

void UInteractionTargetComponent::OnInteractionEndEvent(EInteractionResult InteractionResult, APawn* InteractorPawn)
{
	// 오너가 서버 권한을 가지고 있는지 확인합니다.
	if (!GetOwner()->HasAuthority()) return;

	// 네트워크 처리 방식이 상호작용 중 비활성화로 설정된 경우
	if (NetworkHandleMethod == EInteractionNetworkMethod::DisableWhileInteracting)
	{
		// 할당된 모든 인터랙터들을 순회합니다.
		for (AController* AssignedInteractor : AssignedInteractors)
		{
			// 현재 상호작용을 종료한 폰과 다른 폰인 경우
			if (AssignedInteractor->GetPawn() != InteractorPawn)
			{
				// 해당 인터랙터의 상호작용 매니저 컴포넌트를 가져옵니다.
				if (UInteractionManagerComponent* CurrentInteractionManager = GetInteractionManager(AssignedInteractor))
				{
					// 현재 상호작용 매니저의 비활성화된 대상 목록에 현재 대상이 포함되어 있다면
					if (CurrentInteractionManager->DeactivatedTargets.Contains(this))
					{
						// 해당 대상을 비활성화된 대상 목록에서 제거합니다.
						CurrentInteractionManager->RemoveFromDeactivatedTargets(this);

						// 상호작용 결과를 적용합니다.
						CurrentInteractionManager->ApplyFinishMethod(this, InteractionResult);
					}
				}
			}
		}
	}
}

bool UInteractionTargetComponent::IsInteractionEnabled()
{
	// 현재 상호작용 가능 여부를 반환합니다.
	return bInteractionEnabled;
}

void UInteractionTargetComponent::AssignInteractor(bool bAdd, APlayerController* Interactor)
{
	if (bAdd)
	{
		// 인터랙터를 할당합니다. 중복을 피하기 위해 AddUnique를 사용합니다.=-0
		AssignedInteractors.AddUnique(Interactor);
	}
	else
	{
		// 인터랙터 할당을 해제합니다.
		AssignedInteractors.Remove(Interactor);
	}
}

void UInteractionTargetComponent::EnableInteraction(bool bEnabled)
{
	// 상호작용 가능 상태를 설정합니다.
	bInteractionEnabled = bEnabled;

	// 상호작용이 활성화된 경우
	if (bInteractionEnabled)
	{
		// 할당된 모든 컨트롤러들을 순회합니다.
		for (AController* AssignedController : AssignedInteractors)
		{
			// 할당된 컨트롤러에 대한 상호작용 관리자 컴포넌트를 가져옵니다.
			if (UInteractionManagerComponent* AssignedInteractionManager = GetInteractionManager(AssignedController))
			{
				// 비활성화된 대상 목록에서 현재 대상을 제거합니다.
				AssignedInteractionManager->RemoveFromDeactivatedTargets(this);

				// 상호작용 대상이 재활성화되었음을 알립니다.
				AssignedInteractionManager->OnInteractionTargetReactivated(this);
			}
		}
	}
	// 상호작용이 비활성화된 경우
	else
	{
		// 할당된 모든 컨트롤러들을 순회합니다.
		for (AController* AssignedController : AssignedInteractors)
		{
			// 할당된 컨트롤러에 대한 상호작용 관리자 컴포넌트를 가져옵니다.
			if (UInteractionManagerComponent* AssignedInteractionManager = GetInteractionManager(AssignedController))
			{
				// 현재 대상을 비활성화된 대상 목록에 추가합니다.
				AssignedInteractionManager->AddToDeactivatedTargets(this);
			}
		}
	}
}

void UInteractionTargetComponent::OnAddedToPendingTarget()
{
	// 마지막 상호작용 시간을 현재 게임 시간으로 설정합니다.
	LastInteractedTime = UKismetSystemLibrary::GetGameTimeInSeconds(this);

	// 이 상호작용 대상에 할당된 모든 컨트롤러를 순회합니다.
	for (AController* AssignedController : AssignedInteractors)
	{
		// 각 컨트롤러에 해당하는 인터랙션 매니저를 가져옵니다.
		if (UInteractionManagerComponent* AssignedInteractionManager = GetInteractionManager(AssignedController))
		{
			// 인터랙션 매니저에 이 상호작용 대상을 대기 중인 상호작용 대상 목록에 추가하도록 요청합니다.
			AssignedInteractionManager->AddToPendingTargets(this);
		}
	}

	// 이 상호작용 대상의 상호작용 가능 여부를 false로 설정합니다.
	bInteractionEnabled = false;
}

void UInteractionTargetComponent::OnDeactivated()
{
	// 상호작용 대상의 내부 영역이 다른 액터와의 충돌 이벤트를 생성하지 않도록 설정합니다.
	InnerZone->SetGenerateOverlapEvents(false);

	// 상호작용 대상의 외부 영역이 다른 액터와의 충돌 이벤트를 생성하지 않도록 설정합니다.
	OuterZone->SetGenerateOverlapEvents(false);

	// 게임 중에 내부 영역이 보이지 않도록 설정합니다.
	InnerZone->SetHiddenInGame(true, false);

	// 게임 중에 외부 영역이 보이지 않도록 설정합니다.
	OuterZone->SetHiddenInGame(true, false);
}

void UInteractionTargetComponent::SetHighlight(bool bIsHighlighted)
{
	for (UPrimitiveComponent* HighlightedComponent : HighlightedComponents)
	{
		HighlightedComponent->SetRenderCustomDepth(bIsHighlighted);
	}
}

void UInteractionTargetComponent::UpdateWidgetInfo(const FMargin& InWidgetMargin, float InScreenRadiusPercent)
{
	WidgetMargin = InWidgetMargin;
	ScreenRadiusPercent = InScreenRadiusPercent;
}

bool UInteractionTargetComponent::CancelOnRelease()
{
	// 상호작용 완료 방식에 따라 분기 처리합니다.
	switch (FinishMethod)
	{
	// 다음 경우에는 상호작용이 취소될 때 특정 작업을 수행합니다.
	case EInteractionFinishMethod::DestroyOnCanceled:
	case EInteractionFinishMethod::DestroyOnCompletedOrCanceled:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCanceled:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedOrCanceled:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCanceledAndDestroyOnCompleted:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCanceledAndDeactivateOnCompleted:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedAndDestroyOnCanceled:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedAndDeactivateOnCanceled:
	case EInteractionFinishMethod::DeactivateOnCanceled:
	case EInteractionFinishMethod::DeactivateOnCompletedOrCanceled:
	case EInteractionFinishMethod::DeactivateOnCanceledAndDestroyOnCompleted:
	case EInteractionFinishMethod::DeactivateOnCanceledAndReactivateAfterDurationOnCompleted:
	case EInteractionFinishMethod::DeactivateOnCompletedAndDestroyOnCanceled:
	case EInteractionFinishMethod::DeactivateOnCompletedAndReactivateAfterDurationOnCanceled:
		return true;

	// 기본적으로는 false를 반환합니다.
	default: return false;
	}
}

bool UInteractionTargetComponent::IsReactivationEnabled()
{
	// 상호작용 완료 방식에 따라 분기 처리합니다.
	switch (FinishMethod)
	{
	// 다음 경우에는 상호작용 대상이 재활성화될 수 있습니다.
	case EInteractionFinishMethod::ReactivateAfterDurationOnCompleted:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCanceled:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedOrCanceled:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCanceledAndDestroyOnCompleted:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCanceledAndDeactivateOnCompleted:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedAndDestroyOnCanceled:
	case EInteractionFinishMethod::ReactivateAfterDurationOnCompletedAndDeactivateOnCanceled:
	case EInteractionFinishMethod::DeactivateOnCanceledAndReactivateAfterDurationOnCompleted:
	case EInteractionFinishMethod::DeactivateOnCompletedAndReactivateAfterDurationOnCanceled:

		return true;

	default: return false;
	}
}

UInteractionManagerComponent* UInteractionTargetComponent::GetInteractionManager(AController* InController)
{
	return Cast<UInteractionManagerComponent>(
		InController->GetComponentByClass(UInteractionManagerComponent::StaticClass()));
}

AController* UInteractionTargetComponent::GetOtherController(AActor* InActor)
{
	APawn* OtherPawn = Cast<APawn>(InActor);
	if (!OtherPawn) return nullptr;

	AController* OtherController = OtherPawn->GetController();
	if (!OtherController) return nullptr;

	if (!OtherController->IsLocalPlayerController()) return nullptr;

	return OtherController;
}
