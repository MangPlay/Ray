#include "InventorySystem/Components/InventoryCore.h"

#include "InventorySystem/Components/PlayerEquipmentComponent.h"
#include "InventorySystem/Components/PlayerInventoryComponent.h"
#include "InventorySystem/Libraries/InventoryFunctionLibrary.h"

#include "GameFramework/PlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "InventorySystem/Objects/MainItem.h"
#include "UI/WidgetSystemLibrary.h"
#include "UI/WidgetController/InventoryWidgetController.h"


UInventoryCore::UInventoryCore()
{
	// 게임이 시작될 때 초기화되고 매 프레임마다 틱되도록 이 구성 요소를 설정합니다.
	// 필요하지 않은 경우 이러한 기능을 꺼서 성능을 향상시킬 수 있습니다.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	MessageNotEnoughGold = FText::FromString(TEXT("Inventory has Not enough gold."));
	MessageInventoryFull = FText::FromString(TEXT("Inventory is full."));
}

void UInventoryCore::BeginPlay()
{
	Super::BeginPlay();

	InitializeInventory();
}

void UInventoryCore::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 변수 리플리케이션 설정
	DOREPLIFETIME(UInventoryCore, MaxInventoryWeight);
	DOREPLIFETIME(UInventoryCore, OwnerGold);
	DOREPLIFETIME(UInventoryCore, CurrentInventoryWeight);
}

void UInventoryCore::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryCore::InitializeInventory()
{
	ClientInitializeInventory();

	if (GetOwner()->HasAuthority())
	{
		for (TTuple<EInventoryPanels, FInventory> Inventory : Inventories)
		{
			BuildInventory(Inventory.Key);
		}
		BuildInitialInventory();
	}
}

void UInventoryCore::BuildInventory(EInventoryPanels Panel)
{
	FInventory& InventoryDataRef = GetInventoryRefByPanel(Panel);

	if (bUseInventorySize)
	{
		InventoryDataRef.InventoryArray.SetNum(InventoryDataRef.InventorySize);
		for (int i = 0; i < InventoryDataRef.InventorySize - 1; ++i)
		{
			InventoryDataRef.InventoryArray[i].Index = i;
			InventoryDataRef.InventoryArray[i].bIsEquipped = false;
		}
	}
	else
	{
		TArray<FItemData> EmptyInventory;
		for (int i = 0; i < InventoryDataRef.InventoryArray.Num(); ++i)
		{
			if (InventoryDataRef.InventoryArray[i].IsItemClassValid())
			{
				const int32 AddIndex = EmptyInventory.Add(InventoryDataRef.InventoryArray[i]);
				EmptyInventory[AddIndex].Index = i;
				EmptyInventory[AddIndex].bIsEquipped = false;
			}
		}
		InventoryDataRef.InventoryArray = EmptyInventory;
	}
}

void UInventoryCore::ApplyChangesToInventoryArray(EInventoryPanels Panel, const TArray<FItemData>& ApplyInventory)
{
	if (Inventories.Contains(Panel))
	{
		Inventories[Panel].InventoryArray = ApplyInventory;
	}
	else
	{
		Inventories[EInventoryPanels::P1].InventoryArray = ApplyInventory;
	}
}

void UInventoryCore::ClientInitializeInventory_Implementation()
{
	if (!UKismetSystemLibrary::IsStandalone(this))
	{
		for (const TTuple<EInventoryPanels, FInventory> Inventory : Inventories)
		{
			BuildInventory(Inventory.Key);
		}
	}
}

void UInventoryCore::RandomizeInitialItems()
{
	TArray<FItemData> DrawnItems;
	TArray<FItemData> NotDrawnItems;

	for (const FRandomizedLootTable& RandomizedLootTable : RandomizedItemsData)
	{
		DrawnItems.Empty();
		NotDrawnItems.Empty();

		if (!IsValid(RandomizedLootTable.DataTable)) continue;

		TArray<FName> RowNames = RandomizedLootTable.DataTable->GetRowNames();

		for (const FName RowName : RowNames)
		{
			FItemData ItemData = *RandomizedLootTable.DataTable->FindRow<FItemData>(RowName,TEXT(""));

			if (RandomizedLootTable.MaxLootItems <= DrawnItems.Num()) break;

			if (!ItemData.IsItemClassValid()) continue;

			const UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject();
			if (MainItemCDO->DropPercentage >= FMath::RandRange(0, 100))
			{
				DrawnItems.Add(ItemData);
				ItemData = RandomizeItemParameters(ItemData);

				//인벤토리에 아이템 추가 [빈 슬롯이 있는 경우]
				int32 SlotIndex;
				if (GetEmptyInventorySlot(ItemData, SlotIndex))
				{
					AddItemToInventoryArray(ItemData, SlotIndex);
					AddWeightToInventory(ItemData);
				}
			}
			else
			{
				NotDrawnItems.Add(ItemData);
			}
		}

		if (DrawnItems.Num() < RandomizedLootTable.MinLootItems)
		{
			for (int i = 1; i <= RandomizedLootTable.MinLootItems - DrawnItems.Num(); ++i)
			{
				const int32 NotDrawnLastIndex = NotDrawnItems.Num() - 1;
				if (NotDrawnLastIndex >= 0)
				{
					const int32 RandomIndex = FMath::RandRange(0, NotDrawnLastIndex);
					FItemData ItemData = NotDrawnItems[RandomIndex];
					NotDrawnItems.RemoveAt(RandomIndex);
					ItemData = RandomizeItemParameters(ItemData);

					//인벤토리에 아이템 추가 [빈 슬롯이 있는 경우]
					int32 SlotIndex;
					if (GetEmptyInventorySlot(ItemData, SlotIndex))
					{
						AddItemToInventoryArray(ItemData, SlotIndex);
						AddWeightToInventory(ItemData);
					}
				}
			}
		}
	}
}

