// RAI Project 개발자 : Nam Chan woo

#include "InventorySystem/Libraries/InventoryFunctionLibrary.h"

#include "InventorySystem/Components/ConsumableBuffsComponent.h"
#include "InventorySystem/Components/CraftingComponent.h"
#include "InventorySystem/Components/InteractableComponent.h"
#include "InventorySystem/Components/InteractionComponent.h"
#include "InventorySystem/Components/LevelingComponent.h"
#include "InventorySystem/Components/PlayerEquipmentComponent.h"
#include "InventorySystem/Components/PlayerInventoryComponent.h"
#include "InventorySystem/Components/StatsComponent.h"
#include "InventorySystem/Objects/MainItem.h"
#include "Kismet/KismetStringLibrary.h"
#include "Libraries/RayColors.h"
#include "Player/RayPlayerController.h"
#include "UI/WidgetSystemLibrary.h"
#include "UI/WidgetController/InventoryWidgetController.h"


EInventoryPanels UInventoryFunctionLibrary::GetInventoryPanelFromItem(const FItemData& ItemData)
{
	if (UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject())
	{
		return MainItemCDO->InventoryPanel;
	}
	return EInventoryPanels::None;
}

bool UInventoryFunctionLibrary::CanItemBeRemoved(const FItemData& ItemData)
{
	return ItemData.ItemClass.GetDefaultObject()->RemoveType != EItemRemoveType::CannotBeRemoved;
}

bool UInventoryFunctionLibrary::AreItemsStackable(const FItemData& FirstItem, const FItemData& SecondItem)
{
	return FirstItem.ItemClass.GetDefaultObject()->bStackable && SecondItem.ItemClass.GetDefaultObject()->bStackable;
}

bool UInventoryFunctionLibrary::AreWeaponTypesEqual(const FItemData& FirstItem, const FItemData& SecondItem)
{
	if (!FirstItem.IsItemClassValid()) return false;
	if (!SecondItem.IsItemClassValid()) return false;

	return FirstItem.ItemClass.GetDefaultObject()->WeaponType == SecondItem.ItemClass.GetDefaultObject()->WeaponType;
}

float UInventoryFunctionLibrary::CalculateStackedItemWeight(const FItemData& ItemData)
{
	if (const UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject())
	{
		return ItemData.Quantity * MainItemCDO->Weight;
	}
	return 0.0f;
}

float UInventoryFunctionLibrary::CalculateStackedItemValue(const FItemData& ItemData)
{
	if (const UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject())
	{
		int32 ItemQuantity = FMath::Clamp(ItemData.Quantity, 1, ItemData.Quantity);
		return (MainItemCDO->Value * ItemData.ValueModifier) * ItemQuantity;
	}
	return 0.0f;
}

bool UInventoryFunctionLibrary::IsStackableAndHaveStacks(const FItemData& ItemData, int32 Quantity)
{
	return ItemData.ItemClass.GetDefaultObject()->bStackable && ItemData.Quantity > Quantity;
}

bool UInventoryFunctionLibrary::CanWeaponsBeSwapped(const FItemData& FirstItem, const FItemData& SecondItem)
{
	if (FirstItem.IsItemClassValid())
	{
		const bool bIsSameOneHand =
			FirstItem.ItemClass.GetDefaultObject()->WeaponType == EWeaponType::OneHand &&
			SecondItem.ItemClass.GetDefaultObject()->WeaponType == EWeaponType::OneHand;

		if (bIsSameOneHand)
		{
			return true;
		}
	}

	return FirstItem.ItemClass.GetDefaultObject()->WeaponType == EWeaponType::OneHand &&
		(SecondItem.EquipmentSlot == EItemSlot::Weapon || SecondItem.EquipmentSlot == EItemSlot::Shield);
}

// void UInventoryFunctionLibrary::FindEmptySlotInArray(const TArray<FItemData>& Inventory, bool& bFound, int32& SlotIndex)
// {
// 	for (int i = 0; i < Inventory.Num() - 1; ++i)
// 	{
// 		if (!Inventory[i].IsItemClassValid())
// 		{
// 			SlotIndex = i;
// 			bFound = true;
// 			return;
// 		}
// 	}
//
// 	bFound = false;
// 	SlotIndex = 0;
// }

