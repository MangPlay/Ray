
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"


class UBoxComponent;
class UWidgetComponent;

UCLASS(ClassGroup=("Ray|Interaction"), meta=(BlueprintSpawnableComponent))
class RAY_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	/*Before Interaction*/
public:
	void EventInitialize();

	void EventToggleHighlight(bool bHighlight, AActor* InInteractor);

	void SetupInteractableReferences(UBoxComponent* InInteractableArea, UWidgetComponent* InInteractionWidget, TSet<UPrimitiveComponent*> InHighlightableObjects);

	void EventDurationPress();

	UFUNCTION()
	void IsKeyDown();

	/*OnInteraction*/
public:
	UFUNCTION(Category="Interactable Component Event|OnInteraction")
	void EventAssociatedActorInteraction(AActor* InInteractor);

	UFUNCTION(Category="Interactable Component Event|OnInteraction")
	void EventInteraction(AActor* InInteractor);

	UFUNCTION(Category="Interactable Component Event|OnInteraction")
	void EventCheckForInteractionWithAssociate(AActor* InInteractor);


	void EventEndInteraction(APlayerController* Controller);

	void RemoveInteractionByResponse();


	UFUNCTION(NetMulticast, Reliable, Category="Interactable Component|After Interaction")
	void EventRemoveInteraction();

	void ToggleCanBeReInitialized(bool bCondition);


	/*Utility*/
public:
	bool IsTargetInteractableValue();


	bool HoldingInput();

	bool GetPressedKeyByActionName(FName InActionName, FKey& OutKey);


	UInteractableComponent* GetInteractableComponent(AActor* Target);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Component|Main")
	bool bIsInteractable = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable Component|Main")
	UBoxComponent* InteractableArea;


	UPROPERTY(BlueprintReadWrite, Category="Interactable Component|Reference")
	TObjectPtr<UWidgetComponent> InteractionWidget;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Component|Interaction")
	bool bCheckForAssociatedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Component|Interaction")
	bool bRemoveAssociatedInteractablesOnComplete;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Component|Interaction")
	bool bDestroyAfterPickup;

	UPROPERTY(BlueprintReadWrite, Category="Interactable Component|Interaction")
	bool bAlreadyInteracted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Component|Interaction")
	int32 InteractableValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Component|Interaction")
	float MaxKeyTimeDown = 1.0f;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Component|Interaction")
	// EInteractionResponse InteractableResponse = EInteractionResponse;

	UPROPERTY(BlueprintReadWrite, Category="Interactable Component|Interaction")
	FName DestroyableTag;

	UPROPERTY(BlueprintReadWrite, Category="Interactable Component|Interaction")
	FName InteratableTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Component|Interaction")
	FKey PressedInteractionKey;

	UPROPERTY(BlueprintReadWrite, Category="Interactable Component|Interaction")
	FTimerHandle KeyDownTimer;

	UPROPERTY(BlueprintReadWrite, Category="Interactable Component|Interaction")
	TObjectPtr<AActor> Interactor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable Component|Main")
	TMap<AActor*, int32> AssociatedInteractableActors;

	UPROPERTY(BlueprintReadOnly, Category="Interactable Component|Interaction")
	TArray<UPrimitiveComponent*> ObjectsToHighlight;
};
