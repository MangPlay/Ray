// RAI Project 개발자 : Nam Chan woo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "PlayerCapture.generated.h"

class ABaseEquippable;
class UPlayerEquipmentComponent;
class AWindDirectionalSource;
class UArrowComponent;
class UTextRenderComponent;
class USpotLightComponent;

UCLASS()
class RAY_API APlayerCapture : public AActor
{
	GENERATED_BODY()

public:
	APlayerCapture();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category="Player Capture Event")
	void InitializePlayerCapture(UPlayerEquipmentComponent* InPlayerEquipment);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category="Player Capture Event")
	void EventStartCapture();

	UFUNCTION(BlueprintCallable, Client, Reliable, Category="Player Capture Event")
	void EventEndCapture();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category="Player Capture Event")
	void SpawnWindComponent(float Strength, float Speed);

protected:
	UFUNCTION()
	void AttachItem(const FItemData& ItemData);

	UFUNCTION()
	void DetachItem(const FItemData& ItemData);

	UFUNCTION()
	FName GetSwordSocketName(EWeaponType WeaponType);

	UFUNCTION()
	FName GetShieldSocketName(EWeaponType WeaponType);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<USkeletalMeshComponent> Boots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<USkeletalMeshComponent> Legs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<USkeletalMeshComponent> Gloves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<USkeletalMeshComponent> Chest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<USkeletalMeshComponent> Head;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<USpotLightComponent> SpotLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<UTextRenderComponent> WidPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<UArrowComponent> WindDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent2D;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Player Capture|Components")
	TObjectPtr<UArrowComponent> Arrow;

	UPROPERTY(BlueprintReadWrite, Category="Player Capture|Reference")
	UPlayerEquipmentComponent* PlayerEquipment;

	UPROPERTY(BlueprintReadWrite, Category="Player Capture|Reference")
	AWindDirectionalSource* Wind;

	UPROPERTY(BlueprintReadWrite, Category="Player Capture|Reference")
	TObjectPtr<ABaseEquippable> EquippedWeaponSlot;

	UPROPERTY(BlueprintReadWrite, Category="Player Capture|Reference")
	TObjectPtr<ABaseEquippable> EquippedShieldSlot;
};
