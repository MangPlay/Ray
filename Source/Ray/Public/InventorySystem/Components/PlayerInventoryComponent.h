// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "InventoryCore.h"
#include "PlayerInventoryComponent.generated.h"


class UInteractableComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeAllItems);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemLooted, const FItemData&, ItemData);

UCLASS(ClassGroup=("Ray|Inventory"), meta=(BlueprintSpawnableComponent))
class RAY_API UPlayerInventoryComponent : public UInventoryCore
{
	GENERATED_BODY()

public:
	UPlayerInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void InitializeInventory() override;

	UFUNCTION()
	void BindEvents();

public:
	UFUNCTION(BlueprintCallable, Category="Player Inventory Event|Interaction")
	void SetNewInteractable(UInteractableComponent* Interactable);

	UFUNCTION(BlueprintCallable, Category="Player Inventory Event|Interaction")
	void RemoveCurrentInteractable();

public:
	UFUNCTION(BlueprintCallable, Category="Player Inventory Event|Input")
	void InputInventory();

	// UFUNCTION(BlueprintCallable, Category="Player Inventory Event|Input")
	// void InputCloseWidget();

	UFUNCTION(BlueprintCallable, Category="Player Inventory Event|Input")
	void InputTakeAll();



public:
	/*Inventory Core Override*/
	virtual bool TransferItemFromInventory(FItemData& ItemData, const FItemData& InSlotData, EInputMethod Method,
	                                       UInventoryCore* Sender, FText& OutFailureMessage) override;

	virtual void SplitItemsInInventory(UInventoryCore* Sender, const FItemData& ItemData, const FItemData& InSlotData,
	                                   const FItemData& StackableLeft, EInputMethod InputMethod, EItemDestination Initiator,
	                                   EItemDestination Destination) override;

	virtual void ConfirmationPopupAccepted(UInventoryCore* Sender, const FItemData& ItemData, const FItemData& InSlotData,
	                                       EInputMethod InputMethod, EItemDestination Initiator,
	                                       EItemDestination Destination) override;
	/*Inventory Core Override*/

	UFUNCTION(BlueprintCallable, Category="Player Inventory Event")
	void TakeItem(const FItemData& ItemData, UInventoryCore* Sender);

	UFUNCTION(BlueprintCallable, Category="Player Inventory Event")
	void TakeAllItems(UInventoryCore* Sender);

	UFUNCTION(BlueprintCallable, Category="Player Inventory Event")
	void IncreaseItemDurability(const FItemData& ItemData, int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Player Inventory Event")
	void DecreaseItemDurability(const FItemData& ItemData, int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Player Inventory Event")
	void DropItemOnTheGround(const FItemData& ItemData, EItemDestination Initiator);

	UFUNCTION(BlueprintCallable, Category="Player Inventory Event")
	void SpawnLootBagNearThePlayer(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category="Player Inventory Event")
	void SpawnItemMeshNearThePlayer(const FItemData& ItemData);

protected:
	//삭제해야할코드
	// UFUNCTION(BlueprintPure, Category="Player Inventory Event")
	 bool IsCollidingWithLootBag(FVector Start, FVector End, AActor*& OutDroppedBag);
	// UFUNCTION(BlueprintPure, Category="Player Inventory Event")
	// bool IsCollidingWithLootBag(FVector Start, FVector End, ALootBag*& OutDroppedBag);

public:
	/*Inventory Core Override*/
	virtual void ClientInitializeInventory() override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Player Inventory|Network Replication Events")
	void ServerTakeItem(const FItemData& ItemData, UInventoryCore* Sender);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Player Inventory|Network Replication Events")
	void ServerTakeAllItems(UInventoryCore* Sender);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Player Inventory|Network Replication Events")
	void ServerSpawnLootBag(const FItemData& ItemData);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Player Inventory|Network Replication Events")
	void ServerSpawnInteractable(const FItemData& ItemData);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Player Inventory|Network Replication Events")
	void ServerDropItemOnTheGround(const FItemData& ItemData, EItemDestination Initiator);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Player Inventory|Network Replication Events")
	void ServerChangeDroppedIndex(int NewDroppedIndex);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Player Inventory|Network Replication Events")
	void ClientTakeItemReturnValue(bool bSuccess, const FString& FailureMessage, bool bRemoveInteraction);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Player Inventory|Network Replication Events")
	void ClientItemLooted(const FItemData& ItemData);

public:
	//삭제해야할코드
	// UPROPERTY(EditAnywhere, Category="PlayerInventory|Setup")
	// TSubclassOf<ALootBag> LootBagClass;
	//
	// UPROPERTY(BlueprintReadWrite, Category="PlayerInventory")
	// TObjectPtr<ALootBag> LootBagRef;

	UPROPERTY(BlueprintReadWrite, Replicated, Category="PlayerInventory")
	int32 DroppedIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category="PlayerInventory")
	TObjectPtr<UInteractableComponent> CurrentInteractable;



public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Player Inventory|Delegate")
	FOnTakeItem OnTakeItem;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Player Inventory|Delegate")
	FOnTakeAllItems OnTakeAllItems;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Player Inventory|Delegate")
	FOnItemLooted OnItemLooted;
};
