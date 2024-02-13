#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionSystem/Libraries/InteractionEnumStruct.h"
#include "InteractionManagerComponent.generated.h"

class UInputAction;
class UUW_InteractionTarget;
class UInteractionTargetComponent;
class UPostProcessComponent;

UCLASS(ClassGroup=("Ray|Interaction"), meta=(BlueprintSpawnableComponent))
class RAY_API UInteractionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/**
 	* 플레이어 관련 필수 컴포넌트를 구성하는 함수입니다.
 	*
 	* 이 함수는 플레이어 컨트롤러를 초기화하고, 포스트 프로세스 컴포넌트를 구성합니다.
 	* 또한, 상호작용 키를 업데이트하기 위한 타이머를 설정합니다.
 	*/
	UFUNCTION(Category="Interaction Manager|Main")
	void ConstructPlayerEssentials();

	/**
 	* 포스트 프로세스 컴포넌트를 구성하는 함수입니다.
 	*
 	* 이 함수는 포스트 프로세스 컴포넌트를 생성하고, 윤곽선을 위한 동적 머티리얼 인스턴스를 설정합니다.
 	*/
	UFUNCTION(Category="Interaction Manager|Main")
	void ConstructPostProcessComponent();

	/**
 	* 위젯 풀을 구성하는 함수입니다.
 	*
 	* 이 함수는 상호작용 타겟 위젯들을 미리 생성하여 위젯 풀에 추가합니다.
 	*/
	UFUNCTION(Category="Interaction Manager|Main")
	void ConstructPooledMarkerWidgets();

	UFUNCTION(BlueprintCallable, Category="Interaction Manager|Main")
	void DebugFunction();

public:
	/**
     * 주변에서 상호작용 가능한 대상을 찾습니다.
     * @return UInteractionTargetComponent* - 가장 가까운 상호작용 가능한 대상을 반환합니다. 상호작용 가능한 대상이 없는 경우 nullptr을 반환합니다.
     */
	UFUNCTION(BlueprintCallable, Category="Interaction Manager|Interactable")
	UInteractionTargetComponent* FindNearInteractable();

	/**
 	* 최적의 상호작용 대상을 업데이트합니다.
 	* @param NewTarget  새로운 상호작용 대상 컴포넌트.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interaction Manager|Interactable")
	void UpdateBestInteractable(UInteractionTargetComponent* NewTarget);

	/**
 	* 상호작용에 사용되는 키를 업데이트하는 함수입니다.
 	*
 	* 이 함수는 로컬 플레이어 컨트롤러의 상호작용 입력 액션에 매핑된 키들을 조회하고, 그 결과를 InteractionKeys 배열에 저장합니다. 또한, 이 함수는 관련 타이머 핸들을 정리합니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interaction Manager|Interactable")
	void UpdateInteractionKeys();

	/**
 	* 위젯 풀에서 현재 사용되지 않는 위젯을 찾는 함수입니다.
 	*
 	* 이 함수는 위젯 풀의 모든 위젯을 순회하며, 연결된 상호작용 대상이 없는 위젯을 찾습니다.
 	* 사용 가능한 위젯이 발견되면 해당 위젯을 반환하고, 그렇지 않으면 nullptr을 반환합니다.
 	*
 	* @return UUW_InteractionTarget* - 사용 가능한 위젯 또는 nullptr.
 	*/
	UFUNCTION(BlueprintPure, Category="Interaction Manager|Interactable")
	UUW_InteractionTarget* FindEmptyWidget();

	/**
 	* 주어진 상호작용 대상에 대응하는 위젯을 찾습니다.
 	* @param InteractionTarget 찾고자 하는 상호작용 대상 컴포넌트.
 	* @return UUW_InteractionTarget* 해당 상호작용 대상과 연결된 위젯을 반환합니다. 해당 위젯이 없는 경우 nullptr을 반환합니다.
 	*/
	UFUNCTION(BlueprintPure, Category="Interaction Manager|Interactable")
	UUW_InteractionTarget* FindWidgetByInteractionTarget(UInteractionTargetComponent* InteractionTarget);

	/**
 	* 특정 상호작용 대상 컴포넌트가 상호작용 가능한지 여부를 확인합니다.
 	* @param ItemToFind  확인할 상호작용 대상 컴포넌트.
 	* @return bool  상호작용이 가능하면 true를, 그렇지 않으면 false를 반환합니다.
 	*/
	UFUNCTION(BlueprintPure, Category="Interaction Manager|Interactable")
	bool IsInteractable(UInteractionTargetComponent* ItemToFind);

	/**
 	* 현재 상호작용 대상에 대한 상호작용 키를 가져옵니다.
 	* @param OutKeys - 상호작용에 사용할 키 배열을 반환합니다.
 	* @return bool - 상호작용 키가 유효하면 true를, 그렇지 않으면 false를 반환합니다.
 	*/
	UFUNCTION(BlueprintPure, Category="Interaction Manager|Interactable")
	bool GetInteractionKeys(TArray<FKey>& OutKeys);