void UInventoryCore::BuildInitialInventory()
{
	RandomizeInitialItems();

	for (UDataTable* ItemsFromDT : AllItemsFromDT)
	{
		if (!IsValid(ItemsFromDT)) continue;

		// 모든 DataTable 행을 인벤토리 배열에 추가합니다.
		TArray<FName> RowNames = ItemsFromDT->GetRowNames();
		for (const FName RowName : RowNames)
		{
			FItemData ItemData = *ItemsFromDT->FindRow<FItemData>(RowName,TEXT(""));
			if (!ItemData.IsItemClassValid()) continue;

			int32 SlotIndex;
			if (GetEmptyInventorySlot(ItemData, SlotIndex))
			{
				AddItemToInventoryArray(ItemData, SlotIndex);
				AddWeightToInventory(ItemData);
			}
		}
	}

	// 인벤토리 배열에 단일 DT 행 추가
	for (const FSingleDTItem& DTItem : SingleDTItem)
	{
		FItemData* ItemData = DTItem.TableAndRow.DataTable->FindRow<FItemData>(DTItem.TableAndRow.RowName,TEXT(""));
		if (!ItemData) continue;;

		if (!ItemData->IsItemClassValid())continue;

		ItemData->Quantity = FMath::Clamp(DTItem.Quantity, 1, DTItem.Quantity);

		int32 SlotIndex;
		if (GetEmptyInventorySlot(*ItemData, SlotIndex))
		{
			AddItemToInventoryArray(*ItemData, SlotIndex);
			AddWeightToInventory(*ItemData);
		}
	}

	AllItemsFromDT.Empty();
	SingleDTItem.Empty();
	RandomizedItemsData.Empty();

	if (bSortInitialItems)
	{
		SortInventory(DefaultSortingMethod, EInventoryPanels::P1, true);
	}
}

void UInventoryCore::AddItemToInventoryArray(FItemData& ItemData, int32 SlotIndex)
{
	if (!GetOwner()->HasAuthority()) return;

	ModifyItemValue(ItemData);

	const EInventoryPanels InventoryPanel = ItemData.GetInventoryPanelFromItem();
	FInventory& InventoryRef = GetInventoryRefByPanel(InventoryPanel);

	if (bUseInventorySize)
	{
		ItemData.Index = SlotIndex;
		InventoryRef.InventoryArray[SlotIndex] = ItemData;
		ApplyChangesToInventoryArray(InventoryPanel, InventoryRef.InventoryArray);
		ApplyChangesAndBroadcast(ItemData, InventoryPanel);
		return;
	}

	if (SlotIndex >= 0)
	{
		ItemData.Index = SlotIndex;
		InventoryRef.InventoryArray[SlotIndex] = ItemData;
		ApplyChangesToInventoryArray(InventoryPanel, InventoryRef.InventoryArray);
		ApplyChangesAndBroadcast(ItemData, InventoryPanel);
		return;
	}

	if (ItemData.IsItemClassValid())
	{
		const int32 AddIndex = InventoryRef.InventoryArray.Add(ItemData);
		InventoryRef.InventoryArray[AddIndex].Index = AddIndex;
		InventoryRef.InventoryArray[AddIndex].bIsEquipped = false;
		ApplyChangesToInventoryArray(InventoryPanel, InventoryRef.InventoryArray);
		ApplyChangesAndBroadcast(ItemData, InventoryPanel);
	}
}

