#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "InventoryCore.generated.h"


class UPlayerEquipmentComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshInventory, EInventoryPanels, Panel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemovedFromInventoryArray, FItemData, ItemData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddedToInventoryArray, FItemData, ItemData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHighlightInventorySlot, int32, SlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedActivePanel, EInventoryPanels, NewPanel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedMaxWeight);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedCurrentWeight);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangedOwnerGold);


UCLASS(Blueprintable, BlueprintType, ClassGroup=("Ray|Inventory"), meta=(BlueprintSpawnableComponent))
class RAY_API UInventoryCore : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryCore();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Initialize")
	virtual void InitializeInventory();

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Initialize")
	void BuildInventory(EInventoryPanels Panel);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Initialize")
	void ApplyChangesToInventoryArray(EInventoryPanels Panel, const TArray<FItemData>& ApplyInventory);

public:
	UFUNCTION()
	void RandomizeInitialItems();

	UFUNCTION()
	void BuildInitialInventory();

public:
	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void AddItemToInventoryArray(FItemData& ItemData, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void RemoveItemFromInventoryArray(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	virtual bool TransferItemFromInventory(UPARAM(ref) FItemData& ItemData, const FItemData& InSlotData, EInputMethod Method,
	                                       UInventoryCore* Sender, FText& OutFailureMessage);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	virtual bool TransferItemFromEquipment(const FItemData& ItemData, const FItemData& InSlotData, EInputMethod Method,
	                               UPlayerEquipmentComponent* Sender, FText& OutFailureMessage);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void SwapItemsInInventory(const FItemData& FirstItem, const FItemData& SecondItem);


	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void AddToStackInInventory(FItemData& ItemData, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void AddGoldToOwner(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void RemoveGoldFromOwner(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void AddWeightToInventory(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void RemoveWeightFromInventory(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void SortInventory(ESortMethod SortMethod, EInventoryPanels SinglePanel, bool bEveryPanel);

	//ToDo : 네트워크 동기화를 해야함
	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Inventory")
	void ChangeInventorySize(EInventoryPanels Panel, int32 NewSize);

protected:
	void ApplyChangesAndBroadcast(const FItemData& ItemData, EInventoryPanels InventoryPanel);

	void BroadcastInventoryChanges(EInventoryPanels InventoryPanel, const FItemData& ItemData);

	void UpdateNetworkedPlayers(EInventoryPanels InventoryPanel, const FItemData& ItemData);

	void SortSingleInventory(EInventoryPanels Panel, ESortMethod SortMethod);

public:
	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Item")
	void ModifyItemValue(FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Item")
	void RemoveItemQuantity(const FItemData& ItemData, int32 Quantity);

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Item")
	FItemData GetItemBySlot(EInventoryPanels Panel, int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Item")
	EItemRemoveType GetItemRemoveType(const FItemData& ItemData);

public:
	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|User Interface")
	void SwitchActivePanel(EInventoryPanels Panel);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|User Interface")
	virtual void SplitItemsInInventory(UInventoryCore* Sender, const FItemData& ItemData, const FItemData& InSlotData,
	                                   const FItemData& StackableLeft, EInputMethod InputMethod, EItemDestination Initiator,
	                                   EItemDestination Destination);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|User Interface")
	virtual void ConfirmationPopupAccepted(UInventoryCore* Sender, const FItemData& ItemData, const FItemData& InSlotData,
	                                       EInputMethod InputMethod, EItemDestination Initiator,
	                                       EItemDestination Destination);

public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Player Inventory|Network Replication Events")
	void ServerSortInventory(UInventoryCore* Inventory, ESortMethod SortMethod, EInventoryPanels SinglePanel,
	                         bool bEveryPanel);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ServerTransferItemFromInventory(UInventoryCore* Receiver, const FItemData& ItemData, FItemData InSlotData,
	                                     EInputMethod InputMethod, UInventoryCore* Sender);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ServerTransferItemFromEquipment(const FItemData& ItemData, FItemData InSlotData,
	                                     EInputMethod InputMethod, UPlayerEquipmentComponent* Sender);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ServerMoveItemToSlot(UInventoryCore* Inventory, EInventoryPanels Panel, int32 MoveFrom, int32 MoveTo);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ServerAddItemToInventory(UInventoryCore* Inventory, const FItemData& ItemData, int32 SlotIndex);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ServerRemoveItemFromInventory(UInventoryCore* Inventory, const FItemData& ItemData);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ServerSplitItemsInInventory(UInventoryCore* Receiver, UInventoryCore* Sender, const FItemData& ItemData,
	                                 FItemData InSlotData, const FItemData& StackableLeft, EInputMethod InputMethod,
	                                 EItemDestination Initiator, EItemDestination Destination);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ServerConfirmationPopUpAccepted(UInventoryCore* Receiver, UInventoryCore* Sender, const FItemData& ItemData,
	                                     FItemData InSlotData, EInputMethod InputMethod,
	                                     EItemDestination Initiator, EItemDestination Destination);


	// 클라이언트에게 아이템이 추가되었음을 알리는 함수
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ClientUpdateAddedItem(const FItemData& ItemData, UInventoryCore* Inventory);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ClientUpdateRemovedItem(const FItemData& ItemData, UInventoryCore* Inventory);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ClientUpdateItems(UInventoryCore* InventoryCore, EInventoryPanels InventoryPanel,
	                       const TArray<FItemData>& Array);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void ClientTransferItemReturnValue(bool bSuccess, const FString& FailureMessage);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	virtual void ClientInitializeInventory();

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void UpdateViewersItem(const FItemData& ItemData, bool bRemove);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void UpdateViewersInventory(EInventoryPanels Panel);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void UpdateAddedItemInInventory(const FItemData& ItemData, UInventoryCore* Inventory);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void UpdateRemovedItemInInventory(const FItemData& ItemData, UInventoryCore* Inventory);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void AddViewer(APlayerState* PlayerState, UInventoryCore* InventoryCore);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory Core Event|Network Replication")
	void RemoveViewer(APlayerState* PlayerState, UInventoryCore* InventoryCore);

public:
	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	FInventory& GetInventoryRefByPanel(EInventoryPanels Panel);

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	void GetInventoryAndSize(EInventoryPanels Panel, TArray<FItemData>& OutInventory, int32& OutSize);

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	bool GetEmptyInventorySlot(const FItemData& ItemData, int32& OutEmptySlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory Core Event|Utility")
	FItemData RandomizeItemParameters(const FItemData& ItemData);

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	bool HasEnoughGold(const FItemData& ItemData);

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	bool IsInventoryOverweight();

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	bool GetItemByID(FString ItemID, EInventoryPanels InSpecifiedPanel, FItemData& OutItem);

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	bool GetItemByData(const FItemData& ItemData, FItemData& OutItemData);

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	TArray<FItemData> GetCombinedInventories();

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	TArray<FItemData> GetItemsOfSpecifiedType(EItemType ItemType);

	UFUNCTION(BlueprintPure, Category="Inventory Core Event|Utility")
	int32 GetAmountOfEmptySlots(EInventoryPanels Panel);


	/*RepNotify*/
public:
	UFUNCTION()
	void OnRep_MaxInventoryWeight();

	UFUNCTION()
	void OnRep_OwnerGold();

	UFUNCTION()
	void OnRep_CurrentInventoryWeight();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|SetUp")
	TArray<UDataTable*> AllItemsFromDT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|SetUp")
	TArray<FSingleDTItem> SingleDTItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|SetUp")
	TArray<FRandomizedLootTable> RandomizedItemsData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|SetUp")
	FText MessageNotEnoughGold;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|SetUp")
	FText MessageInventoryFull;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|Config")
	bool bSortInitialItems = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Core|Config")
	bool bRefreshOnClosingWidget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|Config")
	bool bUseInventorySize = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|Config")
	bool bCheckOwnerGold = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|Config")
	bool bCheckRemoveType = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core|Config")
	bool bUseInventoryWeight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Core|Config",
		ReplicatedUsing = OnRep_MaxInventoryWeight)
	double MaxInventoryWeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Core|Config")
	ESortMethod DefaultSortingMethod = ESortMethod::ByType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Core|Config")
	double ValueModifier = 1.0f;

public:
	// 현재 뷰어 목록을 나타내는 배열
	UPROPERTY(BlueprintReadWrite, Category = "Inventory Core|Network Replication")
	TArray<APlayerState*> CurrentViewers;

public:
	UPROPERTY(BlueprintReadWrite, Category="Inventory Core")
	EInventoryPanels ActivePanel = EInventoryPanels::P1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core", ReplicatedUsing=OnRep_OwnerGold)
	double OwnerGold = 1000.0f;

	UPROPERTY(BlueprintReadWrite, Category="Inventory Core", ReplicatedUsing=OnRep_CurrentInventoryWeight)
	double CurrentInventoryWeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Core")
	TMap<EInventoryPanels, FInventory> Inventories;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory Core|Delegate")
	FOnRefreshInventory OnRefreshInventory;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory Core|Delegate")
	FOnRemovedFromInventoryArray OnRemovedFromInventoryArray;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory Core|Delegate")
	FOnAddedToInventoryArray OnAddedToInventoryArray;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory Core|Delegate")
	FOnHighlightInventorySlot OnHighlightInventorySlot;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory Core|Delegate")
	FOnSwitchedActivePanel OnSwitchedActivePanel;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory Core|Delegate")
	FOnChangedMaxWeight OnChangedMaxWeight;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory Core|Delegate")
	FOnChangedCurrentWeight OnChangedCurrentWeight;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Inventory Core|Delegate")
	FOnChangedOwnerGold OnChangedOwnerGold;
};
