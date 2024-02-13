
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "StatsComponent.generated.h"


class UPlayerEquipmentComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangedStatValue, EStatCategory,Stat, float,Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshStats);

UCLASS(ClassGroup=("Ray|Inventory"), meta=(BlueprintSpawnableComponent))
class RAY_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
public:
	void InitializeStats();

public:
	UFUNCTION(BlueprintCallable,Category="Stats Component Event")
	void AddStatValue(EStatCategory Stat,float Value);

	UFUNCTION(BlueprintCallable,Category="Stats Component Event")
	void RemoveStatValue(EStatCategory Stat,float Value);

	UFUNCTION(BlueprintCallable,Category="Stats Component Event")
	void AddItemStats(const FItemData& ItemData);
	
	UFUNCTION(BlueprintCallable,Category="Stats Component Event")
	void RemoveItemStats(const FItemData& ItemData);
	
	UFUNCTION(BlueprintCallable,Category="Stats Component Event")
	void SetStatValue(EStatCategory Stat,float Value);

	UFUNCTION(BlueprintCallable,Category="Stats Component Event")
	void FixAnyMiscalculations();

	UFUNCTION(BlueprintPure,Category="Stats Component Event")
	float GetStatValue(EStatCategory Stat);

	UFUNCTION(BlueprintPure,Category="Stats Component Event")
	float GetMaxStatValue(EStatCategory Stat);
	
	UFUNCTION(BlueprintPure,Category="Stats Component Event")
	float ClampStatValue(EStatCategory Stat,float Value);
	
	



public:
	UFUNCTION(Client,Reliable,BlueprintCallable,Category="Stats Component Event|Network Replication")
	void ClientInitializeStats();
	
	UFUNCTION(Server,Reliable,BlueprintCallable,Category="Stats Component Event|Network Replication")
	void ServerAddStatValue(EStatCategory Stat,float Value);

	UFUNCTION(Server,Reliable,BlueprintCallable,Category="Stats Component Event|Network Replication")
	void ServerRemoveStatValue(EStatCategory Stat,float Value);

	UFUNCTION(Server,Reliable,BlueprintCallable,Category="Stats Component Event|Network Replication")
	void ServerSetStatValue(EStatCategory Stat,float Value);
	
	UFUNCTION(Client,Reliable,BlueprintCallable,Category="Stats Component Event|Network Replication")
	void ClientStatValueChanged(EStatCategory Stat, float Value);
	
	
	

public:
	UPROPERTY(BlueprintReadOnly, Category="Stats Component|Reference")
	TObjectPtr<UPlayerEquipmentComponent> OwnerEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stats Component")
	TMap<EStatCategory, float> Stats;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Stats Component|Delegate")
	FOnChangedStatValue OnChangedStatValue;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Stats Component|Delegate")
	FOnRefreshStats OnRefreshStats;
};
