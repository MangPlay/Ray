// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractionComponent.generated.h"

class UInteractableComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RAY_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void InitializeInteraction();

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Interaction Component")
	TEnumAsByte<EDrawDebugTrace::Type> DebugTrace;

	UPROPERTY(BlueprintReadWrite,Category="Interaction Component")
	UInteractableComponent* CurrentInteractable;
};
