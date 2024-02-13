#include "InventorySystem/Actors/Equippable/BaseEquippable.h"

#include "Particles/ParticleSystemComponent.h"


// Sets default values
ABaseEquippable::ABaseEquippable()
{
	// 매 프레임마다 Tick()을 호출하도록 이 액터를 설정합니다. 필요하지 않은 경우 이 기능을 꺼서 성능을 향상시킬 수 있습니다.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);
	
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(DefaultSceneRoot);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(DefaultSceneRoot);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(DefaultSceneRoot);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ABaseEquippable::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseEquippable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
