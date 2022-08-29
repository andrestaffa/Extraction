// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Weapon.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFPSCharacter::AFPSCharacter() :
	moveForwardValue(0.0f),
	moveRightValue(0.0f),
	turnValue(0.0f),
	lookValue(0.0f),
	isCrouching(false),
	isProning(false),
	isVaulting(false),
	isClimbing(false),
	isSliding(false),
	isSprinting(false),
	runButtonPressed(false),
	isReloading(false),
	leanValue(0.0f),
	ADSEnabled(false),
	ADSValue(0.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TArray<UCapsuleComponent*> components;
	this->GetComponents(components, false);
	this->capsuleComp = components[0];

	this->GetCharacterMovement()->MaxWalkSpeedCrouched = this->movementSettings.crouchSpeed;
	this->GetCharacterMovement()->JumpZVelocity = this->movementSettings.jumpHeight;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay() {
	Super::BeginPlay();

	this->SpawnDefaultWeapon();
	this->NullChecks();
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->MovementUpdate();
	this->CancelReloadUpdate();
	this->SlideUpdate();
	this->ADSUpdate();
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AFPSCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AFPSCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Lean"), this, &AFPSCharacter::Lean);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AFPSCharacter::TurnRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AFPSCharacter::LookUpRate);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AFPSCharacter::JumpButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AFPSCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Prone"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ProneButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AFPSCharacter::SprintButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AFPSCharacter::SprintButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &AFPSCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &AFPSCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction(TEXT("ADS"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ADSButtonPressed);
	PlayerInputComponent->BindAction(TEXT("ADS"), EInputEvent::IE_Released, this, &AFPSCharacter::ADSButtonReleased);
}

void AFPSCharacter::NullChecks() {
	if (!this->capsuleComp) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: capsuleComp* is NULL")), false);
	if (!this->headBobWalkCameraShake) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: headBobWalkCameraShake* is NULL")), false);
	if (!this->headBobSprintCameraShake) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: headBobSprintCameraShake* is NULL")), false);
	if (!this->vaultMontage) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: vaultMontage* is NULL")), false);
	if (!this->climbMontage) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: climbMontage* is NULL")), false);
}

void AFPSCharacter::HandleCameraShake() {
	if (this->isSprinting) {
		if (this->headBobSprintCameraShake)
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->headBobSprintCameraShake);
	} else {
		if (this->headBobWalkCameraShake)
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->headBobWalkCameraShake, 1.5f);
	}
}

void AFPSCharacter::SpawnDefaultWeapon() {
	if (this->weaponClass) {
		this->equippedWeapon = GetWorld()->SpawnActor<AWeapon>(this->weaponClass, this->GetActorTransform());
		if (this->equippedWeapon) {
			this->equippedWeapon->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RightHand"));
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: equippedWeapon* is NULL")), false);
		}
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: weaponClass* is NULL")), false);
	}
}

void AFPSCharacter::FireButtonPressed() {
	if (this->isReloading) return;
	this->equippedWeapon->Shoot();
}

void AFPSCharacter::FireButtonReleased() {
	this->equippedWeapon->StopShooting();
}

void AFPSCharacter::MovementUpdate() {
	if (this->ADSEnabled || this->moveForwardValue <= -1.0f || (this->moveRightValue != 0.0f && this->moveForwardValue == 0.0f) || this->isVaulting || this->isClimbing 
		|| this->equippedWeapon->GetWeaponStats().bCanShoot) {
		this->ToggleSprint(false);
		return;
	}
	if (this->runButtonPressed) {
		if (this->isProning || this->isCrouching) {
			this->ToggleCrouch(false);
			this->isProning = false;
			this->ToggleSprint(true);
		} else {
			this->ToggleSprint(true);
		}
	} else {
		this->ToggleSprint(false);
	}
}

