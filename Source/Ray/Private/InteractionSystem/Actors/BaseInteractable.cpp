#include "InteractionSystem/Actors/BaseInteractable.h"

#include "InteractionSystem/Components/InteractionTargetComponent.h"


// Sets default values
ABaseInteractable::ABaseInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	InteractionTargetComponent = CreateDefaultSubobject<UInteractionTargetComponent>(TEXT("InteractionTargetComponent"));
}

// Called when the game starts or when spawned
void ABaseInteractable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseInteractable::InitializeInteractable_Implementation()
{
}

void ABaseInteractable::SetupOverlapAttachComponent_Implementation(USceneComponent* NewAttachComponent)
{
	if (NewAttachComponent)
		InteractionTargetComponent->ComponentToAttach = NewAttachComponent;
	else
		InteractionTargetComponent->ComponentToAttach = RootComponent;
}

void ABaseInteractable::SetupMarkerTargetComponent_Implementation(USceneComponent* NewMarkerTarget)
{
	if (NewMarkerTarget)
		InteractionTargetComponent->MarkerTargetComponent = NewMarkerTarget;
	else
		InteractionTargetComponent->MarkerTargetComponent = RootComponent;
}

void ABaseInteractable::SetupHighlightedComponents_Implementation(const TArray<UPrimitiveComponent*>& NewHighlightComponents)
{
	InteractionTargetComponent->HighlightedComponents = NewHighlightComponents;
}
