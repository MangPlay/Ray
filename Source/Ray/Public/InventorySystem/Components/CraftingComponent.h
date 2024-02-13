// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "CraftingComponent.generated.h"


UCLASS(ClassGroup=("Ray|Inventory"), meta=(BlueprintSpawnableComponent))
class RAY_API UCraftingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCraftingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category="Crafting Component Event")
	void FocusSelectedItem(const FCrafting& CraftingItemData);
	
	UFUNCTION(BlueprintPure, Category="Crafting Component Event")
	void GetCraftableData(const FCrafting& CraftingData,FItemData& CraftableItem,TArray<FSingleDTItem>& RequiredMaterials);

public:
	UPROPERTY(BlueprintReadWrite, Category="Crafting Component")
	FCrafting CurrentCraftingData;
	
	UPROPERTY(BlueprintReadWrite, Category="Crafting Component")
	TArray<FCrafting> WeaponsList;

	UPROPERTY(BlueprintReadWrite, Category="Crafting Component")
	TArray<FCrafting> ArmorList;
};
