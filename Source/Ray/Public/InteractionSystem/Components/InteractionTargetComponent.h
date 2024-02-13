#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionSystem/Libraries/InteractionEnumStruct.h"
#include "InteractionTargetComponent.generated.h"


class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionBegin, APawn*, InteractorPawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnd, EInteractionResult, InteractionResult, APawn*, InteractorPawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionUpdated, float, Alpha, int32, Repeated, APawn*, InteractorPawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionReactivated, APawn*, ForPawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionDeactivated);


UCLASS(ClassGroup=("Ray|Interaction"), meta=(BlueprintSpawnableComponent))
class RAY_API UInteractionTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionTargetComponent();

protected:
	virtual void BeginPlay() override;

	/**
 	* 상호작용 대상 컴포넌트의 기본 설정을 수행하는 함수입니다.
 	* 이 함수는 상호작용 대상 컴포넌트를 소유한 액터에 대한 참조를 설정합니다.
 	*/
	UFUNCTION(Category="Interaction Target|Main")
	void ConstructOwnerEssentials();

	/**
 	* 상호작용 대상의 내부 및 외부 영역을 구성하는 함수입니다.
 	* 이 함수는 상호작용 영역을 정의하는 구 컴포넌트를 생성하고 설정합니다.
 	*/
	UFUNCTION(Category="Interaction Target|Main")
	void ConstructOverlapZones();

	/**
 	* 상호작용 대상의 하이라이트 컴포넌트를 구성하는 함수입니다.
 	* 이 함수는 상호작용 대상에 포함된 특정 씬 컴포넌트를 하이라이트 컴포넌트로 선택합니다.
 	*/
	UFUNCTION(Category="Interaction Target|Main")
	void ConstructHighlightedComponents();

public:
	UFUNCTION(Category="Interaction Target|Overlap Events")
	void OnInnerZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                             bool bFromWeep, const FHitResult& SweepResult);

	UFUNCTION(Category="Interaction Target|Overlap Events")
	void OnInnerZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Category="Interaction Target|Overlap Events")
	void OnOuterZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                             bool bFromWeep, const FHitResult& SweepResult);

	UFUNCTION(Category="Interaction Target|Overlap Events")
	void OnOuterZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/**
 	* 하이라이트를 설정하거나 해제합니다.
 	* @param bIsHighlighted - 하이라이트를 설정할지 여부를 나타내는 플래그.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interaction Target|Visual")
	void SetHighlight(bool bIsHighlighted);

	/**
 	* 위젯 정보를 업데이트합니다.
 	* @param InWidgetMargin - 위젯의 여백을 나타내는 FMargin 값.
 	* @param InScreenRadiusPercent - 화면 반경의 비율을 나타내는 값.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interaction Target|Visual")
	void UpdateWidgetInfo(const FMargin& InWidgetMargin, float InScreenRadiusPercent);

public:
	/**
 	* 상호작용 대상이 현재 상호작용 가능한 상태인지 확인합니다.
 	* 
 	* @return bool - 상호작용 가능한 상태이면 true, 그렇지 않으면 false를 반환합니다.
 	*/
	UFUNCTION(BlueprintPure, Category="Interaction Target|Checks")
	bool IsInteractionEnabled();

	/**
 	* 상호작용이 취소될 때 특정 작업을 수행할지 여부를 결정합니다.
 	*
 	* @return bool - 특정 상호작용 완료 방식에 따라 상호작용이 취소될 때 특정 작업을 수행하는 경우 true를 반환합니다.
 	*/
	UFUNCTION(BlueprintPure, Category="Interaction Target|Checks")
	bool CancelOnRelease();

	/**
 	* 상호작용 대상이 재활성화 가능한지 여부를 결정합니다.
 	*
 	* @return bool - 특정 상호작용 완료 방식에 따라 상호작용 대상이 재활성화될 수 있는 경우 true를 반환합니다.
 	*/
	UFUNCTION(BlueprintPure, Category="Interaction Target|Checks")
	bool IsReactivationEnabled();

