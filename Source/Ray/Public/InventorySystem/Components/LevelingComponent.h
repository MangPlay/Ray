#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelingComponent.generated.h"


USTRUCT(BlueprintType)
struct FLevelingSystem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leveling System")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leveling System")
	float ExpNeeded;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGainedExperience, float, ExpGained);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurrentExpChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);

UCLASS(ClassGroup=("Ray|Inventory"), meta=(BlueprintSpawnableComponent))
class RAY_API ULevelingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULevelingComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="Leveling Component Event")
	void EventInitialize();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category="Leveling Component Event")
	void SetUpLevelingData();
	
	UFUNCTION(BlueprintCallable, Category="Leveling Component Event")
	void AddLevelExp(float ExpToAdd);

	UFUNCTION(BlueprintCallable, Category="Leveling Component Event")
	void IncreaseCurrentLevel();

	UFUNCTION(BlueprintCallable, Category="Leveling Component Event")
	void SetCurrentLevel(int32 NewLevel);

	
	UFUNCTION(BlueprintPure, Category="Leveling Component Event")
	bool IsNextLevelIsValid();
	
	UFUNCTION(BlueprintPure, Category="Leveling Component Event")
	bool ExceedLevelMaxExp();

	UFUNCTION(BlueprintPure, Category="Leveling Component Event")
	int32 GetCurrentLevel();

public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Leveling Component Event|Network Replication")
	void ServerIncreaseCurrentLevel();
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Leveling Component Event|Network Replication")
	void ServerAddLevelExp(float ExpToAdd);
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Leveling Component Event|Network Replication")
	void ServerSetCurrentLevel(int32 NewLevel);
	
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Leveling Component Event|Network Replication")
	void ClientGainedExp(float ExpGained);
	
	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Leveling Component Event|Network Replication")
	void ClientLevelUp(int32 NewLevel);
	
	UFUNCTION()
	void OnRep_CurrentLevel();
	
	UFUNCTION()
	void OnRep_CurrentExperience();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leveling Component",ReplicatedUsing = OnRep_CurrentLevel)
	int32 CurrentLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leveling Component",ReplicatedUsing = OnRep_CurrentExperience)
	float CurrentExperience = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leveling Component")
	TObjectPtr<UDataTable> LevelingDataTable;

	UPROPERTY(BlueprintReadWrite, Category="Leveling Component")
	float LevelMaxExperience = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category="Leveling Component")
	TMap<int32, float> LevelingData;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Leveling Component|Delegate")
	FOnGainedExperience OnGainedExperience;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Leveling Component|Delegate")
	FOnLevelChanged OnLevelChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Leveling Component|Delegate")
	FOnCurrentExpChanged OnCurrentExpChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Leveling Component|Delegate")
	FOnLevelUp OnLevelUp;
};
