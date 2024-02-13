// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryFunctionLibrary.generated.h"


class UCraftingComponent;
class ULevelingComponent;
class UConsumableBuffsComponent;
class UInteractionComponent;
class UStatsComponent;
class UPlayerEquipmentComponent;
/**
 * 
 */
UCLASS()
class RAY_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static bool IsItemClassValid(const FItemData& ItemData);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static bool AreItemSlotsEqual(const FItemData& FirstItem, const FItemData& SecondItem);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static bool AreItemTheSame(const FItemData& FirstItem, const FItemData& SecondItem);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static bool AreItemsStackable(const FItemData& FirstItem, const FItemData& SecondItem);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static bool AreWeaponTypesEqual(const FItemData& FirstItem, const FItemData& SecondItem);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static float CalculateStackedItemWeight(const FItemData& ItemData);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static float CalculateStackedItemValue(const FItemData& ItemData);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static EInventoryPanels GetInventoryPanelFromItem(const FItemData& ItemData);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static bool CanItemBeRemoved(const FItemData& ItemData);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static bool IsStackableAndHaveStacks(const FItemData& ItemData, int32 Quantity);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|Item")
	static bool CanWeaponsBeSwapped(const FItemData& FirstItem, const FItemData& SecondItem);

	// UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Utility")
	// static void FindEmptySlotInArray(const TArray<FItemData>& Inventory, bool& bOutFound, int32& OutSlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Item")
	static bool HasPartialStack(const TArray<FItemData>& ItemArray, const FItemData& ItemData, int32& OutSlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Item")
	static bool FindItemByID(const TArray<FItemData> ItemArray, FString ItemID, FItemData& OutItem);
	//
	// UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Utility")
	// static bool FindItemIndex(const TArray<FItemData>& Inventory,const FItemData& ItemData,int32& OutSlotIndex);

	// UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Utility")
	// static TArray<FItemData> GetAllItemsOfType(const TArray<FItemData>& Inventory, EItemType ItemType);
	//
	// UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Utility")
	// static int32 FindAmountOfEmptySlots(const TArray<FItemData>& Inventory);


	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Sorting")
	static TArray<FItemData> QuickSortItems(const TArray<FItemData>& InventoryArray);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Sorting")
	static TArray<FItemData> SortItemsByRarity(const TArray<FItemData>& InventoryArray);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Sorting")
	static TArray<FItemData> SortItemsByValue(const TArray<FItemData>& InventoryArray);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Sorting")
	static TArray<FItemData> SortItemsByWeight(const TArray<FItemData>& InventoryArray);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Sorting")
	static TArray<FItemData> SortItemsByType(const TArray<FItemData>& InventoryArray);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Player Capture")
	static void StartPlayerCapture2D(AActor* Controller);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Player Capture")
	static void StopPlayerCapture2D(AActor* Controller);


	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Getter")
	static UPlayerInventoryComponent* GetPlayerInventory(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Getter")
	static UPlayerEquipmentComponent* GetPlayerEquipment(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Getter")
	static UStatsComponent* GetStatsComponent(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Getter")
	static UInteractionComponent* GetInteractionComponent(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Getter")
	static UConsumableBuffsComponent* GetConsumableBuffsComponent(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Getter")
	static ULevelingComponent* GetLevelingComponent(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Getter")
	static UCraftingComponent* GetCraftingComponent(AActor* Target);


	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Equipment")
	static bool FindEmptyEquipmentSlot(const TMap<EItemSlot, FItemData>& EquipmentData,
	                                   const TArray<EItemSlot>& SlotsToSearch, EItemSlot DefaultSlot,
	                                   EItemSlot& OutSlot);


	// UFUNCTION(BlueprintPure, Category="Inventory Function Library|Interaction System")
	// static UInteractableComponent* GetCurrentInteractableObject(AActor* Target);
	//
	// UFUNCTION(BlueprintPure, Category="Inventory Function Library|Interaction System")
	// static bool GetInventoryFromInteractable(UInteractableComponent* Interactable, UInventoryCore*& Inventory);


	// UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Widget Event")
	// static void PlaySoundOnOpeningWidget(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Sound Effects",
		meta=(WorldContext="WorldContextObject"))
	static void PlaySoundOnTabSwitched(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="Inventory Function Library|Sound Effects",
		meta=(WorldContext="WorldContextObject"))
	static void PlaySoundOnItemDropped(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="Inventory Function Library|User Interface")
	static FSlateColor GetRarityColor(EItemRarity ItemRarity);
};
