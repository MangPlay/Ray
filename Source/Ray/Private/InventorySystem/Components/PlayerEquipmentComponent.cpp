// RAI Project 개발자 : Nam Chan woo


#include "InventorySystem/Components/PlayerEquipmentComponent.h"

#include "Charater/RayCharacter.h"
#include "InventorySystem/Actors/Equippable/BaseEquippable.h"
#include "InventorySystem/Components/InventoryCore.h"
#include "InventorySystem/Components/LevelingComponent.h"
#include "InventorySystem/Libraries/InventoryFunctionLibrary.h"
#include "InventorySystem/Objects/MainItem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/WidgetSystemLibrary.h"
#include "UI/WidgetController/InventoryWidgetController.h"


// Sets default values for this component's properties
UPlayerEquipmentComponent::UPlayerEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UPlayerEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UPlayerEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerEquipmentComponent::InitializeEquipment()
{
	ClientInitializeEquipment();

	if (APlayerController* OwnerPlayerController = Cast<APlayerController>(GetOwner()))
	{
		if (ARayCharacter* CastCharacter = Cast<ARayCharacter>(OwnerPlayerController->GetPawn()))
		{
			OwningCharacter = CastCharacter;

			BuildEquipment();

			BuildInitialEquipment();
		}
	}
}

void UPlayerEquipmentComponent::BuildEquipment()
{
	for (EItemSlot Slot = EItemSlot::None; Slot < EItemSlot::NewSlot; Slot = static_cast<EItemSlot>(static_cast<uint8>(Slot) + 1))
	{
		if (EquipmentData.Contains(Slot))
		{
			EquipmentData.Find(Slot)->EquipmentSlot = Slot;
			EquipmentData.Find(Slot)->bIsEquipped = true;
		}
		else
		{
			EquipmentData.Add(Slot, FItemData(Slot));
		}
	}
}

void UPlayerEquipmentComponent::BuildInitialEquipment()
{
	for (const FSingleDTItem& SingleDT : InitialEquipment)
	{
		FItemData* SingleItem = SingleDT.TableAndRow.DataTable->FindRow<FItemData>(SingleDT.TableAndRow.RowName,TEXT(""));

		if (!SingleItem) continue;

		SingleItem->Quantity = FMath::Clamp(SingleDT.Quantity, 1, SingleDT.Quantity);
		const FItemData InSlotItem = AssignItemFromEquipmentSlot(*SingleItem);
		AddItemToEquipmentArray(*SingleItem, InSlotItem.EquipmentSlot);
		AttachItemToEquipment(*SingleItem);

		if (UInventoryCore* OwnerInventoryCore = Cast<UInventoryCore>(GetOwner()->GetComponentByClass(UInventoryCore::StaticClass())))
		{
			OwnerInventoryCore->AddWeightToInventory(*SingleItem);
		}
	}
	InitialEquipment.Empty();
}

FItemData UPlayerEquipmentComponent::AssignItemFromEquipmentSlot(const FItemData& ItemData)
{
	TSet<EItemSlot> ConsumableItemSlots{EItemSlot::Pocket1, EItemSlot::Pocket2, EItemSlot::Pocket3, EItemSlot::Pocket4};

	if (ConsumableItemSlots.Contains(ItemData.EquipmentSlot))
	{
		EItemSlot EmptyConsumableSlot;
		UInventoryFunctionLibrary::FindEmptyEquipmentSlot(EquipmentData, ConsumableItemSlots.Array(),
		                                                  EItemSlot::Pocket1, EmptyConsumableSlot);

		FItemData EmptyConsumableItem;
		GetItemByEquipmentSlot(EmptyConsumableSlot, EmptyConsumableItem);
		return EmptyConsumableItem;
	}

	UMainItem* ItemCDO = ItemData.ItemClass.GetDefaultObject();

	if (bEnableOffHand && ItemCDO->WeaponType == EWeaponType::OneHand)
	{
		FItemData ShieldItemData;
		if (!GetItemByEquipmentSlot(EItemSlot::Shield, ShieldItemData))
		{
			TArray<EItemSlot> WeaponSlot{EItemSlot::Weapon, EItemSlot::Shield};
			EItemSlot EmptyWeaponSlot;
			UInventoryFunctionLibrary::FindEmptyEquipmentSlot(EquipmentData, WeaponSlot, EItemSlot::Weapon,
			                                                  EmptyWeaponSlot);
			FItemData EmptyWeaponItem;
			GetItemByEquipmentSlot(EmptyWeaponSlot, EmptyWeaponItem);
			return EmptyWeaponItem;
		}

		FItemData WeaponItemData;
		GetItemByEquipmentSlot(EItemSlot::Weapon, WeaponItemData);
		return WeaponItemData;
	}


	FItemData OutItemData;
	GetItemByEquipmentSlot(ItemData.EquipmentSlot, OutItemData);
	return OutItemData;
}

