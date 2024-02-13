// RAI Project 개발자 : Nam Chan woo


#include "InventorySystem/Actors/Capture/PlayerCapture.h"
#include "InventorySystem/Actors/Equippable/BaseEquippable.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WindDirectionalSourceComponent.h"
#include "Engine/WindDirectionalSource.h"
#include "InventorySystem/Components/PlayerEquipmentComponent.h"


#include "InventorySystem/Objects/MainItem.h"


// Sets default values
APlayerCapture::APlayerCapture()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bNetLoadOnClient = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Scene);

	Boots = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Boots"));
	Boots->SetupAttachment(SkeletalMesh);

	Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	Legs->SetupAttachment(SkeletalMesh);

	Gloves = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gloves"));
	Gloves->SetupAttachment(SkeletalMesh);

	Chest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));
	Chest->SetupAttachment(SkeletalMesh);

	Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(SkeletalMesh);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(Scene);

	WidPosition = CreateDefaultSubobject<UTextRenderComponent>(TEXT("WidPosition"));
	WidPosition->SetupAttachment(Scene);

	WindDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("WindDirection"));
	WindDirection->SetupAttachment(WidPosition);

	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureComponent2D->SetupAttachment(Scene);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(SceneCaptureComponent2D);
}

void APlayerCapture::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SceneCaptureComponent2D->ShowOnlyActorComponents(this, true);
}

void APlayerCapture::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCapture::InitializePlayerCapture(UPlayerEquipmentComponent* InPlayerEquipment)
{
	if (IsValid(InPlayerEquipment))
	{
		PlayerEquipment = InPlayerEquipment;
		PlayerEquipment->OnItemAttach.AddDynamic(this, &APlayerCapture::AttachItem);
		PlayerEquipment->OnItemDetach.AddDynamic(this, &APlayerCapture::DetachItem);
	}
}

void APlayerCapture::SpawnWindComponent(float Strength, float Speed)
{
	if (GetWorld())
	{
		FTransform SpawnTransform = WidPosition->GetRelativeTransform();
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
		Wind = Cast<AWindDirectionalSource>(
			GetWorld()->SpawnActor(AWindDirectionalSource::StaticClass(), &SpawnTransform, ActorSpawnParameters));

		Wind->GetComponent()->SetStrength(Strength);
		Wind->GetComponent()->SetSpeed(Speed);
	}
}

void APlayerCapture::AttachItem(const FItemData& ItemData)
{
	if (!ItemData.IsItemClassValid()) return;

	const EItemSlot EquipmentSlot = ItemData.EquipmentSlot;

	UMainItem* MainItemCDO = ItemData.ItemClass.GetDefaultObject();

	EWeaponType WeaponType = MainItemCDO->WeaponType;
	TSubclassOf<ABaseEquippable> EquipItemClass = MainItemCDO->EquippableClass;
	USkeletalMesh* EquipSkeletalMesh = MainItemCDO->SkeletalMesh;

	FTransform SpawnTransform = GetActorTransform();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
	SpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
	auto AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);

	switch (EquipmentSlot)
	{
	case EItemSlot::Head:
		if (IsValid(Head))
		{
			Head->SetSkinnedAssetAndUpdate(EquipSkeletalMesh);
			Head->SetLeaderPoseComponent(SkeletalMesh, true);
		}
		break;

	case EItemSlot::Gloves:
		if (IsValid(Gloves))
		{
			Gloves->SetSkinnedAssetAndUpdate(EquipSkeletalMesh);
			Gloves->SetLeaderPoseComponent(SkeletalMesh, true);
		}
		break;

	case EItemSlot::ChestPlate:
		if (IsValid(Chest))
		{
			Chest->SetSkinnedAssetAndUpdate(EquipSkeletalMesh);
			Chest->SetLeaderPoseComponent(SkeletalMesh, true);
		}
		break;

	case EItemSlot::Legs:
		if (IsValid(Legs))
		{
			Legs->SetSkinnedAssetAndUpdate(EquipSkeletalMesh);
			Legs->SetLeaderPoseComponent(SkeletalMesh, true);
		}
		break;

	case EItemSlot::Boots:
		if (IsValid(Boots))
		{
			Boots->SetSkinnedAssetAndUpdate(EquipSkeletalMesh);
			Boots->SetLeaderPoseComponent(SkeletalMesh, true);
		}
		break;

	case EItemSlot::Weapon:
		if (IsValid(EquippedWeaponSlot))
		{
			EquippedWeaponSlot->Destroy();
		}
		EquippedWeaponSlot = Cast<ABaseEquippable>(
			GetWorld()->SpawnActor(EquipItemClass, &SpawnTransform, SpawnParameters));
		SceneCaptureComponent2D->ShowOnlyActorComponents(EquippedWeaponSlot, true);
		EquippedWeaponSlot->AttachToComponent(SkeletalMesh, AttachmentTransformRules, GetSwordSocketName(WeaponType));
		break;

	case EItemSlot::Shield:
		if (IsValid(EquippedShieldSlot))
		{
			EquippedShieldSlot->Destroy();
		}
		EquippedShieldSlot = Cast<ABaseEquippable>(GetWorld()->SpawnActor(EquipItemClass, &SpawnTransform, SpawnParameters));
		SceneCaptureComponent2D->ShowOnlyActorComponents(EquippedShieldSlot, true);
		EquippedShieldSlot->AttachToComponent(SkeletalMesh, AttachmentTransformRules, GetShieldSocketName(WeaponType));
		break;

	default: break;
	}
}

