// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

USTRUCT(BlueprintType)
struct FSensitivitySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = "1.0", UIMax = "100.0"))
	float mouseSensitivity = 35.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = "0.1", UIMax = "1.0"))
	float mouseADSSensitivityMultiplier = 0.75f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = "1.0", UIMax = "20.0"))
	float controllerVerticalSensitivity = 6.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = "1.0", UIMax = "20.0"))
	float controllerHorizontalSensitivity = 6.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = "0.1", UIMax = "1.0"))
	float controllerADSSensitivityMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FMovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float sprintSpeed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float walkSpeed = 350.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float crouchSpeed = 250.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float slideSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float jumpHeight = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float adsMoveForwardWalkSpeedMultiplier = 0.6f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float adsMoveRightWalkSpeedMultiplier = 0.6f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float sprintMoveRightStrafeSpeedMultiplier = 0.4f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float crouchInitializeSpeed = sprintSpeed - 50.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float startSlideCancelSpeed = walkSpeed - 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float parkourReachDistance = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float vaultingReachDistance = 125.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float climbReachDistance = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float playerWallVaultAngleDegrees = 155.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float wallThinknessVaultToleranceDistance = 110.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float wallToLookDistance = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float eyePosOffset = 70.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float legPosOffset = -70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float leanDistance = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float leanInterpTime = 5.0f;

};

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
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class AWeapon* equippedWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> weaponClass;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FSensitivitySettings sensitivitySettings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FMovementSettings movementSettings;

private:
	void NullChecks();

	void SpawnDefaultWeapon();
	void FireButtonPressed();
	void FireButtonReleased();



// MARK: - [START] Movement Variables/Functions
private:

	// Camera Shake
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCameraShakeBase> headBobWalkCameraShake;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCameraShakeBase> headBobSprintCameraShake;

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
	class UAnimMontage* climbMontage;
	FTimerHandle slideTimerHandle;

	// Sprinting
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isSprinting;
	bool runButtonPressed;
	
	// Reloading
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isReloading;
	FTimerHandle reloadTimerHandle;

	// Leaning
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float leanValue;

	// ADS
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool ADSEnabled;
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float ADSValue;

protected:

	// Movement Update Functions
	void MovementUpdate();
	void CancelReloadUpdate();
	void SlideUpdate();
	void ADSUpdate();

	// Camera Shake
	void HandleCameraShake();

	// Mouse Inputs
	void MoveForward(float axisValue);
	void MoveRight(float axisValue);
	void Turn(float axisValue);
	void LookUp(float axisValue); 
	void TurnRate(float axisValue);
	void LookUpRate(float axisValue);

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
	bool SlideCancel();
	void PlayMantleAnimation(class UAnimMontage* montageAnim, float animTime, bool& inAnimBool);

	// Sprinting
	void SprintButtonPressed();
	void SprintButtonReleased();
	void ToggleSprint(bool toggle);

	// Reloading
	void ReloadButtonPressed();

	// Leaning
	void Lean(float axisValue);

	// ADS
	void ADSButtonPressed();
	void ADSButtonReleased();

// MARK: - [END] Movement Variables/Functions

// MARK: - [START] Helpers

private:
	void CancelTimer(FTimerHandle& timerHandle);

// MARK: - [END] Helpers


};