void UInventoryCore::RemoveItemFromInventoryArray(const FItemData& ItemData)
{
	if (!GetOwner()->HasAuthority()) return;

	const EInventoryPanels ItemPanel = ItemData.GetInventoryPanelFromItem();

	FInventory& InventoryRef = GetInventoryRefByPanel(ItemPanel);

	if (bUseInventorySize)
	{
		FItemData EmptyItemData;
		EmptyItemData.Index = ItemData.Index;
		InventoryRef.InventoryArray[ItemData.Index] = EmptyItemData;
	}
	else
	{
		InventoryRef.InventoryArray.Remove(ItemData);
	}


	if (UKismetSystemLibrary::IsStandalone(this))
	{
		OnRemovedFromInventoryArray.Broadcast(ItemData);
		OnRefreshInventory.Broadcast(ItemPanel);
	}
	else
	{
		UpdateViewersItem(ItemData, true);
		ClientUpdateRemovedItem(ItemData, this);
	}
}

bool UInventoryCore::TransferItemFromInventory(FItemData& ItemData, const FItemData& InSlotData,
                                               EInputMethod Method, UInventoryCore* Sender, FText& OutFailureMessage)
{
	if (!ItemData.IsItemClassValid() || !IsValid(Sender))
	{
		OutFailureMessage = FText();
		return false;
	}

	if (ItemData.IsItemCurrency())
	{
		AddGoldToOwner(ItemData);
		RemoveItemFromInventoryArray(ItemData);
		OutFailureMessage = FText();
		return true;
	}

	if (Sender->bCheckOwnerGold)
	{
		if (!HasEnoughGold(ItemData))
		{
			OutFailureMessage = FText();
			return false;
		}
	}

	const EInventoryPanels ItemPanel = ItemData.GetInventoryPanelFromItem();
	FInventory& InventoryRef = GetInventoryRefByPanel(ItemPanel);

	int32 StackSlotIndex;
	if (InventoryRef.HasPartialStack(ItemData, StackSlotIndex))
	{
		AddToStackInInventory(ItemData, StackSlotIndex);
	}
	else
	{
		if (Method == EInputMethod::RightClick)
		{
			int32 EmptySlotIndex;
			if (GetEmptyInventorySlot(ItemData, EmptySlotIndex))
			{
				AddItemToInventoryArray(ItemData, EmptySlotIndex);
			}
			else
			{
				OutFailureMessage = MessageInventoryFull;
				return false;
			}
		}
		else if (Method == EInputMethod::DragAndDrop)
		{
			AddItemToInventoryArray(ItemData, InSlotData.Index);
		}
	}

	AddWeightToInventory(ItemData);

	if (Sender->bCheckOwnerGold && bCheckOwnerGold)
	{
		RemoveGoldFromOwner(ItemData);
		AddGoldToOwner(ItemData);
	}

	Sender->RemoveItemFromInventoryArray(ItemData);
	Sender->RemoveWeightFromInventory(ItemData);
	OutFailureMessage = FText();
	return true;
}

bool UInventoryCore::TransferItemFromEquipment(const FItemData& ItemData, const FItemData& InSlotData,
                                               EInputMethod Method,
                                               UPlayerEquipmentComponent* Sender, FText& OutFailureMessage)
{
	if (!IsValid(Sender))
	{
		OutFailureMessage = FText();
		return false;
	}


	const EInventoryPanels ItemPanel = ItemData.GetInventoryPanelFromItem();
	FInventory& InventoryRef = GetInventoryRefByPanel(ItemPanel);

	int32 StackSlotIndex;
	if (InventoryRef.HasPartialStack(ItemData, StackSlotIndex))
	{
		AddToStackInInventory(const_cast<FItemData&>(ItemData), StackSlotIndex);
	}
	else
	{
		if (Method == EInputMethod::RightClick)
		{
			int32 EmptySlotIndex;
			if (GetEmptyInventorySlot(ItemData, EmptySlotIndex))
			{
				AddItemToInventoryArray(const_cast<FItemData&>(ItemData), EmptySlotIndex);
			}
			else
			{
				OutFailureMessage = MessageInventoryFull;
				return false;
			}
		}
		if (Method == EInputMethod::RightClick)
		{
			AddItemToInventoryArray(const_cast<FItemData&>(ItemData), InSlotData.Index);
		}
	}

	Sender->RemoveItemFromEquipmentArray(ItemData);
	Sender->DetachItemFromEquipment(ItemData);
	OutFailureMessage = FText();
	return true;
}