void UPlayerEquipmentComponent::AttachEquippedItemToSocket(EItemSlot ItemSlot, FName SocketName)
{
	ABaseEquippable* SlotEquippable = GetEquippedActorReference(ItemSlot);
	if (!IsValid(SlotEquippable)) return;

	auto AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	SlotEquippable->AttachToComponent(OwningCharacter->GetMesh(), AttachmentTransformRules, SocketName);
}

void UPlayerEquipmentComponent::AttachItemToEquipment(const FItemData& ItemData)
{
	if (!ItemData.IsItemClassValid()) return;

	if (!IsValid(OwningCharacter)) return;

	OnItemAttach.Broadcast(ItemData);

	UMainItem* ItemCDO = ItemData.ItemClass.GetDefaultObject();
	EItemSlot ItemSlot = ItemData.EquipmentSlot;
	USkeletalMesh* SkeletalMesh = ItemCDO->SkeletalMesh;

	switch (ItemSlot)
	{
	case EItemSlot::Head:
	case EItemSlot::Gloves:
	case EItemSlot::Neck:
	case EItemSlot::ChestPlate:
	case EItemSlot::Legs:
	case EItemSlot::Boots:
		OwningCharacter->ServerUpdateEquipmentMesh(ItemSlot, SkeletalMesh);
		break;

	case EItemSlot::Weapon:
	case EItemSlot::Shield:
		ServerSpawnEquippableActor(ItemData, OwningCharacter);
		break;

	default: break;;
	}
}

void UPlayerEquipmentComponent::DetachItemFromEquipment(const FItemData& ItemData)
{
	if (!IsValid(OwningCharacter)) return;

	OnItemDetach.Broadcast(ItemData);

	EItemSlot ItemSlot = ItemData.EquipmentSlot;

	switch (ItemSlot)
	{
	case EItemSlot::Head:
	case EItemSlot::Gloves:
	case EItemSlot::Neck:
	case EItemSlot::ChestPlate:
	case EItemSlot::Legs:
	case EItemSlot::Boots:
		OwningCharacter->ServerUpdateEquipmentMesh(ItemSlot, nullptr);
		break;

	case EItemSlot::Weapon:
	case EItemSlot::Shield:
		ServerDestroyEquippableActor(ItemData, OwningCharacter);
		break;

	default: break;;
	}
}

void UPlayerEquipmentComponent::AddItemToEquipmentArray(const FItemData& ItemData, EItemSlot EquipmentSlot)
{
	if (!GetOwner()->HasAuthority()) return;

	if (EquipmentData.Contains(EquipmentSlot))
	{
		EquipmentData[EquipmentSlot].EquipmentSlot = EquipmentSlot;
		EquipmentData[EquipmentSlot].bIsEquipped = true;
	}
	else
	{
		FItemData EquipItemData = ItemData;
		EquipItemData.EquipmentSlot = EquipmentSlot;
		EquipItemData.bIsEquipped = true;
		EquipmentData.Add(EquipmentSlot, EquipItemData);
	}

	if (UKismetSystemLibrary::IsStandalone(this))
	{
		OnAddedToEquipment.Broadcast(ItemData);
	}
	else
	{
		ClientUpdateAddItem(ItemData);
	}
}

void UPlayerEquipmentComponent::AddToStackInEquipment(const FItemData& ItemData, EItemSlot EquipmentSlot)
{
	if (!EquipmentData.Contains(EquipmentSlot)) return;

	EquipmentData.Find(EquipmentSlot)->Quantity = EquipmentData.Find(EquipmentSlot)->Quantity + ItemData.Quantity;

	AddItemToEquipmentArray(*EquipmentData.Find(EquipmentSlot), EquipmentData.Find(EquipmentSlot)->EquipmentSlot);
}

void UPlayerEquipmentComponent::RemoveItemQuantity(const FItemData& ItemData, int32 Quantity)
{
	if (ItemData.Quantity <= Quantity)
	{
		RemoveItemFromEquipmentArray(ItemData);
		DetachItemFromEquipment(ItemData);

		FItemData localItemData = ItemData;
		localItemData.Quantity = localItemData.Quantity - Quantity;
		AddItemToEquipmentArray(localItemData, localItemData.EquipmentSlot);
	}
	else
	{
		RemoveItemFromEquipmentArray(ItemData);
	}
}

