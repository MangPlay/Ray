// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "Libraries/FlowControllerLibiary.h"
#include "ConsumableBuffsComponent.generated.h"


class UPlayerEquipmentComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUsed, EItemSlot, UsedSlot, float, Cooldown);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitCircularAnimation, EStatCategory, Stat);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClearBuffsBar);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefresh);

UCLASS(ClassGroup=("Ray|Inventory"), meta=(BlueprintSpawnableComponent))
class RAY_API UConsumableBuffsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UConsumableBuffsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void OnUsePocket1();

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void OpenInputPocket1();

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void OnUsePocket2();

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void OpenInputPocket2();

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void OnUsePocket3();

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void OpenInputPocket3();

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void OnUsePocket4();

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void OpenInputPocket4();

	
	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void EventInitializeCircularAnimation(EStatCategory Stat);

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	void EventClearBuffsBar();

	UFUNCTION(BlueprintCallable, Category="ConsumableBuffs Event")
	bool TryToUseConsumable(EItemSlot Slot,float& OutCooldown);
	
public:
	UPROPERTY(BlueprintReadWrite, Category="ConsumableBuffs")
	TObjectPtr<UPlayerEquipmentComponent> PlayerEquipment;

	UPROPERTY(BlueprintReadWrite, Category="ConsumableBuffs")
	EItemSlot UsedSlot = EItemSlot::None;

	UPROPERTY(BlueprintReadWrite, Category="ConsumableBuffs")
	TMap<EStatCategory, float> StatsDecimal;

private:
	FGate Gate_Pocket1;
	FGate Gate_Pocket2;
	FGate Gate_Pocket3;
	FGate Gate_Pocket4;
	
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="ConsumableBuffs|Delegate")
	FOnItemUsed OnItemUsed;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="ConsumableBuffs|Delegate")
	FOnInitCircularAnimation OnInitCircularAnimation;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="ConsumableBuffs|Delegate")
	FOnClearBuffsBar OnClearBuffsBar;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="ConsumableBuffs|Delegate")
	FOnRefresh OnRefresh;
};
