// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

USTRUCT(BlueprintType)
struct FSensitivitySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin = "90.0", UIMax = "110.0"))
	int FOV = 90;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(UIMin = "90.0", UIMax = "110.0"))
	int affectedFOV = 80;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAffectedFOV = false;
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

	// Character Movement
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

	// Parkour
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

	// Leaning
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float leanDistance = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float leanInterpTime = 5.0f;

	// Camera Shake
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UCameraShakeBase> headBobWalkCameraShake;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UCameraShakeBase> headBobSprintCameraShake;

	// Mouse Movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float moveForwardValue = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float moveRightValue = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float turnValue = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float lookValue = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float turnValueController = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float lookValueController = 0.0f;

	// Crouching/Proning
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isCrouching = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isProning = false;

	// Jumping/Vaulting/Sliding/Climbing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isVaulting = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isClimbing = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isSliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* vaultMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* climbMontage;
	FTimerHandle slideTimerHandle;

	// Sprinting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isSprinting = false;
	bool runButtonPressed = false;
	
	// Reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isReloading = false;
	FTimerHandle reloadTimerHandle;

	// Leaning
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float leanValue = 0.0f;

	// ADS
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bAdsButtonPressed = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool ADSEnabled = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ADSValue = 0.0f;

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

	void FireModeButtonPressed();

protected:

	// Movement Update Functions
	void MovementUpdate();
	void CancelReloadUpdate();
	void SlideUpdate();
	void ADSUpdate();

	// Camera Shake
	void HandleCameraShake();
	void AdjustCamera();

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


public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FSensitivitySettings& GetSensitivitySettings() { return this->sensitivitySettings; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FMovementSettings& GetMovementSettings() { return this->movementSettings; }


// MARK: - [START] Helpers
private:
	void CancelTimer(FTimerHandle& timerHandle);

};
