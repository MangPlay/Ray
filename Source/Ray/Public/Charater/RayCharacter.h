// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventorySystem/Libraries/InventoryEnumStruct.h"
#include "Logging/LogMacros.h"
#include "RayCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ARayCharacter : public ACharacter
{
	GENERATED_BODY()

	

public:
	ARayCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();


public:
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="Ray Character|NetWork Replication Events")
	void ServerUpdateEquipmentMesh(EItemSlot ItemSlot, USkeletalMesh* InSkeletaMesh);

public:
	// /** Returns CameraBoom subobject **/
	// FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// /** Returns FollowCamera subobject **/
	// FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


public:
	// /** Camera boom positioning the camera behind the character */
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	// USpringArmComponent* CameraBoom;
	//
	// /** Follow camera */
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	// UCameraComponent* FollowCamera;
	//
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	//
	// /** Jump Input Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// UInputAction* JumpAction;
	//
	// /** Move Input Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// UInputAction* MoveAction;
	//
	// /** Look Input Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// UInputAction* LookAction;
};