void UPlayerEquipmentComponent::RemoveItemFromEquipmentArray(const FItemData& ItemData)
{
	if (!GetOwner()->HasAuthority()) return;

	EquipmentData.Remove(ItemData.EquipmentSlot);

	FItemData EmptyData;
	EmptyData.EquipmentSlot = ItemData.EquipmentSlot;
	EquipmentData.Add(ItemData.EquipmentSlot, EmptyData);

	if (UKismetSystemLibrary::IsStandalone(this))
	{
		OnRemovedFromEquipment.Broadcast(ItemData);
	}
	else
	{
		ClientUpdateRemovedItem(ItemData);
	}
}


bool UPlayerEquipmentComponent::TransferItemFromInventoryToEquipment(const FItemData& ItemData, const FItemData& InSlotData,
                                                                     UInventoryCore* Inventory, EInputMethod InputMethod,
                                                                     FText& OutFailureMessage)
{
	FItemData local_InSlotData = InSlotData;

	FText FailureMessage;
	if (!CanItemBeEquipped(ItemData, FailureMessage))
	{
		OutFailureMessage = FailureMessage;
		return false;
	}

	if (InputMethod == EInputMethod::RightClick)
	{
		local_InSlotData = AssignItemFromEquipmentSlot(ItemData);
	}

	if (local_InSlotData.IsItemClassValid())
	{
		if (UInventoryFunctionLibrary::AreItemTheSame(ItemData, local_InSlotData) &&
			UInventoryFunctionLibrary::AreItemsStackable(ItemData, local_InSlotData))
		{
			AddToStackInEquipment(ItemData, local_InSlotData.EquipmentSlot);
			Inventory->RemoveItemFromInventoryArray(ItemData);
			return true;
		}

		RemoveItemFromEquipmentArray(local_InSlotData);
		Inventory->AddItemToInventoryArray(local_InSlotData, ItemData.Index);
		AddItemToEquipmentArray(ItemData, local_InSlotData.EquipmentSlot);
		DetachItemFromEquipment(local_InSlotData);
		AttachItemToEquipment(ItemData);
		TryToUnEquipAssociatedSlot(ItemData, Inventory);
		return true;
	}


	Inventory->RemoveItemFromInventoryArray(ItemData);
	AddItemToEquipmentArray(ItemData, local_InSlotData.EquipmentSlot);
	AttachItemToEquipment(ItemData);
	TryToUnEquipAssociatedSlot(ItemData, Inventory);
	return true;
}

void UPlayerEquipmentComponent::TransferItemFromEquipmentToEquipment(const FItemData& DraggedData, const FItemData& InSlotData)
{
	if (InSlotData.IsItemClassValid())
	{
		RemoveItemFromEquipmentArray(InSlotData);
		AddItemToEquipmentArray(DraggedData, InSlotData.EquipmentSlot);
		RemoveItemFromEquipmentArray(DraggedData);
		AddItemToEquipmentArray(InSlotData, DraggedData.EquipmentSlot);
		AttachItemToEquipment(InSlotData);
		AttachItemToEquipment(DraggedData);
	}
	else
	{
		RemoveItemFromEquipmentArray(DraggedData);
		DetachItemFromEquipment(DraggedData);
		AddItemToEquipmentArray(DraggedData, InSlotData.EquipmentSlot);
		AttachItemToEquipment(DraggedData);
	}
}

bool UPlayerEquipmentComponent::TryToAddToPartialStack(const FItemData& ItemData)
{
	TArray<FItemData> EquipmentItemArray;
	EquipmentData.GenerateValueArray(EquipmentItemArray);

	int32 StackSlotIndex;
	if (UInventoryFunctionLibrary::HasPartialStack(EquipmentItemArray, ItemData, StackSlotIndex))
	{
		AddToStackInEquipment(ItemData, EquipmentItemArray[StackSlotIndex].EquipmentSlot);
		return true;
	}

	return false;
}

