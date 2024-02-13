#include "InventorySystem/Components/LevelingComponent.h"

#include "Net/UnrealNetwork.h"
#include "Utilities/RayLog.h"


// Sets default values for this component's properties
ULevelingComponent::ULevelingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void ULevelingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 변수 리플리케이션 설정
	DOREPLIFETIME(ULevelingComponent, CurrentLevel);
	DOREPLIFETIME(ULevelingComponent, CurrentExperience);
}

void ULevelingComponent::EventInitialize()
{
	RAY_IF_CHECK(LevelingDataTable,TEXT("LevelingDataTable 설정하세요"));
	
	SetUpLevelingData();
	
	if (LevelingData.Contains(CurrentLevel))
	{
		LevelMaxExperience = *LevelingData.Find(CurrentLevel);
	}
	else
	{
		RAY_LOG_SCREEN_ERROR(TEXT("현재 레벨이 유효하지 않습니다!"));
	}
}

void ULevelingComponent::BeginPlay()
{
	Super::BeginPlay();

	RAY_IF_CHECK(LevelingDataTable,TEXT("LevelingDataTable 설정하세요"));
	
	SetUpLevelingData();
	
	if (LevelingData.Contains(CurrentLevel))
	{
		LevelMaxExperience = *LevelingData.Find(CurrentLevel);
	}
	else
	{
		RAY_LOG_SCREEN_ERROR(TEXT("현재 레벨이 유효하지 않습니다!"));
	}
}

void ULevelingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetIsReplicatedByDefault(true);

	// ...
}

void ULevelingComponent::SetUpLevelingData()
{
	TArray<FName> LevelingRowNames = LevelingDataTable->GetRowNames();

	for (FName RowName : LevelingRowNames)
	{
		FLevelingSystem* LevelingSystemData = LevelingDataTable->FindRow<FLevelingSystem>(RowName,TEXT(""));
		if (LevelingSystemData)
		{
			LevelingData.Add(LevelingSystemData->Level, LevelingSystemData->ExpNeeded);
		}
	}
}

void ULevelingComponent::AddLevelExp(float ExpToAdd)
{
	if (!GetOwner()->HasAuthority()) return;

	if (ExpToAdd <= 0.0f) return;

	CurrentExperience = CurrentExperience + ExpToAdd;

	ClientGainedExp(ExpToAdd);

	if (ExceedLevelMaxExp())
	{
		IncreaseCurrentLevel();
	}
}

void ULevelingComponent::IncreaseCurrentLevel()
{
	if (!GetOwner()->HasAuthority()) return;

	if (!ExceedLevelMaxExp())return;

	if (IsNextLevelIsValid())
	{
		CurrentExperience = CurrentExperience - LevelMaxExperience;
		CurrentLevel = CurrentLevel + 1;
		ClientLevelUp(CurrentLevel);

		if (ExceedLevelMaxExp())
			IncreaseCurrentLevel();
	}
	else
	{
		CurrentExperience = FMath::Clamp(CurrentExperience, 0.0f, LevelMaxExperience);
	}
}

void ULevelingComponent::SetCurrentLevel(int32 NewLevel)
{
	if (!GetOwner()->HasAuthority()) return;

	if (LevelingData.Contains(NewLevel))
	{
		CurrentLevel = NewLevel;
		CurrentExperience = 0.0f;
	}
}

bool ULevelingComponent::IsNextLevelIsValid()
{
	return LevelingData.Contains(CurrentLevel + 1);
}

bool ULevelingComponent::ExceedLevelMaxExp()
{
	return CurrentExperience >= LevelMaxExperience;
}

int32 ULevelingComponent::GetCurrentLevel()
{
	return CurrentLevel;
}

void ULevelingComponent::ServerIncreaseCurrentLevel_Implementation()
{
	IncreaseCurrentLevel();
}

void ULevelingComponent::ServerAddLevelExp_Implementation(float ExpToAdd)
{
	AddLevelExp(ExpToAdd);
}

void ULevelingComponent::ServerSetCurrentLevel_Implementation(int32 NewLevel)
{
	SetCurrentLevel(NewLevel);
}

void ULevelingComponent::ClientGainedExp_Implementation(float ExpGained)
{
	OnGainedExperience.Broadcast(ExpGained);
}

void ULevelingComponent::ClientLevelUp_Implementation(int32 NewLevel)
{
	OnLevelUp.Broadcast(NewLevel);
}

void ULevelingComponent::OnRep_CurrentLevel()
{
	if (LevelingData.Contains(CurrentLevel))
	{
		LevelMaxExperience = *LevelingData.Find(CurrentLevel);
		OnLevelChanged.Broadcast();
	}
}

void ULevelingComponent::OnRep_CurrentExperience()
{
	OnCurrentExpChanged.Broadcast();
}