void UInventoryCore::SwapItemsInInventory(const FItemData& FirstItem, const FItemData& SecondItem)
{
	if (!GetOwner()->HasAuthority()) return;

	if (!FirstItem.IsItemClassValid()) return;

	FItemData localFirstItem = FirstItem;
	FItemData localSecondItem = SecondItem;

	if (localSecondItem.IsItemClassValid())
	{
		if (localFirstItem == localSecondItem)
		{
			if (UInventoryFunctionLibrary::AreItemsStackable(localFirstItem, localSecondItem))
			{
				RemoveItemFromInventoryArray(localFirstItem);
				AddToStackInInventory(localFirstItem, localSecondItem.Index);
				return;
			}
		}
	}

	const EInventoryPanels ItemPanel = localFirstItem.GetInventoryPanelFromItem();
	FInventory& InventoryRef = GetInventoryRefByPanel(ItemPanel);

	localFirstItem.Index = SecondItem.Index;
	localFirstItem.bIsEquipped = false;
	InventoryRef.InventoryArray[SecondItem.Index] = localFirstItem;

	localSecondItem.Index = FirstItem.Index;
	localSecondItem.bIsEquipped = false;
	InventoryRef.InventoryArray[FirstItem.Index] = localSecondItem;

	OnRefreshInventory.Broadcast(ItemPanel);

	if (!UKismetSystemLibrary::IsStandalone(this))
	{
		if (AController* OwnerController = Cast<AController>(GetOwner()))
		{
			ClientUpdateItems(this, FirstItem.GetInventoryPanelFromItem(), InventoryRef.InventoryArray);
		}
		else
		{
			UpdateViewersInventory(ItemPanel);
		}
	}
}

void UInventoryCore::AddToStackInInventory(FItemData& ItemData, int32 SlotIndex)
{
	ItemData.Index = SlotIndex;

	const EInventoryPanels ItemPanel = ItemData.GetInventoryPanelFromItem();

	FItemData SlotItemData = GetItemBySlot(ItemPanel, SlotIndex);
	SlotItemData.Quantity = SlotItemData.Quantity + ItemData.Quantity;

	AddItemToInventoryArray(SlotItemData, SlotIndex);
}

void UInventoryCore::AddGoldToOwner(const FItemData& ItemData)
{
	if (bCheckOwnerGold)
	{
		float ItemGold = ItemData.CalculateStackedItemValue();
		float CalculateGold = ItemGold + OwnerGold;
		OwnerGold = FMath::Clamp(CalculateGold, 0.0f, CalculateGold);
	}
}

void UInventoryCore::RemoveGoldFromOwner(const FItemData& ItemData)
{
	if (bCheckOwnerGold)
	{
		float ItemGold = ItemData.CalculateStackedItemValue();
		float CalculateGold = ItemGold - OwnerGold;
		OwnerGold = FMath::Clamp(CalculateGold, 0.0f, CalculateGold);
	}
}

void UInventoryCore::ApplyChangesAndBroadcast(const FItemData& ItemData, EInventoryPanels InventoryPanel)
{
	if (UKismetSystemLibrary::IsStandalone(this))
	{
		BroadcastInventoryChanges(InventoryPanel, ItemData);
	}
	else
	{
		UpdateNetworkedPlayers(InventoryPanel, ItemData);
	}
}

void UInventoryCore::BroadcastInventoryChanges(EInventoryPanels InventoryPanel, const FItemData& ItemData)
{
	SwitchActivePanel(InventoryPanel);
	OnRefreshInventory.Broadcast(InventoryPanel);
	OnAddedToInventoryArray.Broadcast(ItemData);
	OnHighlightInventorySlot.Broadcast(ItemData.Index);
}

void UInventoryCore::UpdateNetworkedPlayers(EInventoryPanels InventoryPanel, const FItemData& ItemData)
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		ClientUpdateAddedItem(ItemData, this);
	}
	else
	{
		UpdateViewersItem(ItemData, false);
	}
}

void UInventoryCore::SortSingleInventory(EInventoryPanels Panel, ESortMethod SortMethod)
{
	const FInventory& InventoryRef = GetInventoryRefByPanel(Panel);

	TArray<FItemData> SortedItems;
	switch (SortMethod)
	{
	case ESortMethod::Quicksort:
		SortedItems = UInventoryFunctionLibrary::QuickSortItems(InventoryRef.InventoryArray);
		break;
	case ESortMethod::ByType:
		SortedItems = UInventoryFunctionLibrary::SortItemsByType(InventoryRef.InventoryArray);
		break;

	case ESortMethod::ByRarity:
		SortedItems = UInventoryFunctionLibrary::SortItemsByRarity(InventoryRef.InventoryArray);
		break;

	case ESortMethod::ByValue:
		SortedItems = UInventoryFunctionLibrary::SortItemsByValue(InventoryRef.InventoryArray);
		break;

	case ESortMethod::ByWeight:
		SortedItems = UInventoryFunctionLibrary::SortItemsByWeight(InventoryRef.InventoryArray);
		break;
	}

	ApplyChangesToInventoryArray(Panel, SortedItems);
	BuildInventory(Panel);
	OnRefreshInventory.Broadcast(Panel);

	if (!UKismetSystemLibrary::IsStandalone(this))
	{
		UpdateViewersInventory(Panel);
		ClientUpdateItems(this, Panel, InventoryRef.InventoryArray);
	}
}

