// RAI Project 개발자 : Nam Chan woo


#include "InventorySystem/Components/ConsumableBuffsComponent.h"

#include "InventorySystem/Actors/Consumable/BaseConsumableBuff.h"
#include "InventorySystem/Components/PlayerEquipmentComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InventorySystem/Objects/MainItem.h"


// Sets default values for this component's properties
UConsumableBuffsComponent::UConsumableBuffsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UConsumableBuffsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	PlayerEquipment = Cast<UPlayerEquipmentComponent>(GetOwner()->GetComponentByClass(UPlayerEquipmentComponent::StaticClass()));
}


// Called every frame
void UConsumableBuffsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UConsumableBuffsComponent::OnUsePocket1()
{
	if (Gate_Pocket1.IsOpen())
	{
		float Pocket1Cooldown = 0;
		if (TryToUseConsumable(EItemSlot::Pocket1, Pocket1Cooldown))
		{
			FTimerDynamicDelegate TimerDynamicDelegate;
			TimerDynamicDelegate.BindUFunction(this, FName(TEXT("OpenInputPocket1")));
			UKismetSystemLibrary::K2_SetTimerDelegate(TimerDynamicDelegate, Pocket1Cooldown, false);
			Gate_Pocket1.Close();
		}
	}
}

void UConsumableBuffsComponent::OpenInputPocket1()
{
	Gate_Pocket1.Open();
}

void UConsumableBuffsComponent::OnUsePocket2()
{
	if (Gate_Pocket2.IsOpen())
	{
		float Pocket2Cooldown = 0;
		if (TryToUseConsumable(EItemSlot::Pocket2, Pocket2Cooldown))
		{
			FTimerDynamicDelegate TimerDynamicDelegate;
			TimerDynamicDelegate.BindUFunction(this, FName(TEXT("OpenInputPocket2")));
			UKismetSystemLibrary::K2_SetTimerDelegate(TimerDynamicDelegate, Pocket2Cooldown, false);
			Gate_Pocket2.Close();
		}
	}
}

void UConsumableBuffsComponent::OpenInputPocket2()
{
	Gate_Pocket2.Open();
}

void UConsumableBuffsComponent::OnUsePocket3()
{
	if (Gate_Pocket3.IsOpen())
	{
		float Pocket3Cooldown = 0;
		if (TryToUseConsumable(EItemSlot::Pocket3, Pocket3Cooldown))
		{
			FTimerDynamicDelegate TimerDynamicDelegate;
			TimerDynamicDelegate.BindUFunction(this, FName(TEXT("OpenInputPocket3")));
			UKismetSystemLibrary::K2_SetTimerDelegate(TimerDynamicDelegate, Pocket3Cooldown, false);
			Gate_Pocket3.Close();
		}
	}
}

void UConsumableBuffsComponent::OpenInputPocket3()
{
	Gate_Pocket3.Open();
}

void UConsumableBuffsComponent::OnUsePocket4()
{
	if (Gate_Pocket4.IsOpen())
	{
		float Pocket4Cooldown = 0;
		if (TryToUseConsumable(EItemSlot::Pocket4, Pocket4Cooldown))
		{
			FTimerDynamicDelegate TimerDynamicDelegate;
			TimerDynamicDelegate.BindUFunction(this, FName(TEXT("OpenInputPocket4")));
			UKismetSystemLibrary::K2_SetTimerDelegate(TimerDynamicDelegate, Pocket4Cooldown, false);
			Gate_Pocket4.Close();
		}
	}
}

void UConsumableBuffsComponent::OpenInputPocket4()
{
	Gate_Pocket4.Open();
}

void UConsumableBuffsComponent::EventInitializeCircularAnimation(EStatCategory Stat)
{
	OnInitCircularAnimation.Broadcast(Stat);
}

void UConsumableBuffsComponent::EventClearBuffsBar()
{
	OnClearBuffsBar.Broadcast();
}

bool UConsumableBuffsComponent::TryToUseConsumable(EItemSlot Slot, float& OutCooldown)
{
	UsedSlot = Slot;
	FItemData ItemData;
	if (PlayerEquipment->GetItemByEquipmentSlot(UsedSlot, ItemData))
	{
		UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject();
		if (IsValid(MainItemCDO->ConsumableClass))
		{
			ABaseConsumableBuff* ConsumableBuffCDO =
				ItemData.ItemClass.GetDefaultObject()->ConsumableClass.GetDefaultObject();

			float Cooldown = ConsumableBuffCDO->Cooldown;

			ABaseConsumableBuff* ConsumableBuff = Cast<ABaseConsumableBuff>(
				UGameplayStatics::GetActorOfClass(this, MainItemCDO->ConsumableClass));

			if (IsValid(ConsumableBuff))
			{
				ConsumableBuff->OnRemove();
			}

			// 스폰할 액터가 있는 월드 컨텍스트를 얻습니다.
			UWorld* World = GetWorld();
			if (World != nullptr)
			{
				FVector SpawnLocation = FVector(0.0f, 0.0f, 0.0f);
				FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();

				// 스폰 함수를 호출하여 액터를 생성합니다.
				ABaseConsumableBuff* SpawnConsumableBuff = World->SpawnActor<ABaseConsumableBuff>(
					MainItemCDO->ConsumableClass, SpawnLocation, SpawnRotation, SpawnParams);
			}

			FItemData EquipmentItemData;
			PlayerEquipment->GetItemByEquipmentSlot(UsedSlot, EquipmentItemData);
			PlayerEquipment->ServerRemoveItemQuantity(EquipmentItemData, 1);
			OnItemUsed.Broadcast(UsedSlot, Cooldown);

			OutCooldown = Cooldown;
			return true;
		}
	}

	OutCooldown = 0.0f;
	return false;
}