public:
	/**
 	* 상호작용이 시작될 때 호출되는 이벤트 처리 함수입니다.
 	* @param InteractorPawn - 상호작용을 시작한 폰.
 	* 이 함수는 네트워크 상에서 상호작용을 비활성화하는 설정이 활성화된 경우,해당 상호작용 대상에 연결된 다른 인터랙터들의 상호작용을 비활성화합니다.
 	*/
	UFUNCTION(Category="Interaction Target|Binded Actions")
	void OnInteractionBeginEvent(APawn* InteractorPawn);

	/**
 	* 상호작용이 종료될 때 호출되는 이벤트 처리 함수입니다.
 	* @param InteractionResult - 상호작용의 결과 (완료, 취소 등).
 	* @param InteractorPawn - 상호작용을 종료한 폰.
 	* 이 함수는 네트워크 상에서 상호작용을 비활성화하는 설정이 활성화된 경우,
 	* 해당 상호작용 대상에 연결된 다른 인터랙터들의 상호작용 상태를 업데이트합니다.
 	*/
	UFUNCTION(Category="Interaction Target|Binded Actions")
	void OnInteractionEndEvent(EInteractionResult InteractionResult, APawn* InteractorPawn);

	/**
 	* 상호작용 대상이 비활성화될 때 호출되는 함수입니다.
 	* 이 함수는 상호작용 대상의 내부 및 외부 영역의 이벤트 생성을 중지하고,
 	* 게임 중에 이러한 영역이 숨겨지도록 설정합니다.
 	*/
	UFUNCTION(Category="Interaction Target|Binded Actions")
	void OnDeactivated();

	/**
 	* 상호작용 대상을 대기 중인 목록에 추가하는 함수입니다.
 	*
 	* 이 함수는 상호작용 대상을 대기 중인 목록에 추가하고, 상호작용 대상의 상호작용 가능 여부를 false로 설정합니다.
 	* 또한, 상호작용 대상에 할당된 모든 컨트롤러의 대응하는 인터랙션 매니저에 대기 중인 상태를 알립니다.
 	*/
	UFUNCTION(Category="Interaction Target|Binded Actions")
	void OnAddedToPendingTarget();

