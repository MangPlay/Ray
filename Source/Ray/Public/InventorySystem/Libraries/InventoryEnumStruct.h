#pragma once

#include "CoreMinimal.h"
#include "InventoryEnumStruct.generated.h"

class UMainItem;

UENUM(BlueprintType)
enum class EStatCategory : uint8
{
	None UMETA(DisplayName="None"),
	Health UMETA(DisplayName="Health"),
	MaxHealth UMETA(DisplayName="Max Health"),
	Stamina UMETA(DisplayName="Stamina"),
	MaxStamina UMETA(DisplayName="Max Stamina"),
	Armor UMETA(DisplayName="Armor"),
	Damage UMETA(DisplayName="Damage"),
	AttackSpeed UMETA(DisplayName="Attack Speed")
};

UENUM(BlueprintType)
enum class EWidgetType : uint8
{
	None,
	Inventory,
	Equipment,
	Crafting,
	Vendor,
	Storage,
	LoadGame,
	Abilities
};

UENUM(BlueprintType)
enum class EWidgetPopUp : uint8
{
	None UMETA(DisplayName="None"),
	SplitStackPopup UMETA(DisplayName="Split Stack Pop up"),
	ConfirmationPopup UMETA(DisplayName="Confirmation Pop up"),
	TextDocumentPopup UMETA(DisplayName="Text Document Pop up")
};

UENUM(BlueprintType)
enum class EAutoSaveTimer : uint8
{
	OFF UMETA(DisplayName="OFF"),
	Every10Min UMETA(DisplayName="Every 10 min"),
	Every5Min UMETA(DisplayName="Every 5 min"),
	Every3Min UMETA(DisplayName="Every 3 min"),
	Every60Sec UMETA(DisplayName="Every 60 sec")
};


UENUM(BlueprintType)
enum class EInventoryPanels : uint8
{
	None,
	P1,
	P2,
	P3,
	P4
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	OneHand,
	TwoHand,
	OffHand
};

UENUM(BlueprintType)
enum class EWeaponSubType : uint8
{
	None,
	Sword,
	WarHammer,
	Axe,
	Mace
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	None,
	Common,
	Superior,
	Epic,
	Legendary,
	Consumable,
	Usable
};

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	None,
	TextDocument
};


UENUM(BlueprintType)
enum class EItemRemoveType : uint8
{
	Default,
	OnConfirmation UMETA(DisplayName="On Confirmation"),
	CannotBeRemoved UMETA(DisplayName="Cannot be Removed")
};

UENUM(BlueprintType)
enum class EInputMethod : uint8
{
	RightClick UMETA(DisplayName="Right Click"),
	DragAndDrop UMETA(DisplayName="Drag And Drop"),
};


UENUM(BlueprintType)
enum class ESortMethod : uint8
{
	Quicksort UMETA(DisplayName="Quick Sort"),
	ByType UMETA(DisplayName="By Type"),
	ByRarity UMETA(DisplayName="by Rarity"),
	ByValue UMETA(DisplayName="By Value"),
	ByWeight UMETA(DisplayName="By Weight")
};

UENUM(BlueprintType)
enum class EItemSlot : uint8
{
	None,
	Head,
	Gloves,
	Neck,
	ChestPlate,
	Legs,
	Boots,
	Weapon,
	Shield,
	Pocket1,
	Pocket2,
	Pocket3,
	Pocket4,
	NewSlot
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Armor,
	Weapon,
	Shield,
	Food,
	Potion,
	CraftingIngredient,
	QuestItem,
	Other,
	Currency
};

UENUM(BlueprintType)
enum class EItemDestination : uint8
{
	None,
	InventorySlot,
	EquipmentSlot,
	VendorSlot,
	StorageSlot,
	DropBar
};

UENUM(BlueprintType)
enum class EBuffEffectCategory : uint8
{
	None,
	AddInstantly,
	AddOverDuration,
	AddForDuration,
};

UENUM(BlueprintType)
enum class EAdditionalBuffEffect : uint8
{
	None,
	Drunk,
	NightVision
};

UENUM(BlueprintType)
enum class EInteractionInputType : uint8
{
	Single,
	Holding UMETA(DisplayName="Holding"),
	MultipleANDMashing UMETA(DisplayName="Multiple/Mashing")
};


UENUM(BlueprintType)
enum class ESaveType : uint8
{
	QuickSave UMETA(DisplayName="Persistent"),
	AutoSave UMETA(DisplayName="Only Once"),
	ManualSave UMETA(DisplayName="Manual Save"),
	Checkpoint UMETA(DisplayName="Check Point")
};

UENUM(BlueprintType)
enum class ECraftingStation : uint8
{
	None UMETA(DisplayName="None"),
	Forge UMETA(DisplayName="Forge")
};