void APlayerCapture::DetachItem(const FItemData& ItemData)
{
	EItemSlot ItemSlot = ItemData.EquipmentSlot;

	switch (ItemSlot)
	{
	case EItemSlot::Head:
		Head->SetSkinnedAssetAndUpdate(nullptr);
		break;

	case EItemSlot::Gloves:
		Gloves->SetSkinnedAssetAndUpdate(nullptr);
		break;

	case EItemSlot::ChestPlate:
		Chest->SetSkinnedAssetAndUpdate(nullptr);
		break;

	case EItemSlot::Legs:
		Legs->SetSkinnedAssetAndUpdate(nullptr);
		break;

	case EItemSlot::Boots:
		Boots->SetSkinnedAssetAndUpdate(nullptr);
		break;

	case EItemSlot::Weapon:
		if (IsValid(EquippedWeaponSlot))
			EquippedWeaponSlot->Destroy();
		break;

	case EItemSlot::Shield:
		if (IsValid(EquippedShieldSlot))
			EquippedShieldSlot->Destroy();
		break;

	default: break;
	}
}

FName APlayerCapture::GetSwordSocketName(EWeaponType WeaponType)
{
	if (WeaponType == EWeaponType::OneHand)
		return FName(TEXT("main_hand"));
	if (WeaponType == EWeaponType::TwoHand)
		return FName(TEXT("two_hand"));
	if (WeaponType == EWeaponType::OffHand)
		return FName(TEXT("main_hand"));

	return FName(TEXT("main_hand"));
}

FName APlayerCapture::GetShieldSocketName(EWeaponType WeaponType)
{
	if (WeaponType == EWeaponType::OneHand)
		return FName(TEXT("off_hand"));
	if (WeaponType == EWeaponType::TwoHand)
		return FName(TEXT("two_hand"));
	if (WeaponType == EWeaponType::OffHand)
		return FName(TEXT("shield"));

	return FName(TEXT("shield"));
}

void APlayerCapture::EventStartCapture_Implementation()
{
	SceneCaptureComponent2D->bCaptureEveryFrame = true;
	SpawnWindComponent(1.0f, 0.5f);
}

void APlayerCapture::EventEndCapture_Implementation()
{
	SceneCaptureComponent2D->bCaptureEveryFrame = false;
	if (IsValid(Wind))
	{
		Wind->Destroy();
	}
	const FRotator SkeletalRelativeRotation = SkeletalMesh->GetRelativeRotation();
	const FRotator NewRelativeRotation = FRotator(SkeletalRelativeRotation.Pitch, 0, SkeletalRelativeRotation.Roll);
	SkeletalMesh->SetRelativeRotation(NewRelativeRotation);
}
