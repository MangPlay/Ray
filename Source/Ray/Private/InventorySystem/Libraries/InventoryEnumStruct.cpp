#include "InventorySystem/Libraries/InventoryEnumStruct.h"

#include "Kismet/KismetStringLibrary.h"
#include "InventorySystem/Objects/MainItem.h"

FItemData::FItemData()
{
}

FItemData::FItemData(EItemSlot InEquipmentSlot)
{
	EquipmentSlot = InEquipmentSlot;
}

bool FItemData::operator==(const FItemData& other) const
{
	return UKismetStringLibrary::EqualEqual_StrStr(ID, other.ID);
	// return ID == other.ID;
	// && EquipmentSlot == other.EquipmentSlot
	// && ItemClass == other.ItemClass
	// && Index == other.Index
	// && bIsEquipped == other.bIsEquipped
	// && bAlreadyUsed == other.bAlreadyUsed
	// && ValueModifier == other.ValueModifier;
}

bool FItemData::IsItemClassValid() const
{
	return IsValid(ItemClass);
}

bool FItemData::IsItemCurrency() const
{
	if (IsValid(ItemClass))
	{
		// return ItemClass.GetDefaultObject()->Type == EItemType::Currency;
	}
	return false;
}

bool FItemData::IsItemConsumable() const
{
	return EquipmentSlot == EItemSlot::Pocket1 || EquipmentSlot == EItemSlot::Pocket2 ||
		EquipmentSlot == EItemSlot::Pocket3 || EquipmentSlot == EItemSlot::Pocket4;
}

float FItemData::CalculateStackedItemValue() const
{
	if (IsValid(ItemClass))
	{
		// const float ItemValue = ItemClass.GetDefaultObject()->Value * ValueModifier;
		// return FMath::Clamp(Quantity, 1, Quantity) * ItemValue;
	}
	return 0;
}

float FItemData::CalculateStackedItemWeight() const
{
	if (IsValid(ItemClass))
	{
		// return FMath::Clamp(Quantity, 1, Quantity) * ItemClass.GetDefaultObject()->Weight;
	}
	return 0;
}

EInventoryPanels FItemData::GetInventoryPanelFromItem() const
{
	if (IsValid(ItemClass))
	{
		return ItemClass.GetDefaultObject()->InventoryPanel;
	}

	return EInventoryPanels::None;
}

TArray<FItemData> FInventory::GetAllItemsOfType(EItemType ItemType)
{
	TArray<FItemData> Items;
	for (const FItemData& ItemData : InventoryArray)
	{
		if (ItemData.IsItemClassValid())
		{
			if (ItemData.ItemClass.GetDefaultObject()->Type == ItemType)
			{
				Items.Add(ItemData);
			}
		}
	}
	return Items;
}

FInventory::FInventory()
{
}

void FInventory::BuildInventory()
{
	InventoryArray.SetNum(InventorySize);
	for (int i = 0; i < InventorySize - 1; ++i)
	{
		InventoryArray[i].Index = i;
		InventoryArray[i].bIsEquipped = false;
	}
}

bool FInventory::HasPartialStack(const FItemData& ItemData, int32& OutSlotIndex)
{
	for (int i = 0; i < InventoryArray.Num(); ++i)
	{
		if (InventoryArray[i].IsItemClassValid())
		{
			if (UKismetStringLibrary::EqualEqual_StrStr(InventoryArray[i].ID, ItemData.ID) &&
				InventoryArray[i].ItemClass.GetDefaultObject()->bStackable &&
				ItemData.ItemClass.GetDefaultObject()->bStackable)
			{
				OutSlotIndex = i;
				return true;
			}
		}
	}
	OutSlotIndex = 0;
	return false;
}

bool FInventory::FindEmptySlotInArray(int32& OutEmptySlotIndex)
{
	for (int i = 0; i < InventoryArray.Num(); ++i)
	{
		if (!InventoryArray[i].IsItemClassValid())
		{
			OutEmptySlotIndex = i;
			return true;
		}
	}

	OutEmptySlotIndex = 0;
	return false;
}

bool FInventory::FindItemByID(const FString& ItemID, FItemData& OutItemData)
{
	for (int i = 0; i < InventoryArray.Num(); ++i)
	{
		if (UKismetStringLibrary::EqualEqual_StrStr(InventoryArray[i].ID, ItemID))
		{
			OutItemData = InventoryArray[i];
			return true;
		}
	}
	OutItemData = FItemData();
	return false;
}

bool FInventory::FindItemIndex(const FItemData& ItemData, int32& OutSlotIndex)
{
	for (int i = 0; i < InventoryArray.Num(); ++i)
	{
		if (InventoryArray[i] == ItemData)
		{
			OutSlotIndex = i;
			return false;
		}
	}

	OutSlotIndex = 0;
	return false;
}

bool FInventory::IsValidSlotIndex(int32 Index)
{
	return InventoryArray.IsValidIndex(Index);
}

int32 FInventory::FindAmountOfEmptySlots()
{
	int32 EmptySlotCount = 0;
	for (const FItemData& ItemData : InventoryArray)
	{
		if (!ItemData.IsItemClassValid())
		{
			EmptySlotCount = EmptySlotCount + 1;
		}
	}
	return EmptySlotCount;
}