UENUM(BlueprintType)
enum class EGamepadInputs : uint8
{
	FaceButtonBottom UMETA(DisplayName="Face Button Bottom"),
	FaceButtonRight UMETA(DisplayName="Face Button Right"),
	FaceButtonLeft UMETA(DisplayName="Face Button Left"),
	FaceButtonTop UMETA(DisplayName="Face Button Top"),
	LeftThumbstick UMETA(DisplayName="Left Thumbstick"),
	LeftThumbstickButton UMETA(DisplayName="Left Thumbstick Button"),
	RightThumbstick UMETA(DisplayName="Right Thumbstick"),
	RightThumbstickButton UMETA(DisplayName="Right Thumbstick Button"),
	LeftShoulder UMETA(DisplayName="Left Shoulder"),
	RightShoulder UMETA(DisplayName="Right Shoulder"),
	LeftTrigger UMETA(DisplayName="Left Trigger"),
	RightTrigger UMETA(DisplayName="Right Trigger"),
	DpadUp UMETA(DisplayName="Dpad Up"),
	DpadDown UMETA(DisplayName="Dpad Down"),
	DpadRight UMETA(DisplayName="Dpad Right"),
	DpadLeft UMETA(DisplayName="Dpad Left"),
	SpecialLeft UMETA(DisplayName="Special Left"),
	SpecialRight UMETA(DisplayName="Special Right"),
};

UENUM(BlueprintType)
enum class EKeyBoardMouseInputs : uint8
{
	A UMETA(DisplayName="Key_A"),
	B UMETA(DisplayName="Key_B"),
	C UMETA(DisplayName="Key_C"),
	D UMETA(DisplayName="Key_D"),
	E UMETA(DisplayName="Key_E"),
	F UMETA(DisplayName="Key_F"),
	G UMETA(DisplayName="Key_G"),
	H UMETA(DisplayName="Key_H"),
	I UMETA(DisplayName="Key_I"),
	J UMETA(DisplayName="Key_J"),
	K UMETA(DisplayName="Key_K"),
	L UMETA(DisplayName="Key_L"),
	M UMETA(DisplayName="Key_M"),
	N UMETA(DisplayName="Key_N"),
	O UMETA(DisplayName="Key_O"),
	P UMETA(DisplayName="Key_P"),
	Q UMETA(DisplayName="Key_Q"),
	R UMETA(DisplayName="Key_R"),
	S UMETA(DisplayName="Key_S"),
	T UMETA(DisplayName="Key_T"),
	U UMETA(DisplayName="Key_U"),
	V UMETA(DisplayName="Key_V"),
	W UMETA(DisplayName="Key_W"),
	X UMETA(DisplayName="Key_X"),
	Y UMETA(DisplayName="Key_Y"),
	Z UMETA(DisplayName="Key_Z"),

	MouseMove UMETA(DisplayName="MouseMove"),
	MouseLeftClick UMETA(DisplayName="MouseLeftClick"),
	MouseRightClick UMETA(DisplayName="MouseRightClick"),
	MouseMiddleClick UMETA(DisplayName="MouseMiddleClick"),
};


USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	FString ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	EItemSlot EquipmentSlot = EItemSlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	TSubclassOf<UMainItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	int32 Durability = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	int32 Index = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	bool bIsEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	bool bAlreadyUsed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	float ValueModifier = 1.0f;

	FItemData();

	FItemData(EItemSlot InEquipmentSlot);

	bool operator==(const FItemData& other) const;

	bool IsItemClassValid() const;

	bool IsItemCurrency() const;

	bool IsItemConsumable() const;

	float CalculateStackedItemValue() const;

	float CalculateStackedItemWeight() const;

	EInventoryPanels GetInventoryPanelFromItem() const;
};


USTRUCT(BlueprintType)
struct FSingleDTItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Single DT Item")
	FDataTableRowHandle TableAndRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Single DT Item")
	int32 Quantity = 0;
};

USTRUCT(BlueprintType)
struct FItemsStatRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Single DT Item")
	EStatCategory Stat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Single DT Item")
	float Value = 0;
};

USTRUCT(BlueprintType)
struct FRandomizedLootTable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Randomized Loot Table")
	TObjectPtr<UDataTable> DataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Randomized Loot Table")
	int32 MinLootItems = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Randomized Loot Table")
	int32 MaxLootItems = 0;
};

USTRUCT(BlueprintType)
struct FInventory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FItemData> InventoryArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 InventorySize = 30;

	FInventory();

	void BuildInventory();

	bool HasPartialStack(const FItemData& ItemData, int32& OutSlotIndex);

	bool FindEmptySlotInArray(int32& OutEmptySlotIndex);

	bool FindItemByID(const FString& ItemID, FItemData& OutItemData);

	bool FindItemIndex(const FItemData& ItemData, int32& OutSlotIndex);

	bool IsValidSlotIndex(int32 Index);

	int32 FindAmountOfEmptySlots();

	TArray<FItemData> GetAllItemsOfType(EItemType ItemType);
};


USTRUCT(BlueprintType)
struct FCrafting : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crafting")
	FSingleDTItem CraftableItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crafting")
	TArray<FSingleDTItem> RequiredItems;
};
