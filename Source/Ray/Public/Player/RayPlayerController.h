// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "RayPlayerController.generated.h"

class UInteractionManagerComponent;
class ULevelingComponent;
class UStatsComponent;
class UPlayerEquipmentComponent;
class UConsumableBuffsComponent;
class UPlayerInventoryComponent;

//
class UInteractionComponent;


UCLASS()
class RAY_API ARayPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARayPlayerController();

	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="Ray Player Controller Event")
	void InitializeComponentsEvent();


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray PlayerController|SetUp")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray PlayerController|Components")
	TObjectPtr<UInteractionManagerComponent> InteractionManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray PlayerController|Components")
	TObjectPtr<UPlayerInventoryComponent> PlayerInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray PlayerController|Components")
	TObjectPtr<UStatsComponent> StatsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray PlayerController|Components")
	TObjectPtr<ULevelingComponent> LevelingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray PlayerController|Components")
	TObjectPtr<UPlayerEquipmentComponent> PlayerEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray PlayerController|Components")
	TObjectPtr<UConsumableBuffsComponent> ConsumableBuffsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ray PlayerController|Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;


	UPROPERTY(BlueprintReadWrite, Category="Ray PlayerController|Gamepad")
	bool bGamepadControls = false;
};
