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

UENUM(BlueprintType)
enum class EWeaponSlot : uint8 {
	EWS_Primary UMETA(DisplayName = "Primary"),
	EWS_Secondary UMETA(DisplayName = "Secondary"),
	EWS_Holster UMETA(DisplayName = "Holster"),
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};


USTRUCT(BlueprintType)
struct FPlayerLoadout
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AWeapon> primaryWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AWeapon> secondaryWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AWeapon> holsterWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimMontage* weaponSwitchMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<class AWeapon>> weaponClasses;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class AWeapon*> weapons;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int weaponIndex = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsSwitchingWeapon = false;
	UPROPERTY()
	FTimerHandle weaponSwitchTimerHandle_1;
	UPROPERTY()
	FTimerHandle weaponSwitchTimerHandle_2;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class AWeapon* equippedWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FPlayerLoadout playerLoadout;	

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FSensitivitySettings sensitivitySettings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FMovementSettings movementSettings;

private:
	void NullChecks();
	
	// Weapons
	void SpawnDefaultWeapons();
	void SpawnWeapon(TSubclassOf<class AWeapon> weaponClass, EWeaponSlot weaponSlot);
	bool HasWeapon();
	TSubclassOf<class AWeapon> FindWeaponClass();

	void FireButtonPressed();
	void FireButtonReleased();

	void FireModeButtonPressed();

public:

	// Weapons
	void SpawnWeapon(class AWeapon*, EWeaponSlot weaponSlot);

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

	// Weapon Switching
	void SwitchWeaponButtonPressed(FKey keyPressed);

	// Interaction
	void InteractButtonPressed(FKey keyPressed);


public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FSensitivitySettings& GetSensitivitySettings() { return this->sensitivitySettings; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FMovementSettings& GetMovementSettings() { return this->movementSettings; }


// MARK: - [START] Helpers
private:
	void CancelTimer(FTimerHandle& timerHandle);

};