public:
	/**
 	* 상호작용 대상에 인터랙터(플레이어 컨트롤러)를 할당하거나 해제합니다.
 	*
 	* @param bAdd - 인터랙터를 할당할지 여부를 결정하는 불리언 값입니다. true이면 할당, false이면 해제합니다.
 	* @param Interactor - 할당하거나 해제할 플레이어 컨트롤러입니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interaction Target")
	void AssignInteractor(bool bAdd, APlayerController* Interactor);

	/**
 	* 상호작용 대상의 활성화/비활성화 상태를 설정합니다.
 	* 
 	* @param bEnabled - 상호작용 대상을 활성화할지 여부를 결정하는 불리언 값입니다. true이면 활성화, false이면 비활성화합니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interaction Target")
	void EnableInteraction(bool bEnabled);

private:
	UFUNCTION(BlueprintPure, Category="Interaction Target|Utility")
	UInteractionManagerComponent* GetInteractionManager(AController* InController);

	AController* GetOtherController(AActor* InActor);

public:
	// 디버그 모드 활성화 여부. true일 경우 추가적인 디버그 정보가 표시됨.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target")
	bool bEnableDebug = true;

	// 이 컴포넌트를 소유하는 액터에 대한 참조.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Main")
	TObjectPtr<AActor> OwnerReference;

	// 상호작용의 내부 구역을 정의하는 구 컴포넌트.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Main|Components")
	USphereComponent* InnerZone;

	// 상호작용의 외부 구역을 정의하는 구 컴포넌트.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Main|Components")
	USphereComponent* OuterZone;


	// 상호작용이 활성화되어 있는지 여부.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Data")
	bool bInteractionEnabled = true;

	// 상호작용 위젯이 화면에서 차지하는 반경의 비율.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Data")
	float ScreenRadiusPercent;

	// 마지막 상호작용 시점.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Data")
	float LastInteractedTime;

	// 위젯의 여백.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Data")
	FMargin WidgetMargin;

	// 이 컴포넌트에 할당된 상호작용 컨트롤러 목록.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Data")
	TArray<AController*> AssignedInteractors;

	// 마커 대상 컴포넌트.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Data")
	USceneComponent* MarkerTargetComponent;

	// 강조 표시될 구성 요소 목록.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Data")
	TArray<UPrimitiveComponent*> HighlightedComponents;


	// 진행 상태를 표시할지 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	bool bDisplayProgress = true;

	// 삼각형 마커 표시 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	bool bDisplayTriangle = true;

	// POI 아이콘 깜빡임 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	bool bFlashPOI_Icon;

	// 대상 아이콘 깜빡임 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	bool bFlashTargetIcon;

	// 상호작용 아이콘 회전 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	bool bRotateInteractionIcon;

	// 마커 오프셋.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FVector MarkerOffset;

	// 대상 아이콘 색상.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FLinearColor TargetIconColor;

	// POI 아이콘 색상.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FLinearColor POIIconColor;

	// 상호작용 아이콘 배경 색상.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FLinearColor InteractionIconBackgroundColor;

	// 상호작용 아이콘 전경 색상.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FLinearColor InteractionIconForegroundColor;

	// 대상 아이콘 선택기.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FIconSelector TargetIcon;

	// POI 아이콘 선택기.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FIconSelector POIIcon;

	// 상호작용 아이콘 배경 선택기.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FIconSelector InteractionIconBackground;

	// 상호작용 아이콘 전경 선택기.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FIconSelector InteractionIconForeground;

	// 강조 표시 색상.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Highlight Settings")
	FLinearColor HighlightColor;


	// 쿨다운 활성화 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	bool bCooldownEnabled = true;

	// 네트워크 처리 방식.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	EInteractionNetworkMethod NetworkHandleMethod = EInteractionNetworkMethod::KeepEnabled;

	// 상호작용 유형 (탭, 홀드 등).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	EInteractionType InteractionType = EInteractionType::Tap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	EInteractionResponse InteractionResponse = EInteractionResponse::Persistent;

	// 내부 구역 반지름.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	float InnerZoneRadius = 200.0f;

	// 외부 구역 반지름 추가 범위.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	float OuterZoneExtent = 200.0f;

	// 반복 쿨다운 시간.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	float RepeatCoolDown = 1.0f;

	// 반복 횟수 제한.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings", meta=(ClampMin="2.0", ClampMax="30.0", UIMin="2.0", UIMax="30.0"))
	int32 RepeatCount = 2;

	// 홀드 상호작용 시간.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings", meta=(ClampMin="0.0", ClampMax="30.0", UIMin="0.0", UIMax="30.0"))
	float HoldSeconds = 2.0f;


	UPROPERTY(BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	USceneComponent* ComponentToAttach;

	// 각 상호작용 상태에 대한 텍스트.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	TMap<EInteractionState, FText> InteractionText;

	// 상호작용 키를 화면에 표시할지 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Key Settings")
	bool bDisplayInteractionKey = true;

	// 사용자 지정 키를 사용할지 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Key Settings")
	bool bUseCustomKeys = false;

	// 상호작용에 사용할 사용자 지정 키의 목록.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Key Settings")
	TArray<FKey> CustomKeys;

	// 상호작용 완료 후 처리 방식.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|On Finished")
	EInteractionFinishMethod FinishMethod = EInteractionFinishMethod::DestroyOnCompleted;

	// 재활성화 지속 시간.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|On Finished")
	float ReactivationDuration = 2.0f;

public:
	// 상호작용이 시작될 때 호출되는 델리게이트.
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Interaction Target|Delegate|Main")
	FOnInteractionBegin OnInteractionBegin;

	// 상호작용이 종료될 때 호출되는 델리게이트.
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Interaction Target|Delegate|Main")
	FOnInteractionEnd OnInteractionEnd;

	// 상호작용이 업데이트될 때 호출되는 델리게이트.
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Interaction Target|Delegate|Main")
	FOnInteractionUpdated OnInteractionUpdated;

	// 상호작용이 재활성화될 때 호출되는 델리게이트.
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Interaction Target|Delegate|Main")
	FOnInteractionReactivated OnInteractionReactivated;

	// 상호작용이 비활성화될 때 호출되는 델리게이트.
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Interaction Target|Delegate|Main")
	FOnInteractionDeactivated OnInteractionDeactivated;
};


/*
  	
 	 주어진 컴포넌트 이름에 해당하는 마커 컴포넌트를 선택합니다.
 	 @param InComponentName - 선택할 컴포넌트의 이름.
 	
	UFUNCTION(BlueprintCallable, Category="Interaction Target|Main|Visual")
	 void SelectMarkerComponent(const FString& InComponentName);

*/

/* 삭제 한 변수
 *
 *
*	// 상호작용 구역에 첨부할 컴포넌트의 이름.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Interaction Settings")
	FString InteractionZoneComponentToAttach;

	// 강조 표시할 구성 요소의 이름 목록.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Highlight Settings")
	TArray<FString> ComponentsToHighlight;

	// 마커 표시에 사용될 컴포넌트의 이름.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Target|Marker Settings")
	FString MarkerComponentName;
 * 
 */