void UInventoryCore::AddWeightToInventory(const FItemData& ItemData)
{
	const float ItemWeight = ItemData.CalculateStackedItemWeight();
	const float CalculateWeight = CurrentInventoryWeight + ItemWeight;
	CurrentInventoryWeight = FMath::Clamp(CalculateWeight, 0.0f, CalculateWeight);
}

void UInventoryCore::RemoveWeightFromInventory(const FItemData& ItemData)
{
	if (bUseInventoryWeight)
	{
		const float ItemWeight = ItemData.CalculateStackedItemWeight();
		const double CalculateWeight = CurrentInventoryWeight - ItemWeight;
		CurrentInventoryWeight = FMath::Clamp(CalculateWeight, 0.0f, CalculateWeight);
	}
}

void UInventoryCore::SortInventory(ESortMethod SortMethod, EInventoryPanels SinglePanel, bool bEveryPanel)
{
	if (bEveryPanel)
	{
		for (const TTuple<EInventoryPanels, FInventory>& Inventory : Inventories)
		{
			SortSingleInventory(Inventory.Key, SortMethod);
		}
	}
	else
	{
		SortSingleInventory(SinglePanel, SortMethod);
	}
}

void UInventoryCore::ChangeInventorySize(EInventoryPanels Panel, int32 NewSize)
{
	if (Inventories.Contains(Panel))
	{
		Inventories[Panel].InventorySize = NewSize;
		BuildInventory(Panel);
		OnRefreshInventory.Broadcast(Panel);
	}
	else
	{
		Inventories[EInventoryPanels::P1].InventorySize = NewSize;
		BuildInventory(EInventoryPanels::P1);
		OnRefreshInventory.Broadcast(EInventoryPanels::P1);
	}
}

void UInventoryCore::ModifyItemValue(FItemData& ItemData)
{
	ItemData.ValueModifier = ValueModifier;
}

void UInventoryCore::RemoveItemQuantity(const FItemData& ItemData, int32 Quantity)
{
	FItemData localItemData = ItemData;
	if (localItemData.Quantity <= Quantity)
	{
		RemoveItemFromInventoryArray(localItemData);
		RemoveWeightFromInventory(localItemData);
	}
	else if (localItemData.Quantity > Quantity)
	{
		RemoveWeightFromInventory(localItemData);
		localItemData.Quantity = localItemData.Quantity - Quantity;
		AddItemToInventoryArray(localItemData, localItemData.Index);
		AddWeightToInventory(localItemData);
	}
}

FItemData UInventoryCore::GetItemBySlot(EInventoryPanels Panel, int32 SlotIndex)
{
	FInventory& InventoryRef = GetInventoryRefByPanel(Panel);
	return InventoryRef.InventoryArray[SlotIndex];
}

EItemRemoveType UInventoryCore::GetItemRemoveType(const FItemData& ItemData)
{
	if (bCheckRemoveType)
	{
		if (ItemData.IsItemClassValid())
		{
			return ItemData.ItemClass.GetDefaultObject()->RemoveType;
		}
	}

	return EItemRemoveType::Default;
}

void UInventoryCore::SwitchActivePanel(EInventoryPanels Panel)
{
	if (Panel != ActivePanel && Inventories.Contains(Panel))
	{
		ActivePanel = Panel;
		OnSwitchedActivePanel.Broadcast(ActivePanel);
	}
}

void UInventoryCore::SplitItemsInInventory(UInventoryCore* Sender, const FItemData& ItemData,
                                           const FItemData& InSlotData,
                                           const FItemData& StackableLeft, EInputMethod InputMethod,
                                           EItemDestination Initiator, EItemDestination Destination)
{
	FText FailureMessage;
	const bool bSuccess = TransferItemFromInventory(const_cast<FItemData&>(ItemData), InSlotData, InputMethod, Sender,
	                                                FailureMessage);
	Sender->ClientTransferItemReturnValue(bSuccess, FailureMessage.ToString());

	if (bSuccess)
	{
		if (UInventoryFunctionLibrary::IsStackableAndHaveStacks(StackableLeft, 0))
		{
			Sender->AddItemToInventoryArray(const_cast<FItemData&>(StackableLeft), StackableLeft.Index);
		}
	}
}