void UPlayerEquipmentComponent::TryToUnEquipAssociatedSlot(const FItemData& ItemData, UInventoryCore* Inventory)
{
	if (!bEnableOffHand) return;

	EItemSlot AssociatedSlot;
	if (ItemData.EquipmentSlot == EItemSlot::Weapon)
		AssociatedSlot = EItemSlot::Shield;
	else if (ItemData.EquipmentSlot == EItemSlot::Shield)
		AssociatedSlot = EItemSlot::Weapon;
	else
		AssociatedSlot = EItemSlot::None;

	UMainItem* ItemCDO = ItemData.ItemClass.GetDefaultObject();
	FItemData AssociatedItem;
	if (!GetItemByEquipmentSlot(AssociatedSlot, AssociatedItem)) return;

	if (ItemCDO->WeaponType == EWeaponType::OneHand || ItemCDO->WeaponType == EWeaponType::OffHand)
	{
		if (AssociatedItem.ItemClass.GetDefaultObject()->WeaponType == EWeaponType::TwoHand)
		{
			FText FailureMessage;
			bool bSuccess = Inventory->TransferItemFromEquipment(AssociatedItem, FItemData(), EInputMethod::RightClick,
			                                                     this, FailureMessage);
			ClientTransferItemReturnValue(bSuccess, FailureMessage.ToString());
		}
	}
	else if (ItemCDO->WeaponType == EWeaponType::TwoHand)
	{
		FText FailureMessage;
		bool bSuccess = Inventory->TransferItemFromEquipment(AssociatedItem, FItemData(), EInputMethod::RightClick,
		                                                     this, FailureMessage);
		ClientTransferItemReturnValue(bSuccess, FailureMessage.ToString());
	}
}

void UPlayerEquipmentComponent::SpawnAndAttachEquippableActor(const FItemData& ItemData, ARayCharacter* InOwningCharacter)
{
	if (!GetOwner()->HasAuthority()) return;

	if (!ItemData.IsItemClassValid()) return;

	if (!IsValid(InOwningCharacter)) return;

	TSubclassOf<ABaseEquippable> EquippableClass = ItemData.ItemClass.GetDefaultObject()->EquippableClass;

	if (ItemData.EquipmentSlot == EItemSlot::Weapon)
	{
		if (IsValid(EquippedWeaponSlot))
			EquippedWeaponSlot->Destroy();

		FTransform SpawnTransform = InOwningCharacter->GetActorTransform();
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
		ActorSpawnParameters.Owner = InOwningCharacter;
		EquippedWeaponSlot =
			Cast<ABaseEquippable>(GetWorld()->SpawnActor(EquippableClass, &SpawnTransform, ActorSpawnParameters));

		auto AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		FName WeaponSocketName = GetWeaponSocketName(ItemData.ItemClass.GetDefaultObject()->WeaponType);
		EquippedWeaponSlot->AttachToComponent(InOwningCharacter->GetMesh(), AttachmentTransformRules, WeaponSocketName);
	}
	else if (ItemData.EquipmentSlot == EItemSlot::Shield)
	{
		if (IsValid(EquippedShieldSlot))
			EquippedShieldSlot->Destroy();

		FTransform SpawnTransform = InOwningCharacter->GetActorTransform();
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
		ActorSpawnParameters.Owner = InOwningCharacter;
		EquippedShieldSlot =
			Cast<ABaseEquippable>(GetWorld()->SpawnActor(EquippableClass, &SpawnTransform, ActorSpawnParameters));

		auto AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		FName WeaponSocketName = GetShieldSocketName(ItemData.ItemClass.GetDefaultObject()->WeaponType);
		EquippedShieldSlot->AttachToComponent(InOwningCharacter->GetMesh(), AttachmentTransformRules, WeaponSocketName);
	}
}

void UPlayerEquipmentComponent::DestroyEquippableActor(const FItemData& SlotItemData)
{
	if (!GetOwner()->HasAuthority()) return;

	ABaseEquippable* EquippedActor = GetEquippedActorReference(SlotItemData.EquipmentSlot);
	if (!IsValid(EquippedActor)) return;

	EquippedActor->Destroy();
}

bool UPlayerEquipmentComponent::CanItemBeEquipped(const FItemData& ItemData, FText& FailureMessage)
{
	if (ULevelingComponent* OwnerLevelingComponent = Cast<ULevelingComponent>(
		GetOwner()->GetComponentByClass(ULevelingComponent::StaticClass())))
	{
		UMainItem* ItemCDO = ItemData.ItemClass.GetDefaultObject();
		const bool bRequiredLevel = bCheckRequiredItemLevel
			                            ? OwnerLevelingComponent->CurrentLevel >= ItemCDO->RequiredLevel
			                            : true;

		if (bRequiredLevel)
		{
			return true;
		}
		else
		{
			FailureMessage = FText::FromString(TEXT("더 높은 레벨이 필요합니다."));
			return false;
		}
	}


	return false;
}

bool UPlayerEquipmentComponent::GetItemByEquipmentSlot(EItemSlot ItemSlot, FItemData& OutItemData)
{
	if (EquipmentData.Contains(ItemSlot))
	{
		OutItemData = *EquipmentData.Find(ItemSlot);
		return EquipmentData.Find(ItemSlot)->IsItemClassValid();
	}

	OutItemData = FItemData();
	return false;
}

