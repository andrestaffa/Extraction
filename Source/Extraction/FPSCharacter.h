// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

UCLASS()
class EXTRACTION_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:

	// Components
	class UCapsuleComponent* capsuleComp;

	// Weapon
	UPROPERTY(BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class AWeapon* equippedWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> weaponClass;

private:
	void SpawnDefaultWeapon();

















// MARK: - [START] Movement Variables/Functions
private:

	// Mouse Movement
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float moveForwardValue;
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float moveRightValue;
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float turnValue;
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float lookValue;

	// Crouching/Proning
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isCrouching;
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isProning;

	// Jumping/Vaulting/Sliding/Climbing
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isVaulting;
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isClimbing;
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isSliding;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement",  meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* vaultMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement",  meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* slideMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement",  meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* climbMontage;

	// Sprinting
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isSprinting;
	bool runButtonPressed;
	
	// Reloading
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isReloading;
	FTimerHandle reloadTimerHandle;

	// (Aiming) SET IN BLUEPRINTS
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool ADSEnabled;

protected:

	// Movement Update Functions
	void MovementUpdate();
	void CancelReloadUpdate();

	// Camera Shake
	void HandleCameraShake();

	// Mouse Inputs
	void MoveForward(float axisValue);
	void MoveRight(float axisValue);
	void Turn(float axisValue);
	void LookUp(float axisValue); 

	// Crouching/Proning
	void CrouchButtonPressed();
	void ToggleCrouch(bool toggle);
	void ProneButtonPressed();

	// Jumping/Vaulting/Sliding/Climbing
	void JumpButtonPressed();
	void SetupParkour();
	void Vault();
	void Climb();
	void Slide();
	void PlayMantleAnimation(class UAnimMontage* montageAnim, float animTime, bool& inAnimBool);

	// Sprinting
	void SprintButtonPressed();
	void SprintButtonReleased();
	void ToggleSprint(bool toggle);

	// Reloading
	void ReloadButtonPressed();

// MARK: - [END] Movement Variables/Functions

};
