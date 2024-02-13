#include "Player/RayPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InteractionSystem/Components/InteractionManagerComponent.h"
#include "InventorySystem/Actors/Capture/PlayerCapture.h"
#include "InventorySystem/Components/ConsumableBuffsComponent.h"
#include "InventorySystem/Components/InteractableComponent.h"
#include "InventorySystem/Components/InteractionComponent.h"
#include "InventorySystem/Components/LevelingComponent.h"
#include "InventorySystem/Components/PlayerEquipmentComponent.h"
#include "InventorySystem/Components/PlayerInventoryComponent.h"
#include "InventorySystem/Components/StatsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RayPlayerState.h"
#include "UI/WidgetSystemLibrary.h"
#include "UI/HUD/RayHUD.h"
#include "UI/WidgetController/InventoryWidgetController.h"

ARayPlayerController::ARayPlayerController()
{
	InteractionManager = CreateDefaultSubobject<UInteractionManagerComponent>(TEXT("InteractionManager"));

	PlayerInventory = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("PlayerInventory"));

	StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));

	LevelingComponent = CreateDefaultSubobject<ULevelingComponent>(TEXT("LevelingComponent"));

	PlayerEquipment = CreateDefaultSubobject<UPlayerEquipmentComponent>(TEXT("PlayerEquipment"));

	ConsumableBuffsComponent = CreateDefaultSubobject<UConsumableBuffsComponent>(TEXT("ConsumableBuffs"));

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void ARayPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ARayPlayerState* RAIPlayerState = GetPlayerState<ARayPlayerState>();
	check(RAIPlayerState);

	// RAIPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(RAIPlayerState, this);
	// Cast<UAuraAbilitySystemComponent>(RAIPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	// AbilitySystemComponent = RAIPlayerState->GetAbilitySystemComponent();
	// AttributeSet = RAIPlayerState->GetAttributeSet();

	if (ARayHUD* RayHUD = Cast<ARayHUD>(GetHUD()))
	{
		RayHUD->InitOverlay(this, RAIPlayerState);
	}
	// InitializeDefaultAttributes();


	InitializeComponentsEvent();
}

void ARayPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ARayPlayerController::InitializeComponentsEvent()
{
	StatsComponent->InitializeStats();

	PlayerInventory->InitializeInventory();

	PlayerEquipment->InitializeEquipment();

	InteractionComponent->InitializeInteraction();
}