public:
	/*상호작용을 시도합니다.*/
	UFUNCTION(BlueprintCallable, Category="Interaction Manager|Interaction")
	void TryTakeInteraction();

	/**
 	* 상호작용이 완료된 후 적용할 메소드를 실행합니다.
 	* 이 함수는 상호작용 결과에 따라 다른 처리를 수행합니다.
 	* @param InteractionTarget - 상호작용이 완료된 대상 컴포넌트입니다.
 	* @param InteractionResult - 상호작용의 결과를 나타내는 열거형 값입니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interaction Manager|Interaction")
	void ApplyFinishMethod(UInteractionTargetComponent* InteractionTarget, EInteractionResult InteractionResult);

	UFUNCTION(BlueprintCallable, Category="Interaction Manager|Interaction")
	void ReceiveAnyKey(FKey Key);

protected:
	/**
 	* 탭 상호작용을 처리하는 함수입니다.
 	* 사용자가 상호작용 키를 탭할 때 이 함수가 호출됩니다.
 	*/
	UFUNCTION()
	void TapInteraction();

	/**
 	* '홀드' 유형의 상호작용을 처리하는 함수입니다.
 	* 이 함수는 상호작용 키가 지속적으로 눌려 있을 때 호출되며, 상호작용의 진행 상태를 관리합니다.
 	* 상호작용이 완료되거나 취소될 때까지 지속적으로 호출됩니다.
 	*/
	UFUNCTION()
	void HoldInteraction();

	/**
 	* 반복 상호작용을 처리하는 함수입니다.
 	* 
 	* 이 함수는 사용자가 상호작용 키를 연속적으로 누를 때 호출됩니다. 상호작용 대상이 '반복' 유형으로 설정된 경우,
 	* 이 함수는 각 키 입력 사이의 쿨다운을 관리하고, 반복된 횟수에 따라 상호작용의 상태를 업데이트합니다.
 	* 상호작용 키가 눌릴 때마다 반복 횟수가 증가하며, 설정된 반복 횟수에 도달하면 상호작용이 완료 처리됩니다.
 	* 또한, 쿨다운이 활성화된 경우, 키 입력 간 쿨다운 시간을 감소시키며, 쿨다운 시간이 완료되면 상호작용을 취소 처리합니다.
 	*/
	UFUNCTION()
	void RepeatInteraction();

