// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "BaseConsumableBuff.generated.h"

UCLASS()
class RAY_API ABaseConsumableBuff : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseConsumableBuff();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
public:
	UFUNCTION(BlueprintCallable,Category="Consumable Buff Event")
	void OnRemove();

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	EStatCategory Stat;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	EBuffEffectCategory Effect;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	float Value;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	int32 NumberOfReps;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	float Duration;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	EAdditionalBuffEffect AdditionalEffect;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	float Cooldown = 0.1f;;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	int32 CurrentRep=0;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	FTimerHandle OverDurationTimer;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff")
	FTimerHandle ForDurationTimer;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Consumable Buff|SetUp")
	USoundBase* ConsumSound;
};