void UInventoryCore::ConfirmationPopupAccepted(UInventoryCore* Sender, const FItemData& ItemData,
                                               const FItemData& InSlotData,
                                               EInputMethod InputMethod, EItemDestination Initiator,
                                               EItemDestination Destination)
{
	if (!UInventoryFunctionLibrary::IsStackableAndHaveStacks(ItemData, 1))
	{
		FText FailureMessage;
		const bool bSuccess = TransferItemFromInventory(const_cast<FItemData&>(ItemData), InSlotData, InputMethod,
		                                                Sender, FailureMessage);
		Sender->ClientTransferItemReturnValue(bSuccess, FailureMessage.ToString());
	}
}

void UInventoryCore::ServerSortInventory_Implementation(UInventoryCore* Inventory, ESortMethod SortMethod,
                                                        EInventoryPanels SinglePanel, bool bEveryPanel)
{
	Inventory->SortInventory(SortMethod, SinglePanel, bEveryPanel);
}

void UInventoryCore::ServerRemoveItemFromInventory_Implementation(UInventoryCore* Inventory, const FItemData& ItemData)
{
	Inventory->RemoveItemFromInventoryArray(ItemData);
}

void UInventoryCore::ServerAddItemToInventory_Implementation(UInventoryCore* Inventory, const FItemData& ItemData,
                                                             int32 SlotIndex)
{
	Inventory->AddItemToInventoryArray(const_cast<FItemData&>(ItemData), SlotIndex);
}

void UInventoryCore::ServerTransferItemFromEquipment_Implementation(const FItemData& ItemData,
                                                                    FItemData InSlotData,
                                                                    EInputMethod InputMethod,
                                                                    UPlayerEquipmentComponent* Sender)
{
	FText FailureMessage;
	bool bSuccess = TransferItemFromEquipment(const_cast<FItemData&>(ItemData), InSlotData, InputMethod, Sender,
	                                          FailureMessage);
	ClientTransferItemReturnValue(bSuccess, FailureMessage.ToString());
}

void UInventoryCore::ServerMoveItemToSlot_Implementation(UInventoryCore* Inventory, EInventoryPanels Panel,
                                                         int32 MoveFrom, int32 MoveTo)
{
	if (!IsValid(Inventory))return;
	SwapItemsInInventory(GetItemBySlot(Panel, MoveFrom), GetItemBySlot(Panel, MoveTo));
}

void UInventoryCore::ServerTransferItemFromInventory_Implementation(UInventoryCore* Receiver, const FItemData& ItemData,
                                                                    FItemData InSlotData,
                                                                    EInputMethod InputMethod, UInventoryCore* Sender)
{
	FText FailureMessage;
	bool bSuccess = Receiver->TransferItemFromInventory(const_cast<FItemData&>(ItemData), InSlotData, InputMethod,
	                                                    Sender, FailureMessage);
	ClientTransferItemReturnValue(bSuccess, FailureMessage.ToString());
}

void UInventoryCore::ClientUpdateAddedItem_Implementation(const FItemData& ItemData, UInventoryCore* Inventory)
{
	UpdateAddedItemInInventory(ItemData, Inventory);
}

void UInventoryCore::ClientUpdateRemovedItem_Implementation(const FItemData& ItemData, UInventoryCore* Inventory)
{
	UpdateRemovedItemInInventory(ItemData, Inventory);
}

void UInventoryCore::ClientUpdateItems_Implementation(UInventoryCore* InventoryCore, EInventoryPanels InventoryPanel,
                                                      const TArray<FItemData>& Array)
{
	if (IsValid(InventoryCore))
	{
		InventoryCore->ApplyChangesToInventoryArray(InventoryPanel, Array);
		InventoryCore->OnRefreshInventory.Broadcast(InventoryPanel);
	}
}

void UInventoryCore::ClientTransferItemReturnValue_Implementation(bool bSuccess, const FString& FailureMessage)
{
	if (!bSuccess)
	{
		UWidgetSystemLibrary::GetInventoryWidgetController(this)->DisplayMessageNotify(FailureMessage);
	}
}

void UInventoryCore::ServerSplitItemsInInventory_Implementation(UInventoryCore* Receiver, UInventoryCore* Sender,
                                                                const FItemData& ItemData, FItemData InSlotData,
                                                                const FItemData& StackableLeft,
                                                                EInputMethod InputMethod,
                                                                EItemDestination Initiator,
                                                                EItemDestination Destination)
{
	Receiver->SplitItemsInInventory(Sender, const_cast<FItemData&>(ItemData), InSlotData,
	                                const_cast<FItemData&>(StackableLeft), InputMethod, Initiator, Destination);
}