bool UInventoryFunctionLibrary::HasPartialStack(const TArray<FItemData>& ItemArray, const FItemData& ItemData,
                                                int32& OutSlotIndex)
{
	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		if (ItemArray[i].IsItemClassValid())
		{
			if (UKismetStringLibrary::EqualEqual_StrStr(ItemArray[i].ID, ItemData.ID) &&
				ItemArray[i].ItemClass.GetDefaultObject()->bStackable &&
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

bool UInventoryFunctionLibrary::FindItemByID(const TArray<FItemData> ItemArray, FString ItemID, FItemData& OutItem)
{
	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		if (UKismetStringLibrary::EqualEqual_StrStr(ItemArray[i].ID, ItemID))
		{
			OutItem = ItemArray[i];
			return true;
		}
	}
	OutItem = FItemData();
	return false;
}

// bool UInventoryFunctionLibrary::FindItemIndex(const TArray<FItemData>& Inventory, const FItemData& ItemData,
//                                               int32& OutSlotIndex)
// {
// 	for (int i = 0; i < Inventory.Num(); ++i)
// 	{
// 		if (Inventory[i] == ItemData)
// 		{
// 			OutSlotIndex = i;
// 			return false;
// 		}
// 	}
//
// 	OutSlotIndex = 0;
// 	return false;
// }

// TArray<FItemData> UInventoryFunctionLibrary::GetAllItemsOfType(const TArray<FItemData>& Inventory, EItemType ItemType)
// {
// 	TArray<FItemData> Items;
// 	for (const FItemData& ItemData : Inventory)
// 	{
// 		if (ItemData.IsItemClassValid())
// 		{
// 			if (ItemData.ItemClass.GetDefaultObject()->Type == ItemType)
// 			{
// 				Items.Add(ItemData);
// 			}
// 		}
// 	}
// 	return Items;
// }

// int32 UInventoryFunctionLibrary::FindAmountOfEmptySlots(const TArray<FItemData>& Inventory)
// {
// 	int32 EmptySlotCount = 0;
// 	for (const FItemData& ItemData : Inventory)
// 	{
// 		if (!ItemData.IsItemClassValid())
// 		{
// 			EmptySlotCount = EmptySlotCount + 1;
// 		}
// 	}
// 	return EmptySlotCount;
// }

TArray<FItemData> UInventoryFunctionLibrary::QuickSortItems(const TArray<FItemData>& InventoryArray)
{
	TArray<FItemData> SortInventory;

	for (const FItemData& ItemData : InventoryArray)
	{
		if (ItemData.IsItemClassValid())
		{
			SortInventory.Add(ItemData);
		}
	}

	return SortInventory;
}

TArray<FItemData> UInventoryFunctionLibrary::SortItemsByRarity(const TArray<FItemData>& InventoryArray)
{
	TArray<FItemData> SortInventory;
	TArray<FItemData> Common;
	TArray<FItemData> Superior;
	TArray<FItemData> Epic;
	TArray<FItemData> Legendary;
	TArray<FItemData> Consumable;
	TArray<FItemData> Usable;

	for (const FItemData& ItemData : InventoryArray)
	{
		if (ItemData.IsItemClassValid())
		{
			const UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject();
			switch (MainItemCDO->Rarity)
			{
			case EItemRarity::Common:
				Common.Add(ItemData);
				break;
			case EItemRarity::Superior:
				Superior.Add(ItemData);
				break;
			case EItemRarity::Epic:
				Epic.Add(ItemData);
				break;
			case EItemRarity::Legendary:
				Legendary.Add(ItemData);
				break;
			case EItemRarity::Consumable:
				Consumable.Add(ItemData);
				break;
			case EItemRarity::Usable:
				Usable.Add(ItemData);
				break;

			default: break;;
			}
		}
	}

	SortInventory.Append(Legendary);
	SortInventory.Append(Epic);
	SortInventory.Append(Superior);
	SortInventory.Append(Common);
	SortInventory.Append(Consumable);
	SortInventory.Append(Usable);

	return SortInventory;
}

TArray<FItemData> UInventoryFunctionLibrary::SortItemsByValue(const TArray<FItemData>& InventoryArray)
{
	TArray<FItemData> SortInventory = InventoryArray;

	Algo::Sort(SortInventory, [](const FItemData& A, const FItemData& B)
	{
		UMainItem* AItemCDO = A.ItemClass.GetDefaultObject();
		UMainItem* BItemCDO = B.ItemClass.GetDefaultObject();
		return AItemCDO && BItemCDO && AItemCDO->Value > BItemCDO->Value;
	});

	return SortInventory;
}

TArray<FItemData> UInventoryFunctionLibrary::SortItemsByWeight(const TArray<FItemData>& InventoryArray)
{
	TArray<FItemData> SortInventory = InventoryArray;

	Algo::Sort(SortInventory, [](const FItemData& A, const FItemData& B)
	{
		UMainItem* AItemCDO = A.ItemClass.GetDefaultObject();
		UMainItem* BItemCDO = B.ItemClass.GetDefaultObject();
		return AItemCDO && BItemCDO && AItemCDO->Weight > BItemCDO->Weight;
	});

	return SortInventory;
}

TArray<FItemData> UInventoryFunctionLibrary::SortItemsByType(const TArray<FItemData>& InventoryArray)
{
	TArray<FItemData> SortInventory;
	TArray<FItemData> Armor;
	TArray<FItemData> Weapon;
	TArray<FItemData> Shield;
	TArray<FItemData> Food;
	TArray<FItemData> Potion;
	TArray<FItemData> CraftingIngredient;
	TArray<FItemData> QuestItem;
	TArray<FItemData> Other;
	TArray<FItemData> Currency;


	for (const FItemData& ItemData : InventoryArray)
	{
		if (ItemData.IsItemClassValid())
		{
			const UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject();
			switch (MainItemCDO->Type)
			{
			case EItemType::Armor:
				Armor.Add(ItemData);
				break;
			case EItemType::Weapon:
				Weapon.Add(ItemData);
				break;
			case EItemType::Shield:
				Shield.Add(ItemData);
				break;
			case EItemType::Food:
				Food.Add(ItemData);
				break;
			case EItemType::Potion:
				Potion.Add(ItemData);
				break;
			case EItemType::CraftingIngredient:
				CraftingIngredient.Add(ItemData);
				break;
			case EItemType::QuestItem:
				QuestItem.Add(ItemData);
				break;
			case EItemType::Other:
				Other.Add(ItemData);
				break;
			case EItemType::Currency:
				Currency.Add(ItemData);
				break;

			default: break;;
			}
		}
	}

	//InventoryP1
	SortInventory.Append(Armor);
	SortInventory.Append(Weapon);
	SortInventory.Append(Shield);

	//InventoryP2
	SortInventory.Append(Potion);
	SortInventory.Append(Food);

	//InventoryP3
	SortInventory.Append(CraftingIngredient);

	//InventoryP4
	SortInventory.Append(QuestItem);
	SortInventory.Append(Other);

	//InventoryP4
	SortInventory.Append(Currency);

	return SortInventory;
}

void UInventoryFunctionLibrary::StartPlayerCapture2D(AActor* Controller)
{
	UWidgetSystemLibrary::GetInventoryWidgetController(Controller)->StartPlayerCapture();
}

void UInventoryFunctionLibrary::StopPlayerCapture2D(AActor* Controller)
{
	UWidgetSystemLibrary::GetInventoryWidgetController(Controller)->StopPlayerCapture();
}

// void UInventoryFunctionLibrary::PlaySoundOnOpeningWidget(const UObject* WorldContextObject)
// {
// 	if (UInventoryWidgetController* InventoryWidgetController =
// 		UWidgetSystemLibrary::GetInventoryWidgetController(WorldContextObject))
// 	{
// 		InventoryWidgetController->PlaySoundOnOpeningWidget();
// 	}
// }

UPlayerInventoryComponent* UInventoryFunctionLibrary::GetPlayerInventory(AActor* Target)
{
	return Cast<UPlayerInventoryComponent>(Target->GetComponentByClass(UPlayerInventoryComponent::StaticClass()));
}

UPlayerEquipmentComponent* UInventoryFunctionLibrary::GetPlayerEquipment(AActor* Target)
{
	return Cast<UPlayerEquipmentComponent>(Target->GetComponentByClass(UPlayerEquipmentComponent::StaticClass()));
}

UStatsComponent* UInventoryFunctionLibrary::GetStatsComponent(AActor* Target)
{
	return Cast<UStatsComponent>(Target->GetComponentByClass(UStatsComponent::StaticClass()));
}

UInteractionComponent* UInventoryFunctionLibrary::GetInteractionComponent(AActor* Target)
{
	return Cast<UInteractionComponent>(Target->GetComponentByClass(UInteractionComponent::StaticClass()));
}

UConsumableBuffsComponent* UInventoryFunctionLibrary::GetConsumableBuffsComponent(AActor* Target)
{
	return Cast<UConsumableBuffsComponent>(Target->GetComponentByClass(UConsumableBuffsComponent::StaticClass()));
}

ULevelingComponent* UInventoryFunctionLibrary::GetLevelingComponent(AActor* Target)
{
	return Cast<ULevelingComponent>(Target->GetComponentByClass(ULevelingComponent::StaticClass()));
}

UCraftingComponent* UInventoryFunctionLibrary::GetCraftingComponent(AActor* Target)
{
	return Cast<UCraftingComponent>(Target->GetComponentByClass(UCraftingComponent::StaticClass()));
}

bool UInventoryFunctionLibrary::FindEmptyEquipmentSlot(const TMap<EItemSlot, FItemData>& EquipmentData,
                                                       const TArray<EItemSlot>& SlotsToSearch, EItemSlot DefaultSlot,
                                                       EItemSlot& OutSlot)
{
	for (EItemSlot SearchItemSlot : SlotsToSearch)
	{
		if (EquipmentData.Contains(SearchItemSlot))
		{
			if (!EquipmentData.Find(SearchItemSlot)->IsItemClassValid())
			{
				OutSlot = SearchItemSlot;
				return true;
			}
		}
	}
	OutSlot = DefaultSlot;
	return false;
}

//
// UInteractableComponent* UInventoryFunctionLibrary::GetCurrentInteractableObject(AActor* Target)
// {
// 	UInteractionComponent* TargetInteraction = Cast<UInteractionComponent>(Target->GetComponentByClass(UInteractionComponent::StaticClass()));
//
// 	if (IsValid(TargetInteraction))
// 	{
// 		if (IsValid(TargetInteraction->CurrentInteractable))
// 		{
// 			return TargetInteraction->CurrentInteractable;
// 		}
// 	}
//
// 	return nullptr;
// }
//
// bool UInventoryFunctionLibrary::GetInventoryFromInteractable(UInteractableComponent* Interactable, UInventoryCore*& Inventory)
// {
// 	if (IsValid(Interactable))
// 	{
// 		UInventoryCore* OwnerInventory = Cast<UInventoryCore>(Interactable->GetOwner()->GetComponentByClass(UInventoryCore::StaticClass()));
// 		if (IsValid(OwnerInventory))
// 		{
// 			Inventory = OwnerInventory;
// 			return true;
// 		}
// 	}
//
// 	Inventory = nullptr;
// 	return false;
// }

bool UInventoryFunctionLibrary::IsItemClassValid(const FItemData& ItemData)
{
	return ItemData.IsItemClassValid();
}

bool UInventoryFunctionLibrary::AreItemSlotsEqual(const FItemData& FirstItem, const FItemData& SecondItem)
{
	return FirstItem.EquipmentSlot == SecondItem.EquipmentSlot || (FirstItem.IsItemConsumable() && SecondItem.IsItemConsumable());
}

bool UInventoryFunctionLibrary::AreItemTheSame(const FItemData& FirstItem, const FItemData& SecondItem)
{
	return UKismetStringLibrary::EqualEqual_StrStr(FirstItem.ID, SecondItem.ID);
}

void UInventoryFunctionLibrary::PlaySoundOnTabSwitched(UObject* WorldContextObject)
{
	UWidgetSystemLibrary::GetInventoryWidgetController(WorldContextObject)->PlaySoundOnTabSwitched();
}

void UInventoryFunctionLibrary::PlaySoundOnItemDropped(UObject* WorldContextObject)
{
	UWidgetSystemLibrary::GetInventoryWidgetController(WorldContextObject)->PlaySoundOnItemDropped();
}

FSlateColor UInventoryFunctionLibrary::GetRarityColor(EItemRarity ItemRarity)
{
	switch (ItemRarity)
	{
	case EItemRarity::None:
		return FSlateColor(RayColors::Black);
	case EItemRarity::Common:
		return FSlateColor(RayColors::WHITE);
	case EItemRarity::Superior:
		return FSlateColor(RayColors::ForestGreen);
	case EItemRarity::Epic:
		return FSlateColor(RayColors::MAGENTA);
	case EItemRarity::Legendary:
		return FSlateColor(RayColors::Gold);
	case EItemRarity::Consumable:
		return FSlateColor(RayColors::LightSkyBlue);
	case EItemRarity::Usable:
		return FSlateColor(RayColors::Orange);
	}
	return FSlateColor(RayColors::Black);
}