public:
	/**
 	* 상호작용 대상이 파괴될 때 호출되는 함수입니다.
 	*
 	* @param DestroyedActor 파괴된 액터.
 	* 이 함수는 파괴된 액터에서 상호작용 대상 컴포넌트를 찾아 관련 목록에서 제거합니다.
 	* 또한, 클라이언트 측에서 필요한 처리를 수행하고, 파괴된 액터의 OnDestroyed 이벤트를 정리합니다.
 	*/
	UFUNCTION(Category="Interactor Manager|Targets")
	void OnInteractionTargetDestroyed(AActor* DestroyedActor);

	/**
 	* 상호작용 대상을 대기 중인 목록에 추가하는 함수입니다.
 	*
 	* @param InteractionTarget 추가할 상호작용 대상 컴포넌트.
 	* 이 함수는 주어진 상호작용 대상을 대기 중인 목록에 추가하며, 관련 서버 함수를 호출하여
 	* 관심 지점 및 상호작용 대상 목록을 업데이트합니다. 또한, 대기 중인 대상들을 주기적으로 체크하기 위한
 	* 타이머를 설정합니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Targets")
	void AddToPendingTargets(UInteractionTargetComponent* InteractionTarget);

	/**
 	* 대기 중인 상호작용 대상이 재활성화되었을 때 호출되는 함수입니다.
 	*
 	* @param InteractionTarget 재활성화될 상호작용 대상 컴포넌트.
 	* 이 함수는 대상을 대기 목록에서 제거하고, 상호작용 가능 상태로 설정합니다.
 	* 또한, 플레이어 컨트롤러의 폰이 대상의 영역 안에 있는지 확인하고, 필요한 경우 서버에 상호작용 대상과 관심 지점을 업데이트합니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Targets")
	void OnInteractionTargetReactivated(UInteractionTargetComponent* InteractionTarget);

	/**
 	* 특정 상호작용 대상을 비활성화된 대상 목록에 추가하는 함수입니다.
 	* @param InteractionTarget - 비활성화할 상호작용 대상 컴포넌트.
 	* 이 함수는 주어진 상호작용 대상을 비활성화된 대상 목록에 추가하고 서버에 해당 대상의 관심 지점 및 상호작용 대상 목록을 업데이트하도록 요청합니다.
 	* 또한, 상호작용 대상 컴포넌트의 비활성화 관련 이벤트를 호출합니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Targets")
	void AddToDeactivatedTargets(UInteractionTargetComponent* InteractionTarget);

	/**
 	* 대기 중인 상호작용 대상들을 체크하고 필요에 따라 재활성화하는 함수입니다.
 	*
 	* 이 함수는 대기 중인 상호작용 대상 목록을 순회하며 각 대상의 마지막 상호작용 시간과 현재 시간의 차이를 비교합니다.
 	* 만약 차이가 대상의 재활성화 기간보다 크거나 같다면, 해당 대상을 재활성화합니다.
 	* 대기 목록이 비어있는 경우, 타이머를 무효화합니다.
 	*/
	UFUNCTION(Category="Interactor Manager|Targets")
	void CheckForPendingTargets();

	/**
 	* 비활성화된 상호작용 대상 목록에서 특정 대상을 제거하는 함수입니다.
 	*
 	* @param InteractionTarget 제거할 상호작용 대상 컴포넌트.
 	* 이 함수는 비활성화된 상호작용 대상 목록에서 지정된 대상을 제거합니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Targets")
	void RemoveFromDeactivatedTargets(UInteractionTargetComponent* InteractionTarget);

	/**
 	* 클라이언트 사이드에서 새로 선택된 상호작용 대상을 처리합니다.
 	* @param bIsSelected - 대상이 선택되었는지 여부.
 	* @param NewTarget - 새로 선택된 상호작용 대상.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Targets")
	void OnNewTargetSelectedClientSide(bool bIsSelected, UInteractionTargetComponent* NewTarget);

	/**
 	* 상호작용 대상의 하이라이트 상태를 설정하는 함수입니다.
 	*
 	* @param InteractionTarget 하이라이트 상태를 변경할 상호작용 대상 컴포넌트.
 	* @param bIsHighlighted 하이라이트 상태를 활성화할지 여부.
 	* 이 함수는 지정된 상호작용 대상의 하이라이트 상태를 변경합니다. 대상의 하이라이트 색상을 설정하고,
 	* 대상 컴포넌트의 하이라이트 상태를 업데이트합니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Targets")
	void SetTargetHighlighted(UInteractionTargetComponent* InteractionTarget, bool bIsHighlighted);

public:
	/**
 	* 서버 측에서 관심 지점 목록을 업데이트합니다.
 	* @param bAdd  관심 지점을 추가할지 여부를 나타냅니다.
 	* @param InteractionTarget  업데이트할 관심 지점 상호작용 대상 컴포넌트입니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Network")
	void OnPointOfInterestUpdatedServerSide(bool bAdd, UInteractionTargetComponent* InteractionTarget);

	/**
 	* 클라이언트 측에서 관심 지점 목록을 업데이트합니다.
 	* @param bAdd - 관심 지점을 추가할지 여부를 나타냅니다.
 	* @param InteractionTarget - 업데이트할 관심 지점 상호작용 대상 컴포넌트입니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Network")
	void OnPointOfInterestUpdatedClientSide(bool bAdd, UInteractionTargetComponent* InteractionTarget);

	/**
 	* 서버 측에서 상호작용 대상 목록을 업데이트합니다.
 	* @param bAdd - 상호작용 대상을 추가할지 여부를 나타냅니다.
 	* @param InteractionTarget - 업데이트할 상호작용 대상 컴포넌트입니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Network")
	void OnInteractionTargetUpdatedServerSide(bool bAdd, UInteractionTargetComponent* InteractionTarget);

	/**
 	* 클라이언트 측에서 상호작용 대상 목록을 업데이트합니다.
 	* @param bAdd  상호작용 대상을 추가할지 여부를 나타냅니다.
 	* @param InteractionTarget  업데이트할 상호작용 대상 컴포넌트입니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Network")
	void OnInteractionTargetUpdatedClientSide(bool bAdd, UInteractionTargetComponent* InteractionTarget);

public:
	/**
 	* 서버에서 상호작용 시작을 처리하는 함수입니다.
 	* 이 함수는 상호작용이 시작될 때 호출되며, 상호작용 대상의 상호작용 시작 이벤트를 방송합니다.
 	* @param InteractionTarget - 상호작용이 시작되는 대상 컴포넌트입니다.
 	*/
	UFUNCTION(Server, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ServerOnInteractionBegin(UInteractionTargetComponent* InteractionTarget);

	/**
 	* 서버에서 상호작용 업데이트 이벤트를 처리하는 함수입니다.
 	* 이 함수는 서버에서 상호작용이 업데이트될 때 호출되어, 상호작용 대상에 변경 사항을 알립니다.
 	* @param InteractionTarget - 업데이트된 상호작용 대상 컴포넌트입니다.
 	* @param Alpha - 상호작용 진행률을 나타내는 값입니다.
 	* @param InRepeated - 반복 상호작용의 경우, 반복된 횟수를 나타냅니다.
 	* @param InteractorPawn - 상호작용을 수행하는 플레이어 폰입니다.
 	*/
	UFUNCTION(Server, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ServerOnInteractionUpdated(UInteractionTargetComponent* InteractionTarget, float Alpha, int32 InRepeated,
	                                APawn* InteractorPawn);

	/**
 	* 서버에서 상호작용 대상 목록을 업데이트합니다.
 	* @param bAdd - 상호작용 대상을 추가할지 여부를 나타냅니다.
 	* @param InteractionTarget - 업데이트할 상호작용 대상 컴포넌트입니다.
 	*/
	UFUNCTION(Server, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ServerUpdateInteractionTargets(bool bAdd, UInteractionTargetComponent* InteractionTarget);

	/**
 	* 서버에서 관심 지점 목록을 업데이트합니다.
 	* @param bAdd  관심 지점을 추가할지 여부를 나타냅니다.
 	* @param InteractionTarget  업데이트할 관심 지점 상호작용 대상 컴포넌트입니다.
 	*/
	UFUNCTION(Server, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ServerUpdatePointOfInterests(bool bAdd, UInteractionTargetComponent* InteractionTarget);

	/**
 	* 서버에서 상호작용 완료를 처리하는 함수입니다.
 	* 이 함수는 상호작용이 완료되었을 때 호출되며, 주어진 상호작용 대상에 대한 상호작용 완료 처리를 수행합니다.
 	* @param InteractionTarget - 상호작용이 완료된 대상 컴포넌트입니다.
 	* @param InteractionResult - 상호작용의 결과를 나타내는 열거형 값입니다.
 	*/
	UFUNCTION(Server, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ServerOnInteractionFinished(UInteractionTargetComponent* InteractionTarget, EInteractionResult InteractionResult);

	/**
 	* 서버에서 상호작용 대상에 인터랙터(플레이어 컨트롤러)를 할당하거나 해제하는 요청을 처리합니다.
 	* @param InteractionTarget - 인터랙터를 할당하거나 해제할 상호작용 대상 컴포넌트입니다.
 	* @param bAdd - 인터랙터를 할당할지(true), 해제할지(false)를 나타냅니다.
 	*/
	UFUNCTION(Server, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ServerRequestAssignInteractor(bool bAdd, UInteractionTargetComponent* InteractionTarget);

	/**
 	* 클라이언트에서 상호작용 키 입력을 체크하는 함수입니다.
 	*/
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ClientCheckPressedKey();

	/**
 	* 클라이언트 사이드에서 새로운 상호작용 대상을 설정합니다.
 	* @param NewTarget - 설정할 새로운 상호작용 대상.
 	* @param bIsSelected - 대상이 선택되었는지 여부를 나타내는 플래그.
 	*/
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ClientSetNewTarget(bool bIsSelected, UInteractionTargetComponent* NewTarget);

	/**
 	* 클라이언트 측에서 상호작용 관련 데이터를 초기화하는 함수입니다.
 	*
 	* 이 함수는 상호작용 키 입력 상태, 마지막으로 눌린 키, 홀드 시간, 반복된 상호작용 횟수 등을 초기화합니다.
 	* 이는 상호작용이 완료되거나 취소된 후에 호출되어 상태를 리셋하는 데 사용됩니다.
 	*/
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ClientResetData();

	/**
 	* 클라이언트에서 관심 지점 목록을 업데이트하는 함수입니다.
 	* @param bAdd - 관심 지점을 추가할지 여부를 나타냅니다.
 	* @param InteractionTarget - 업데이트할 관심 지점 상호작용 대상 컴포넌트입니다.
 	*/
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ClientUpdatePointOfInterests(bool bAdd, UInteractionTargetComponent* InteractionTarget);

	/**
 	* 클라이언트에서 상호작용 대상 목록을 업데이트하는 함수입니다.
 	* @param bAdd  상호작용 대상을 추가할지 여부를 나타냅니다.
 	* @param InteractionTarget  업데이트할 상호작용 대상 컴포넌트입니다.
 	*/
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ClientUpdateInteractionTargets(bool bAdd, UInteractionTargetComponent* InteractionTarget);

	/**
	 * 클라이언트 측에서 상호작용 대상이 파괴될 때 호출되는 함수입니다.
	 *
	 * @param InteractionTarget 파괴된 상호작용 대상 컴포넌트.
	 * 이 함수는 현재 상호작용 마커 위젯의 대상을 null로 업데이트하여,
	 * 상호작용 대상이 더 이상 유효하지 않음을 반영합니다.
	 */
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category="Interactor Manager|Network Events")
	void ClientOnInteractionTargetDestroyed(UInteractionTargetComponent* InteractionTarget);

	/**
 	* 상호작용 업데이트 이벤트를 처리하는 함수입니다.
 	* 이 함수는 상호작용이 업데이트될 때마다 호출되어, 상호작용 대상에 변경 사항을 알립니다.
 	* @param InteractionTarget - 업데이트된 상호작용 대상 컴포넌트입니다.
 	* @param Alpha - 상호작용 진행률을 나타내는 값입니다.
 	* @param InRepeated - 반복 상호작용의 경우, 반복된 횟수를 나타냅니다.
 	*/
	UFUNCTION(BlueprintCallable, Category="Interactor Manager|Network Events")
	void OnInteractionUpdated(UInteractionTargetComponent* InteractionTarget, double Alpha, int32 InRepeated);

public:
	// 상호작용 가능한 대상에 대한 윤곽선을 그릴 때 사용되는 머티리얼.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Manager|SetUp")
	UMaterialInterface* M_OutlineMaterial;

	// 상호작용 위젯 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Manager|SetUp")
	TSubclassOf<UUW_InteractionTarget> WBP_InteractionTargetClass;

	// 상호작용 키가 방금 눌렸는지 여부를 나타냄.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	bool bKeyJustPressed = false;

	// 게임패드를 사용 중인지 여부를 나타냄.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	bool bIsGamepad;

	// 현재 상호작용 중인지 여부를 나타냄.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	bool bIsInteracting = false;

	// 상호작용 유지(홀드) 시간.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	float CurrentHoldTime = 0.0f;

	// 반복 상호작용 횟수.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	int32 Repeated;

	// 반복 상호작용 쿨다운 시간.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	float RepeatCoolDown;

	// 마지막으로 눌린 키.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	FKey LastPressedKey;

	// 상호작용을 관리하는 플레이어 컨트롤러.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	APlayerController* OwnerController;

	// 윤곽선을 동적으로 그리기 위한 머티리얼 인스턴스.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	UMaterialInstanceDynamic* OutlineDynamicMaterial;

	// 포스트 프로세스 컴포넌트.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Interaction Manager|Data")
	UPostProcessComponent* PostProcessComponent;

	// 상호작용 키 업데이트 타이머 핸들.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	FTimerHandle BeginUpdateKeysTimerHandle;

	// 대기 중인 상호작용 대상을 체크하는 타이머 핸들.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	FTimerHandle PendingTargetTimerHandle;

	// 현재 상호작용 마커 위젯.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	TObjectPtr<UUW_InteractionTarget> CurrentInteractionMarker;

	// 상호작용에 사용되는 키 배열.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	TArray<FKey> InteractionKeys;

	// 최적의 상호작용 대상.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	UInteractionTargetComponent* BestInteractionTarget;

	// 상호작용 대상 목록.
	UPROPERTY(BlueprintReadOnly, Category="Interaction Manager|Data")
	TArray<UInteractionTargetComponent*> InteractionTargets;

	// 관심 지점 목록.
	UPROPERTY(BlueprintReadOnly, Category="Interaction Manager|Data")
	TArray<UInteractionTargetComponent*> PointOfInterests;

	// 대기 중인 상호작용 대상 목록.
	UPROPERTY(BlueprintReadOnly, Category="Interaction Manager|Data")
	TArray<UInteractionTargetComponent*> PendingTargets;

	// 비활성화된 상호작용 대상 목록.
	UPROPERTY(BlueprintReadOnly, Category="Interaction Manager|Data")
	TArray<UInteractionTargetComponent*> DeactivatedTargets;

	// 상호작용 위젯 풀.
	UPROPERTY(BlueprintReadWrite, Category="Interaction Manager|Data")
	TArray<UUW_InteractionTarget*> WidgetPool;

	// 디버그 모드 활성화 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Manager|Main")
	bool bDebug = false;

	// 기본 위젯 풀 사이즈.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Manager|Main")
	int32 DefaultWidgetPoolSize = 3;

	// 대기 중인 상호작용 대상 체크 간격.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Manager|Main", meta=(ClampMin="0.1", ClampMax="1.0", UIMin="0.1", UIMax="1.0"))
	float PendingTargetCheckInterval = 0.35f;

	// 상호작용에 사용되는 입력 액션.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Manager|Main")
	UInputAction* InteractionInputAction;

	// 위젯 화면 여백.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Manager|Marker Settings")
	FMargin WidgetScreenMargin;

	// 화면 반경 백분율.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction Manager|Marker Settings", meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float ScreenRadiusPercent = 0.5f;
};
