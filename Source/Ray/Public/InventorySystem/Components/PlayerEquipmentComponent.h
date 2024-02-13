// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "PlayerEquipmentComponent.generated.h"

class ULevelingComponent;
class ABaseEquippable;
class ARayCharacter;
class UInventoryCore;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddedToEquipment, const FItemData&, ItemData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemovedFromEquipment, const FItemData&, ItemData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAttach, const FItemData&, ItemData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDetach, const FItemData&, ItemData);

UCLASS(ClassGroup=("Ray|Inventory"), meta=(BlueprintSpawnableComponent))
class RAY_API UPlayerEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerEquipmentComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void InitializeEquipment();

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void BuildEquipment();

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void BuildInitialEquipment();

public:
	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	FItemData AssignItemFromEquipmentSlot(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void AttachEquippedItemToSocket(EItemSlot ItemSlot, FName SocketName);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void AttachItemToEquipment(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void DetachItemFromEquipment(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void AddItemToEquipmentArray(const FItemData& ItemData, EItemSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void AddToStackInEquipment(const FItemData& ItemData, EItemSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void RemoveItemQuantity(const FItemData& ItemData, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void RemoveItemFromEquipmentArray(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	bool TransferItemFromInventoryToEquipment(const FItemData& ItemData, const FItemData& InSlotData,
	                                          UInventoryCore* Inventory, EInputMethod InputMethod,
	                                          FText& OutFailureMessage);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void TransferItemFromEquipmentToEquipment(const FItemData& DraggedData, const FItemData& InSlotData);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	bool TryToAddToPartialStack(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void TryToUnEquipAssociatedSlot(const FItemData& ItemData, UInventoryCore* Inventory);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void SpawnAndAttachEquippableActor(const FItemData& ItemData, ARayCharacter* InOwningCharacter);

	UFUNCTION(BlueprintCallable, Category="Player Equipment Event")
	void DestroyEquippableActor(const FItemData& SlotItemData);

	UFUNCTION(BlueprintPure, Category="Player Equipment Event|Getter")
	bool CanItemBeEquipped(const FItemData& ItemData, FText& FailureMessage);

	UFUNCTION(BlueprintPure, Category="Player Equipment Event|Getter")
	bool GetItemByEquipmentSlot(EItemSlot ItemSlot, FItemData& OutItemData);

	UFUNCTION(BlueprintPure, Category="Player Equipment Event|Getter")
	bool GetItemByID(FString ItemID, FItemData& OutItemData);

	UFUNCTION(BlueprintPure, Category="Player Equipment Event|Getter")
	TArray<FItemData> GetEquipmentItems();

	UFUNCTION(BlueprintPure, Category="Player Equipment Event|Getter")
	ABaseEquippable* GetEquippedActorReference(EItemSlot ItemSlot);

private:
	FName GetWeaponSocketName(EWeaponType WeaponType);

	FName GetShieldSocketName(EWeaponType WeaponType);

public:
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="Player Equipment|NetWork Replication Events")
	void ServerTransferItemFromInventory(const FItemData& ItemData, FItemData InSlotData,
	                                     UInventoryCore* Inventory, EInputMethod InputMethod);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category="Player Equipment|NetWork Replication Events")
	void ServerTransferItemFromEquipment(const FItemData& DraggedData, FItemData InSlotData);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category="Player Equipment|NetWork Replication Events")
	void ServerSpawnEquippableActor(const FItemData& ItemData, ARayCharacter* InOwningCharacter);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category="Player Equipment|NetWork Replication Events")
	void ServerDestroyEquippableActor(const FItemData& ItemData, ARayCharacter* InOwningCharacter);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category="Player Equipment|NetWork Replication Events")
	void ServerRemoveItemQuantity(const FItemData& ItemData, int Quantiy);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category="Player Equipment|NetWork Replication Events")
	void ClientTransferItemReturnValue(bool bSuccess, const FString& FailureMessage);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category="Player Equipment|NetWork Replication Events")
	void ClientInitializeEquipment();

	UFUNCTION(BlueprintCallable, Client, Reliable, Category="Player Equipment|NetWork Replication Events")
	void ClientUpdateAddItem(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category="Player Equipment|NetWork Replication Events")
	void ClientUpdateRemovedItem(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Player Equipment|NetWork Replication Events")
	void UpdateAddedItem(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Player Equipment|NetWork Replication Events")
	void UpdateRemovedItem(const FItemData& ItemData);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Equipment|SetUp")
	TArray<FSingleDTItem> InitialEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Equipment|Config")
	bool bEnableOffHand = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Equipment|Config")
	bool bCheckRequiredItemLevel = false;

	UPROPERTY(BlueprintReadWrite, Category="Player Equipment|Reference")
	TObjectPtr<ARayCharacter> OwningCharacter;

	UPROPERTY(BlueprintReadWrite, Category="Player Equipment|Reference")
	TObjectPtr<ABaseEquippable> EquippedWeaponSlot;

	UPROPERTY(BlueprintReadWrite, Category="Player Equipment|Reference")
	TObjectPtr<ABaseEquippable> EquippedShieldSlot;

	UPROPERTY(BlueprintReadWrite, Category="Player Equipment|Reference")
	TObjectPtr<ULevelingComponent> LevelingComponent;


	UPROPERTY(BlueprintReadWrite, Category="Player Equipment")
	TMap<EItemSlot, FItemData> EquipmentData;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Player Equipment|Delegate")
	FOnAddedToEquipment OnAddedToEquipment;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Player Equipment|Delegate")
	FOnRemovedFromEquipment OnRemovedFromEquipment;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Player Equipment|Delegate")
	FOnItemAttach OnItemAttach;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Player Equipment|Delegate")
	FOnItemDetach OnItemDetach;
};
