#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEquippable.generated.h"

UCLASS()
class RAY_API ABaseEquippable : public AActor
{
	GENERATED_BODY()

public:
	ABaseEquippable();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base Equippable|Component")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base Equippable|Component")
	TObjectPtr<UParticleSystemComponent> ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base Equippable|Component")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base Equippable|Component")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
};