void AFPSCharacter::CancelReloadUpdate() {
	// TODO: If equipped weapon still has bullets and shoot button pressed, then cancel reload.
	if ((this->ADSEnabled || this->isSprinting) && this->reloadTimerHandle.IsValid()) {
		this->isReloading = false;
		this->CancelTimer(this->reloadTimerHandle);
	}
}

void AFPSCharacter::SlideUpdate() {
	if (this->isSliding && this->GetVelocity().Size() < this->movementSettings.startSlideCancelSpeed) {
		this->SlideCancel();
		return;
	}
	if (this->isSliding) this->GetCharacterMovement()->MaxWalkSpeed = this->movementSettings.slideSpeed;
}

void AFPSCharacter::ADSUpdate() {
	if (!this->equippedWeapon) return;
	float adsSpeed = this->equippedWeapon->GetWeaponStats().adsSpeed;
	this->ADSValue = (this->ADSEnabled) ? FMath::FInterpTo(this->ADSValue, 1.0f, GetWorld()->GetDeltaSeconds(), adsSpeed) : FMath::FInterpTo(this->ADSValue, 0.0f, GetWorld()->GetDeltaSeconds(), adsSpeed);
	this->equippedWeapon->SetADSValue(this->ADSValue);
}

void AFPSCharacter::MoveForward(float axisValue) {
	this->moveForwardValue = axisValue;
	if (this->moveForwardValue != 0.0f) {
		const FRotator rotation = this->Controller->GetControlRotation();
		const FRotator yawRotation = FRotator(0.0f, rotation.Yaw, 0.0f);
		const FVector direction = FVector(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
		if (this->ADSEnabled) this->moveForwardValue *= this->movementSettings.adsMoveForwardWalkSpeedMultiplier;
		this->AddMovementInput(direction, this->moveForwardValue);
		this->HandleCameraShake();
	}
}

void AFPSCharacter::MoveRight(float axisValue) {
	this->moveRightValue = axisValue;
	if (this->moveRightValue != 0.0f) {
		const FRotator rotation = this->Controller->GetControlRotation();
		const FRotator yawRotation = FRotator(0.0f, rotation.Yaw, 0.0f);
		const FVector direction = FVector(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
		if (this->ADSEnabled) this->moveRightValue *= this->movementSettings.adsMoveRightWalkSpeedMultiplier;
		if (this->isSprinting) this->moveRightValue *= this->movementSettings.sprintMoveRightStrafeSpeedMultiplier;
		this->AddMovementInput(direction, this->moveRightValue);
		this->HandleCameraShake();
	}
}

void AFPSCharacter::Turn(float axisValue) {
	this->turnValue = axisValue;
	this->turnValue *= (this->sensitivitySettings.mouseSensitivity / 100.0f);
	if (this->ADSEnabled) this->turnValue *= this->sensitivitySettings.mouseADSSensitivityMultiplier;
	this->AddControllerYawInput(this->turnValue);
}

void AFPSCharacter::LookUp(float axisValue) {
	this->lookValue = axisValue;
	this->lookValue *= (this->sensitivitySettings.mouseSensitivity / 100.0f);
	if (this->ADSEnabled) this->lookValue *= this->sensitivitySettings.mouseADSSensitivityMultiplier;
	this->AddControllerPitchInput(this->lookValue);
}

void AFPSCharacter::TurnRate(float axisValue) {
	this->turnValue = axisValue * (this->sensitivitySettings.controllerHorizontalSensitivity * 10.0f) * GetWorld()->GetDeltaSeconds();
	if (this->ADSEnabled) this->turnValue *= this->sensitivitySettings.controllerADSSensitivityMultiplier;
	this->AddControllerYawInput(this->turnValue);
}

void AFPSCharacter::LookUpRate(float axisValue) {
	this->lookValue = axisValue * (this->sensitivitySettings.controllerHorizontalSensitivity * 10.0f) * GetWorld()->GetDeltaSeconds();
	if (this->ADSEnabled) this->lookValue *= this->sensitivitySettings.controllerADSSensitivityMultiplier;
	this->AddControllerPitchInput(this->lookValue);
}

void AFPSCharacter::CrouchButtonPressed() {
	if (this->SlideCancel()) return;
	if (this->GetVelocity().Size() >= this->movementSettings.crouchInitializeSpeed) {
		this->Slide();
	} else {
		this->ToggleCrouch(!this->isCrouching);
	}
}

void AFPSCharacter::ToggleCrouch(bool toggle) {
	this->isProning = false;
	if (toggle) {
		this->isCrouching = true;
		this->Crouch();
	} else {
		this->isCrouching = false;
		this->UnCrouch();
	}
}

void AFPSCharacter::ProneButtonPressed() {
	this->isProning = !this->isProning;
	if (this->isProning) {
		this->Crouch();
	} else {
		this->UnCrouch();
	}
}

void AFPSCharacter::JumpButtonPressed() {
	this->Jump();
	this->SetupParkour();
	this->SlideCancel();
}

void AFPSCharacter::SprintButtonPressed() {
	this->runButtonPressed = true;
	this->SlideCancel();
}

void AFPSCharacter::SprintButtonReleased() {
	this->runButtonPressed = false;
}

void AFPSCharacter::ToggleSprint(bool toggle) {
	if (toggle) {
		this->GetCharacterMovement()->MaxWalkSpeed = this->movementSettings.sprintSpeed;
		this->isSprinting = true;
	} else {
		if (!this->isSliding) {
			this->GetCharacterMovement()->MaxWalkSpeed = this->movementSettings.walkSpeed;
			this->isSprinting = false;
		}
	}
}

void AFPSCharacter::ReloadButtonPressed() {
	if (this->isReloading) return;
	if (!this->isSprinting && !this->ADSEnabled) {
		this->isReloading = true;
		this->equippedWeapon->StopShooting();
		GetWorldTimerManager().SetTimer(this->reloadTimerHandle, [&](){
			this->isReloading = false;
		}, 2.17f, false);
	}
}

void AFPSCharacter::Lean(float axisValue) {
	if (axisValue <= -1.0f) {
		this->leanValue = FMath::FInterpTo(this->leanValue, -this->movementSettings.leanDistance, GetWorld()->DeltaTimeSeconds, this->movementSettings.leanInterpTime);
	} else if (axisValue >= 1.0f) {
		this->leanValue = FMath::FInterpTo(this->leanValue, this->movementSettings.leanDistance, GetWorld()->DeltaTimeSeconds, this->movementSettings.leanInterpTime);
	} else {
		this->leanValue = FMath::FInterpTo(this->leanValue, 0.0f, GetWorld()->DeltaTimeSeconds, this->movementSettings.leanInterpTime);
	}
}

void AFPSCharacter::ADSButtonPressed() {
	this->ADSEnabled = true;
}

void AFPSCharacter::ADSButtonReleased() {
	this->ADSEnabled = false;
}

void AFPSCharacter::SetupParkour() {
	FHitResult hitResult;
	FVector start = this->GetActorLocation() + FVector(0.0f, 0.0f, this->movementSettings.legPosOffset);
	FVector end = start + this->GetActorForwardVector() * this->movementSettings.parkourReachDistance;
	FCollisionObjectQueryParams params;
	params.ObjectTypesToQuery = params.AllStaticObjects;
	if (GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, params)) {
		if (hitResult.GetActor()) {
			FVector origin;
			FVector boxExtents;
			hitResult.GetActor()->GetActorBounds(false, origin, boxExtents, false);	
			if (boxExtents.Z <= (this->capsuleComp->GetScaledCapsuleHalfHeight() / 2.0f)) {
				this->Vault();
			} else {
				this->Climb();
			}
		}
	}
}

void AFPSCharacter::Vault() {
	if (this->isVaulting) return;
	FHitResult hitResult;
	FVector start = this->GetActorLocation() + FVector(0.0f, 0.0f, this->movementSettings.legPosOffset);
	FVector end = start + this->GetActorRotation().Vector() * this->movementSettings.vaultingReachDistance;
	FCollisionObjectQueryParams params;
	params.ObjectTypesToQuery = params.AllStaticObjects;
	if (GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, params)) {
		FVector A = end - start;
		FVector N = hitResult.ImpactNormal;
		float cosine = A.CosineAngle2D(N);
		float angle = acos(cosine);
		angle = angle * (180.0f / PI);
		FHitResult wallThickHitResult;
		start = hitResult.ImpactPoint;
		end = start + hitResult.ImpactNormal * -this->movementSettings.wallThinknessVaultToleranceDistance;
		bool wallIsThick = GetWorld()->LineTraceSingleByObjectType(wallThickHitResult, start, end, params);
		if (angle < this->movementSettings.playerWallVaultAngleDegrees || !this->isSprinting || !wallIsThick) {
			FVector origin;
			FVector boxExtents;
			if (hitResult.GetActor()) hitResult.GetActor()->GetActorBounds(false, origin, boxExtents, false);
			this->SetActorLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, boxExtents.Z + 10.0f));
			this->PlayMantleAnimation(this->climbMontage, 1.17f, this->isClimbing);
		} else {
			this->PlayMantleAnimation(this->vaultMontage, 0.7f, this->isVaulting);
		}
		this->isReloading = false;
		this->CancelTimer(this->reloadTimerHandle);
	}
}