void UInventoryCore::ServerConfirmationPopUpAccepted_Implementation(UInventoryCore* Receiver, UInventoryCore* Sender,
                                                                    const FItemData& ItemData, FItemData InSlotData,
                                                                    EInputMethod InputMethod,
                                                                    EItemDestination Initiator,
                                                                    EItemDestination Destination)
{
	Receiver->ConfirmationPopupAccepted(Sender, const_cast<FItemData&>(ItemData), InSlotData, InputMethod, Initiator,
	                                    Destination);
}

void UInventoryCore::UpdateViewersItem(const FItemData& ItemData, bool bRemove)
{
	for (const APlayerState* CurrentViewer : CurrentViewers)
	{
		const UPlayerInventoryComponent* OwnerPlayerInventory = Cast<UPlayerInventoryComponent>(
			CurrentViewer->GetOwner());
		if (IsValid(OwnerPlayerInventory))
		{
			if (bRemove)
			{
				ClientUpdateRemovedItem(ItemData, this);
			}
			else
			{
				ClientUpdateAddedItem(ItemData, this);
			}
		}
	}
}

void UInventoryCore::UpdateViewersInventory(EInventoryPanels Panel)
{
	for (const APlayerState* CurrentViewer : CurrentViewers)
	{
		UPlayerInventoryComponent* OwnerPlayerInventory = Cast<UPlayerInventoryComponent>(CurrentViewer->GetOwner());
		if (IsValid(OwnerPlayerInventory))
		{
			FInventory& InventoryRef = GetInventoryRefByPanel(Panel);
			OwnerPlayerInventory->ClientUpdateItems(this, Panel, InventoryRef.InventoryArray);
		}
	}
}

void UInventoryCore::UpdateAddedItemInInventory(const FItemData& ItemData, UInventoryCore* Inventory)
{
	if (!IsValid(Inventory)) return;

	const EInventoryPanels Panel = ItemData.GetInventoryPanelFromItem();
	FInventory& InventoryRef = GetInventoryRefByPanel(Panel);

	if (Inventory->bUseInventorySize)
	{
		InventoryRef.InventoryArray[ItemData.Index] = ItemData;
	}
	else
	{
		InventoryRef.InventoryArray.Add(ItemData);
	}
	Inventory->SwitchActivePanel(Panel);
	Inventory->OnRefreshInventory.Broadcast(Panel);
	Inventory->OnAddedToInventoryArray.Broadcast(ItemData);
	Inventory->OnHighlightInventorySlot.Broadcast(ItemData.Index);
}

void UInventoryCore::UpdateRemovedItemInInventory(const FItemData& ItemData, UInventoryCore* Inventory)
{
	if (!IsValid(Inventory)) return;

	const EInventoryPanels Panel = ItemData.GetInventoryPanelFromItem();
	FInventory& InventoryRef = GetInventoryRefByPanel(Panel);

	if (Inventory->bUseInventorySize)
	{
		FItemData EmptyItem = FItemData();
		EmptyItem.Index = ItemData.Index;
		InventoryRef.InventoryArray[ItemData.Index] = EmptyItem;
	}
	else
	{
		InventoryRef.InventoryArray.Remove(ItemData);
	}
	Inventory->OnRemovedFromInventoryArray.Broadcast(ItemData);
	Inventory->OnRefreshInventory.Broadcast(Panel);
}

void UInventoryCore::AddViewer_Implementation(APlayerState* PlayerState, UInventoryCore* InventoryCore)
{
	if (IsValid(PlayerState))
	{
		InventoryCore->CurrentViewers.Add(PlayerState);
	}
}

void UInventoryCore::RemoveViewer_Implementation(APlayerState* PlayerState, UInventoryCore* InventoryCore)
{
	if (IsValid(PlayerState))
	{
		InventoryCore->CurrentViewers.Remove(PlayerState);
	}
}

FInventory& UInventoryCore::GetInventoryRefByPanel(EInventoryPanels Panel)
{
	if (Inventories.Contains(Panel))
	{
		return Inventories[Panel];
	}

	return Inventories[EInventoryPanels::P1];
}

void UInventoryCore::GetInventoryAndSize(EInventoryPanels Panel, TArray<FItemData>& OutInventory, int32& OutSize)
{
	if (Inventories.Contains(Panel))
	{
		OutInventory = Inventories[Panel].InventoryArray;
		OutSize = Inventories[Panel].InventorySize;
	}
	else
	{
		OutInventory = Inventories[EInventoryPanels::P1].InventoryArray;
		OutSize = Inventories[EInventoryPanels::P1].InventorySize;
	}
}

bool UInventoryCore::GetEmptyInventorySlot(const FItemData& ItemData, int32& OutEmptySlotIndex)
{
	if (bUseInventorySize)
	{
		const EInventoryPanels ItemPanel = ItemData.GetInventoryPanelFromItem();
		FInventory& InventoryRef = GetInventoryRefByPanel(ItemPanel);
		return InventoryRef.FindEmptySlotInArray(OutEmptySlotIndex);
	}

	OutEmptySlotIndex = -1;
	return true;
}

