// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/Actors/Consumable/BaseConsumableBuff.h"


// Sets default values
ABaseConsumableBuff::ABaseConsumableBuff()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseConsumableBuff::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseConsumableBuff::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseConsumableBuff::OnRemove()
{
	switch (Effect)
	{
	case EBuffEffectCategory::None:
	case EBuffEffectCategory::AddInstantly:
		break;


	case EBuffEffectCategory::AddOverDuration:
		
		break;
		
	case EBuffEffectCategory::AddForDuration:
		break;
		
	default: break;
	}
}