bool UPlayerEquipmentComponent::GetItemByID(FString ItemID, FItemData& OutItemData)
{
	TArray<FItemData> EquipmentItemArray;
	EquipmentData.GenerateValueArray(EquipmentItemArray);
	return UInventoryFunctionLibrary::FindItemByID(EquipmentItemArray, ItemID, OutItemData);
}

TArray<FItemData> UPlayerEquipmentComponent::GetEquipmentItems()
{
	TArray<FItemData> EquipmentItemArray;
	EquipmentData.GenerateValueArray(EquipmentItemArray);
	return EquipmentItemArray;
}

ABaseEquippable* UPlayerEquipmentComponent::GetEquippedActorReference(EItemSlot ItemSlot)
{
	if (ItemSlot == EItemSlot::Weapon)
	{
		if (IsValid(EquippedWeaponSlot))
			return EquippedWeaponSlot;
	}

	if (ItemSlot == EItemSlot::Shield)
	{
		if (IsValid(EquippedShieldSlot))
			return EquippedShieldSlot;
	}

	return nullptr;
}

FName UPlayerEquipmentComponent::GetWeaponSocketName(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::None:
	case EWeaponType::OneHand:
	case EWeaponType::OffHand:
		return FName(TEXT("main_hand"));

	case EWeaponType::TwoHand:
		return FName(TEXT("two_hand"));
	}

	return {};
}

FName UPlayerEquipmentComponent::GetShieldSocketName(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::None:
	case EWeaponType::OffHand:
		return FName(TEXT("shield"));

	case EWeaponType::OneHand:
		return FName(TEXT("off_hand"));

	case EWeaponType::TwoHand:
		return FName(TEXT("two_hand"));
	}

	return {};
}

void UPlayerEquipmentComponent::ServerTransferItemFromInventory_Implementation(const FItemData& ItemData,
                                                                               FItemData InSlotData,
                                                                               UInventoryCore* Inventory,
                                                                               EInputMethod InputMethod)
{
	FText FailureMessage;
	bool bSuccess = TransferItemFromInventoryToEquipment(ItemData, InSlotData, Inventory, InputMethod, FailureMessage);
	ClientTransferItemReturnValue(bSuccess, FailureMessage.ToString());
}

void UPlayerEquipmentComponent::ServerTransferItemFromEquipment_Implementation(const FItemData& DraggedData,
                                                                               FItemData InSlotData)
{
	TransferItemFromEquipmentToEquipment(DraggedData, InSlotData);
}

void UPlayerEquipmentComponent::ServerSpawnEquippableActor_Implementation(const FItemData& ItemData,
                                                                          ARayCharacter* InOwningCharacter)
{
	SpawnAndAttachEquippableActor(ItemData, OwningCharacter);
}

void UPlayerEquipmentComponent::ServerDestroyEquippableActor_Implementation(const FItemData& ItemData,
                                                                            ARayCharacter* InOwningCharacter)
{
	DestroyEquippableActor(ItemData);
}

void UPlayerEquipmentComponent::ServerRemoveItemQuantity_Implementation(const FItemData& ItemData, int Quantiy)
{
	RemoveItemQuantity(ItemData, Quantiy);
}

void UPlayerEquipmentComponent::ClientInitializeEquipment_Implementation()
{
	if (!UKismetSystemLibrary::IsStandalone(this))
	{
		BuildEquipment();
	}
}

void UPlayerEquipmentComponent::ClientTransferItemReturnValue_Implementation(bool bSuccess, const FString& FailureMessage)
{
	if (!bSuccess)
	{
		UWidgetSystemLibrary::GetInventoryWidgetController(this)->DisplayMessageNotify(FailureMessage);
	}
}

void UPlayerEquipmentComponent::UpdateAddedItem(const FItemData& ItemData)
{
	EquipmentData.Add(ItemData.EquipmentSlot, ItemData);
	OnAddedToEquipment.Broadcast(ItemData);
	OnItemAttach.Broadcast(ItemData);
}

void UPlayerEquipmentComponent::UpdateRemovedItem(const FItemData& ItemData)
{
	EquipmentData.Remove(ItemData.EquipmentSlot);
	EquipmentData.Add(ItemData.EquipmentSlot, FItemData(ItemData.EquipmentSlot));
	OnRemovedFromEquipment.Broadcast(ItemData);
	OnItemDetach.Broadcast(ItemData);
}

void UPlayerEquipmentComponent::ClientUpdateRemovedItem_Implementation(const FItemData& ItemData)
{
}

void UPlayerEquipmentComponent::ClientUpdateAddItem_Implementation(const FItemData& ItemData)
{
}