FItemData UInventoryCore::RandomizeItemParameters(const FItemData& ItemData)
{
	FItemData RandomItemData = ItemData;
	const UMainItem* RandomItemCDO = RandomItemData.ItemClass.GetDefaultObject();

	//전리품 클래스에 기본값 추가
	TSet<TSubclassOf<UMainItem>> LootClasses = RandomItemCDO->LootClasses;
	LootClasses.Add(RandomItemData.ItemClass);

	//랜덤 루트 아이템 클래스 지정
	TArray<TSubclassOf<UMainItem>> LootClassesArray = LootClasses.Array();
	const int32 RandomLootItemIndex = FMath::RandRange(0, LootClasses.Num() - 1);
	RandomItemData.ItemClass = LootClassesArray[RandomLootItemIndex];

	//무작위 수량 설정
	const int32 Quantity = FMath::RandRange(RandomItemCDO->MinRandQuantity, RandomItemCDO->MaxRandQuantity);
	RandomItemData.Quantity = FMath::Clamp(Quantity, 1, Quantity);

	//임의의 내구성 설정
	const int32 Durability = FMath::RandRange(RandomItemCDO->MinRandDurability, RandomItemCDO->MaxRandDurability);
	RandomItemData.Durability = FMath::Clamp(Durability, 1, Durability);

	return RandomItemData;
}

bool UInventoryCore::HasEnoughGold(const FItemData& ItemData)
{
	if (bCheckOwnerGold)
	{
		return OwnerGold >= ItemData.CalculateStackedItemValue();
	}
	return true;
}

bool UInventoryCore::IsInventoryOverweight()
{
	if (bUseInventorySize)
	{
		return CurrentInventoryWeight > MaxInventoryWeight;
	}
	return false;
}

bool UInventoryCore::GetItemByID(FString ItemID, EInventoryPanels InSpecifiedPanel, FItemData& OutItem)
{
	if (InSpecifiedPanel != EInventoryPanels::None)
	{
		FInventory& InventoryRef = GetInventoryRefByPanel(InSpecifiedPanel);
		return InventoryRef.FindItemByID(ItemID, OutItem);
	}

	for (const TTuple<EInventoryPanels, FInventory>& Inventory : Inventories)
	{
		FInventory& InventoryRef = GetInventoryRefByPanel(Inventory.Key);
		if (InventoryRef.FindItemByID(ItemID, OutItem))
		{
			return OutItem.IsItemClassValid();
		}
	}

	return false;
}

bool UInventoryCore::GetItemByData(const FItemData& ItemData, FItemData& OutItemData)
{
	FInventory& InventoryRef = GetInventoryRefByPanel(ItemData.GetInventoryPanelFromItem());

	int32 FindSlotIndex = 0;
	if (InventoryRef.FindItemIndex(ItemData, FindSlotIndex))
	{
		OutItemData = InventoryRef.InventoryArray[FindSlotIndex];
		return true;
	}

	OutItemData = FItemData();
	return false;
}

TArray<FItemData> UInventoryCore::GetCombinedInventories()
{
	TArray<FItemData> CombinedInventories;

	TArray<FInventory> AllInventories;
	Inventories.GenerateValueArray(AllInventories);
	for (const FInventory& Inventory : AllInventories)
	{
		CombinedInventories.Append(Inventory.InventoryArray);
	}

	return CombinedInventories;
}

TArray<FItemData> UInventoryCore::GetItemsOfSpecifiedType(EItemType ItemType)
{
	TArray<FItemData> ItemDatas;
	for (TTuple<EInventoryPanels, FInventory> Inventory : Inventories)
	{
		ItemDatas.Append(Inventory.Value.GetAllItemsOfType(ItemType));
	}
	return ItemDatas;
	// return UInventoryFunctionLibrary::GetAllItemsOfType(GetCombinedInventories(), ItemType);
}

int32 UInventoryCore::GetAmountOfEmptySlots(EInventoryPanels Panel)
{
	FInventory& InventoryRef = GetInventoryRefByPanel(Panel);
	return InventoryRef.FindAmountOfEmptySlots();
}

void UInventoryCore::OnRep_MaxInventoryWeight()
{
	OnChangedMaxWeight.Broadcast();
}

void UInventoryCore::OnRep_OwnerGold()
{
	OnChangedOwnerGold.Broadcast();
}

void UInventoryCore::OnRep_CurrentInventoryWeight()
{
	OnChangedCurrentWeight.Broadcast();
}
