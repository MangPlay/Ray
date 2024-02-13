#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionSystem/Interfaces/Interface_Interactable.h"
#include "BaseInteractable.generated.h"

class UInteractionTargetComponent;

UCLASS()
class RAY_API ABaseInteractable : public AActor, public IInterface_Interactable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*IInterface_Interactable*/
public:
	virtual void InitializeInteractable_Implementation() override;
	virtual void SetupOverlapAttachComponent_Implementation(USceneComponent* NewAttachComponent) override;
	virtual void SetupMarkerTargetComponent_Implementation(USceneComponent* NewMarkerTarget) override;
	virtual void SetupHighlightedComponents_Implementation(const TArray<UPrimitiveComponent*>& NewHighlightComponents) override;
	/*IInterface_Interactable*/


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base Interactable|Components")
	USceneComponent* Scene;

	UPROPERTY(BlueprintReadWrite, Category="Base Interactable|Reference")
	APawn* Interactor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base Interactable|Reference")
	TObjectPtr<UInteractionTargetComponent> InteractionTargetComponent;
};
