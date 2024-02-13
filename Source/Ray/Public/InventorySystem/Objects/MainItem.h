#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "MainItem.generated.h"

class ABaseEquippable;
class ABaseConsumableBuff;


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RAY_API UMainItem : public UObject
{
	GENERATED_BODY()

public:
	UMainItem();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Info")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Info")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Info")
	FText TextDocument;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Details")
	UTexture2D* Image = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Details")
	EItemType Type = EItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Details")
	EItemRarity Rarity = EItemRarity::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Details")
	EInventoryPanels InventoryPanel = EInventoryPanels::P1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Details")
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Details")
	EWeaponSubType WeaponSubType = EWeaponSubType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Stats")
	double Weight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Stats")
	double Value = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Stats")
	int32 RequiredLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Stats")
	TArray<FItemsStatRow> Stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Equip")
	TSubclassOf<ABaseEquippable> EquippableClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Equip")
	USkeletalMesh* SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Consumable Buff")
	TSubclassOf<ABaseConsumableBuff> ConsumableClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Stacking")
	bool bStackable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Durability")
	bool bUseDurability = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Usage")
	EItemUseType UseTyp = EItemUseType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Usage")
	EItemRemoveType RemoveType = EItemRemoveType::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Loot Randomization")
	int32 DropPercentage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Loot Randomization")
	TSet<TSubclassOf<UMainItem>> LootClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Loot Randomization")
	int32 MinRandQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Loot Randomization")
	int32 MaxRandQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Loot Randomization")
	int32 MinRandDurability = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Loot Randomization")
	int32 MaxRandDurability = 100;

	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Item|Item Interactable")
	// TSubclassOf<ABaseItemPickUp> InteractableClass;
};
