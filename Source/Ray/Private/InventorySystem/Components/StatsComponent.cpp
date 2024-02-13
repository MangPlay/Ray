// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem/Components/StatsComponent.h"

#include "InventorySystem/Components/PlayerEquipmentComponent.h"
#include "InventorySystem/Objects/MainItem.h"


// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStatsComponent::InitializeStats()
{
	ClientInitializeStats();
}

void UStatsComponent::AddStatValue(EStatCategory Stat, float Value)
{
	if (!GetOwner()->HasAuthority()) return;

	if (!Stats.Contains(Stat)) return;

	float AddStatValue = *Stats.Find(Stat) + Value;

	Stats.Add(Stat, ClampStatValue(Stat, AddStatValue));

	ClientStatValueChanged(Stat, *Stats.Find(Stat));
}

void UStatsComponent::RemoveStatValue(EStatCategory Stat, float Value)
{
	if (!GetOwner()->HasAuthority()) return;

	if (!Stats.Contains(Stat)) return;

	float RemoveStatValue = *Stats.Find(Stat) - Value;

	Stats.Add(Stat, ClampStatValue(Stat, RemoveStatValue));

	ClientStatValueChanged(Stat, *Stats.Find(Stat));

	FixAnyMiscalculations();
}

void UStatsComponent::AddItemStats(const FItemData& ItemData)
{
	for (const FItemsStatRow& StatRow : ItemData.ItemClass.GetDefaultObject()->Stats)
	{
		ServerAddStatValue(StatRow.Stat, StatRow.Value);
	}
}

void UStatsComponent::RemoveItemStats(const FItemData& ItemData)
{
	for (const FItemsStatRow& StatRow : ItemData.ItemClass.GetDefaultObject()->Stats)
	{
		ServerRemoveStatValue(StatRow.Stat, StatRow.Value);
	}
}

void UStatsComponent::SetStatValue(EStatCategory Stat, float Value)
{
	if (!GetOwner()->HasAuthority()) return;

	if (!Stats.Contains(Stat)) return;

	Stats.Add(Stat, ClampStatValue(Stat, Value));

	ClientStatValueChanged(Stat, *Stats.Find(Stat));

	FixAnyMiscalculations();
}

void UStatsComponent::FixAnyMiscalculations()
{
	TArray<EStatCategory> StatArray{EStatCategory::Health, EStatCategory::Stamina};

	for (EStatCategory Stat : StatArray)
	{
		float localFixStatValue = 0;
		if (Stat == EStatCategory::Health)
			localFixStatValue = GetStatValue(EStatCategory::MaxHealth);
		else if (Stat == EStatCategory::Stamina)
			localFixStatValue = GetStatValue(EStatCategory::MaxStamina);

		if (GetStatValue(Stat) > localFixStatValue)
		{
			Stats.Add(Stat, ClampStatValue(Stat, localFixStatValue));
			ClientStatValueChanged(Stat, *Stats.Find(Stat));
		}
	}
}

float UStatsComponent::GetStatValue(EStatCategory Stat)
{
	if (Stats.Contains(Stat))
	{
		return *Stats.Find(Stat);
	}
	return 0.0f;
}

float UStatsComponent::GetMaxStatValue(EStatCategory Stat)
{
	if (Stats.Contains(Stat))
	{
		switch (Stat)
		{
		case EStatCategory::None:
			return 0.0f;

		case EStatCategory::Health:
		case EStatCategory::MaxHealth:
			return GetStatValue(EStatCategory::MaxHealth);

		case EStatCategory::Stamina:
		case EStatCategory::MaxStamina:
			return GetStatValue(EStatCategory::MaxStamina);

		case EStatCategory::Armor:
		case EStatCategory::Damage:
		case EStatCategory::AttackSpeed:
			return 999.0f;
		}
	}
	return 0.0f;
}

float UStatsComponent::ClampStatValue(EStatCategory Stat, float Value)
{
	if (Stat == EStatCategory::Health)
	{
		return FMath::Clamp(Value, 0.0f, GetStatValue(EStatCategory::MaxHealth));
	}
	if (Stat == EStatCategory::Stamina)
	{
		return FMath::Clamp(Value, 0.0f, GetStatValue(EStatCategory::MaxStamina));
	}

	return FMath::Clamp(Value, 0, Value);
}

void UStatsComponent::ClientInitializeStats_Implementation()
{
	OnRefreshStats.Broadcast();
	UPlayerEquipmentComponent* CastOwnerEquipment = Cast<UPlayerEquipmentComponent>(
		GetOwner()->GetComponentByClass(UPlayerEquipmentComponent::StaticClass()));
	if (IsValid(CastOwnerEquipment))
	{
		OwnerEquipment = CastOwnerEquipment;
		OwnerEquipment->OnAddedToEquipment.AddDynamic(this, &UStatsComponent::AddItemStats);
		OwnerEquipment->OnRemovedFromEquipment.AddDynamic(this, &UStatsComponent::RemoveItemStats);
	}
}

void UStatsComponent::ClientStatValueChanged_Implementation(EStatCategory Stat, float Value)
{
	Stats.Add(Stat, Value);
	OnChangedStatValue.Broadcast(Stat, Value);
}

void UStatsComponent::ServerSetStatValue_Implementation(EStatCategory Stat, float Value)
{
	SetStatValue(Stat, Value);
}

void UStatsComponent::ServerRemoveStatValue_Implementation(EStatCategory Stat, float Value)
{
	RemoveStatValue(Stat, Value);
}

void UStatsComponent::ServerAddStatValue_Implementation(EStatCategory Stat, float Value)
{
	AddStatValue(Stat, Value);
}