void AFPSCharacter::Climb() {
	if (this->isClimbing) return;
	FHitResult hitResult;
	FVector start = this->GetActorLocation() + FVector(0.0f, 0.0f, this->movementSettings.eyePosOffset);
	FVector end = start + this->GetControlRotation().Vector() * this->movementSettings.climbReachDistance;
	FCollisionObjectQueryParams params;
	params.ObjectTypesToQuery = params.AllStaticObjects;
	if (GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, params)) {
		FVector origin;
		FVector boxExtents;
		if (hitResult.GetActor()) hitResult.GetActor()->GetActorBounds(false, origin, boxExtents, false);
		float distanceToTop = boxExtents.Z - (abs(origin.Z - hitResult.Location.Z));
		if (hitResult.ImpactNormal.Z >= 1.0f || distanceToTop <= this->movementSettings.wallToLookDistance) {
			this->isReloading = false;
			this->CancelTimer(this->reloadTimerHandle);
			this->SetActorLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, boxExtents.Z + 50.0f));
			this->PlayMantleAnimation(this->climbMontage, 1.17f, this->isClimbing);
		}
	}
}

void AFPSCharacter::Slide() {
	this->isSliding = true;
	this->isSprinting = false;
	this->runButtonPressed = false;
	GetWorldTimerManager().SetTimer(this->slideTimerHandle, [&](){
		this->isSliding = false;
	}, 1.53f, false);
}

bool AFPSCharacter::SlideCancel() {
	if (this->isSliding && this->slideTimerHandle.IsValid()) {
		this->isSliding = false;
		this->CancelTimer(this->slideTimerHandle);
		return true;
	}
	return false;
}

void AFPSCharacter::PlayMantleAnimation(UAnimMontage* montageAnim, float animTime, bool& inAnimBool) {
	this->capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Cast<UCharacterMovementComponent>(this->GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Flying);
	bool isClimbingMontage = this->climbMontage == montageAnim;
	if (montageAnim) { 
		inAnimBool = true;
		this->PlayAnimMontage(montageAnim);
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, [&](){ 
			this->capsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Cast<UCharacterMovementComponent>(this->GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Walking);
			inAnimBool = false;
			if (isClimbingMontage) {
				this->SetActorLocation(this->GetActorLocation() + this->GetActorForwardVector() * 50.0f);
			}
		}, animTime, false);
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: montageAnim* is NULL")), false);
	}
}

void AFPSCharacter::CancelTimer(FTimerHandle& timerHandle) {
	GetWorldTimerManager().ClearTimer(timerHandle);
	if (timerHandle.IsValid()) {
		timerHandle.Invalidate();
	}
}


