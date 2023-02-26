// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Weapon.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
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

	this->AdjustCamera();
	this->SpawnDefaultWeapons();
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

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AFPSCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AFPSCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("Lean"), this, &AFPSCharacter::Lean);
	PlayerInputComponent->BindAxis(FName("TurnRate"), this, &AFPSCharacter::TurnRate);
	PlayerInputComponent->BindAxis(FName("LookUpRate"), this, &AFPSCharacter::LookUpRate);

	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &AFPSCharacter::JumpButtonPressed);
	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Pressed, this, &AFPSCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction(FName("Prone"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ProneButtonPressed);
	PlayerInputComponent->BindAction(FName("Sprint"), EInputEvent::IE_Pressed, this, &AFPSCharacter::SprintButtonPressed);
	PlayerInputComponent->BindAction(FName("Sprint"), EInputEvent::IE_Released, this, &AFPSCharacter::SprintButtonReleased);
	PlayerInputComponent->BindAction(FName("Reload"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Pressed, this, &AFPSCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Released, this, &AFPSCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction(FName("FireMode"), EInputEvent::IE_Pressed, this, &AFPSCharacter::FireModeButtonPressed);
	PlayerInputComponent->BindAction(FName("ADS"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ADSButtonPressed);
	PlayerInputComponent->BindAction(FName("ADS"), EInputEvent::IE_Released, this, &AFPSCharacter::ADSButtonReleased);
	PlayerInputComponent->BindAction(FName("WeaponSwitch"), EInputEvent::IE_Pressed, this, &AFPSCharacter::SwitchWeaponButtonPressed);
	PlayerInputComponent->BindAction(FName("Interact"), EInputEvent::IE_Pressed, this, &AFPSCharacter::InteractButtonPressed);
	PlayerInputComponent->BindAction(FName("Interact"), EInputEvent::IE_Repeat, this, &AFPSCharacter::InteractButtonHeld);
	PlayerInputComponent->BindAction(FName("Interact"), EInputEvent::IE_Released, this, &AFPSCharacter::InteractButtonReleased);
}

void AFPSCharacter::NullChecks() {
	if (!this->capsuleComp) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: capsuleComp* is NULL")), false);
	if (!this->movementSettings.headBobWalkCameraShake) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: headBobWalkCameraShake* is NULL")), false);
	if (!this->movementSettings.headBobSprintCameraShake) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: headBobSprintCameraShake* is NULL")), false);
	if (!this->movementSettings.vaultMontage) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: vaultMontage* is NULL")), false);
	if (!this->movementSettings.climbMontage) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: climbMontage* is NULL")), false);
}

void AFPSCharacter::HandleCameraShake() {
	if (this->movementSettings.isSprinting) {
		if (this->movementSettings.headBobSprintCameraShake)
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->movementSettings.headBobSprintCameraShake);
	} else {
		if (this->movementSettings.headBobWalkCameraShake)
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->movementSettings.headBobWalkCameraShake, 1.5f);
	}
}

void AFPSCharacter::AdjustCamera() {
	TArray<UCameraComponent*> components;
	this->GetComponents(components, false);
	if (components.IsEmpty()) { return; }
	if (UCameraComponent* camera = components[0]) {
		this->sensitivitySettings.affectedFOV = (this->sensitivitySettings.bAffectedFOV) ? this->sensitivitySettings.FOV - 10 : 80;
		float normalizedRange = ((this->sensitivitySettings.FOV - 90.0f) / (110.0f - 90.0f));
		float lerpValue = FMath::Lerp<float>(1.0f, 1.5f, normalizedRange);
		FVector result = camera->GetRelativeLocation() * lerpValue;
		camera->SetFieldOfView(this->sensitivitySettings.FOV);
		if (lerpValue > 1.01f) {
			GEngine->Exec( GetWorld(), TEXT("r.Upscale.Panini.D 0.45"));
			GEngine->Exec( GetWorld(), TEXT("r.Upscale.Panini.s 0.15"));
		} else {
			GEngine->Exec( GetWorld(), TEXT("r.Upscale.Panini.D 0"));
			GEngine->Exec( GetWorld(), TEXT("r.Upscale.Panini.s 0"));
		}
		camera->SetRelativeLocation(result);
	}
}

void AFPSCharacter::SpawnDefaultWeapons() {
	if (this->playerLoadout.primaryWeapon) this->playerLoadout.weaponClasses.Add(this->playerLoadout.primaryWeapon);
	if (this->playerLoadout.secondaryWeapon) this->playerLoadout.weaponClasses.Add(this->playerLoadout.secondaryWeapon);
	if (this->playerLoadout.holsterWeapon) this->playerLoadout.weaponClasses.Add(this->playerLoadout.holsterWeapon);
	TSubclassOf<class AWeapon> weaponClass = this->FindWeaponClass();
	if (weaponClass == this->playerLoadout.primaryWeapon) {
		this->playerLoadout.weaponIndex = 0;
		this->SpawnWeapon(weaponClass, EWeaponSlot::EWS_Primary);
	} else if (weaponClass == this->playerLoadout.secondaryWeapon) {
		this->playerLoadout.weaponIndex = 1;
		this->SpawnWeapon(weaponClass, EWeaponSlot::EWS_Secondary);
	} else if (weaponClass == this->playerLoadout.holsterWeapon) {
		this->playerLoadout.weaponIndex = 2;
		this->SpawnWeapon(weaponClass, EWeaponSlot::EWS_Holster);
	}
}

void AFPSCharacter::SpawnWeapon(TSubclassOf<AWeapon> weaponClass, EWeaponSlot weaponSlot) {
	if (this->equippedWeapon) { this->equippedWeapon->Destroy(); this->equippedWeapon = nullptr; }
	if (!weaponClass) { GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: weaponClass* is NULL")), false); return; }
	if (weaponSlot == EWeaponSlot::EWS_Primary) {
		this->playerLoadout.primaryWeapon = weaponClass;
	} else if (weaponSlot == EWeaponSlot::EWS_Secondary) {
		this->playerLoadout.secondaryWeapon = weaponClass;
	} else if (weaponSlot == EWeaponSlot::EWS_Holster) {
		this->playerLoadout.holsterWeapon = weaponClass;
	} else { return; }
	FActorSpawnParameters params;
	params.Owner = this;
	this->equippedWeapon = GetWorld()->SpawnActor<AWeapon>(weaponClass, this->GetActorLocation(), FRotator::ZeroRotator, params);
	if (this->equippedWeapon) {
		FName placement = (this->equippedWeapon->GetWeaponType() == EWeaponType::EWT_Sniper) ? FName("LeftHand") : FName("RightHand");
		this->equippedWeapon->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, placement);
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AFPSCharacter]: equippedWeapon* is NULL")), false);
	}
}

TSubclassOf<class AWeapon> AFPSCharacter::FindWeaponClass() {
	if (!this->playerLoadout.primaryWeapon && !this->playerLoadout.secondaryWeapon && !this->playerLoadout.holsterWeapon) return nullptr;
	if (this->playerLoadout.primaryWeapon && !this->playerLoadout.secondaryWeapon && !this->playerLoadout.holsterWeapon) return this->playerLoadout.primaryWeapon;
	if (this->playerLoadout.secondaryWeapon && !this->playerLoadout.primaryWeapon && !this->playerLoadout.holsterWeapon) return this->playerLoadout.secondaryWeapon;
	if (this->playerLoadout.holsterWeapon && !this->playerLoadout.primaryWeapon && !this->playerLoadout.secondaryWeapon) return this->playerLoadout.holsterWeapon;
	if (this->playerLoadout.secondaryWeapon && this->playerLoadout.holsterWeapon && !this->playerLoadout.primaryWeapon) return this->playerLoadout.secondaryWeapon;
	return this->playerLoadout.primaryWeapon;
}

void AFPSCharacter::SpawnWeapon(AWeapon* weapon, EWeaponSlot weaponSlot) {
	if (!weapon) return;
	UClass* weaponClass = weapon->GetClass();
	TSubclassOf<AWeapon> attachmentSubClass = TSoftClassPtr<AWeapon>(weaponClass).Get();
	this->SpawnWeapon(weaponClass, weaponSlot);
}

void AFPSCharacter::FireButtonPressed() {
	if (!this->equippedWeapon || this->movementSettings.isReloading || this->playerLoadout.bIsSwitchingWeapon) return;
	this->equippedWeapon->Shoot();
}

void AFPSCharacter::FireButtonReleased() {
	if (!this->equippedWeapon) return;
	this->equippedWeapon->StopShooting();
}

void AFPSCharacter::FireModeButtonPressed() {
	if (!this->equippedWeapon || this->movementSettings.isReloading || this->playerLoadout.bIsSwitchingWeapon) return;
	if (this->equippedWeapon->IsShooting()) return;
	this->equippedWeapon->ChangeFiringMode();
}

void AFPSCharacter::MovementUpdate() {
	if (this->movementSettings.ADSEnabled || this->movementSettings.moveForwardValue <= -1.0f || (this->movementSettings.moveRightValue != 0.0f && this->movementSettings.moveForwardValue == 0.0f) || this->movementSettings.isVaulting || this->movementSettings.isClimbing 
		|| this->GetVelocity().Length() <= this->movementSettings.walkSpeed - 50.0f || (this->equippedWeapon && this->equippedWeapon->IsShooting()) || this->playerLoadout.bIsSwitchingWeapon) {
		this->ToggleSprint(false);
		return;
	}
	if (this->movementSettings.runButtonPressed) {
		if (this->movementSettings.isProning || this->movementSettings.isCrouching) {
			this->ToggleCrouch(false);
			this->movementSettings.isProning = false;
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
	if (this->movementSettings.isSprinting && this->movementSettings.reloadTimerHandle.IsValid()) {
		this->movementSettings.isReloading = false;
		this->CancelTimer(this->movementSettings.reloadTimerHandle);
	}
}

void AFPSCharacter::SlideUpdate() {
	if (this->movementSettings.isSliding && this->GetVelocity().Size() < this->movementSettings.startSlideCancelSpeed) {
		this->SlideCancel();
		return;
	}
	if (this->movementSettings.isSliding) this->GetCharacterMovement()->MaxWalkSpeed = this->movementSettings.slideSpeed;
}

void AFPSCharacter::ADSUpdate() {
	if (!this->equippedWeapon) return;
	this->movementSettings.ADSEnabled = !this->movementSettings.isReloading && !this->playerLoadout.bIsSwitchingWeapon && !this->equippedWeapon->GetWeaponStats().bIsClipping && this->movementSettings.bAdsButtonPressed;
	float adsSpeed = this->equippedWeapon->GetWeaponStats().adsSpeed;
	this->movementSettings.ADSValue = (this->movementSettings.ADSEnabled) ? FMath::FInterpTo(this->movementSettings.ADSValue, 1.0f, GetWorld()->GetDeltaSeconds(), adsSpeed) : FMath::FInterpTo(this->movementSettings.ADSValue, 0.0f, GetWorld()->GetDeltaSeconds(), adsSpeed);
	this->equippedWeapon->SetADSValue(this->movementSettings.ADSValue);
}

void AFPSCharacter::MoveForward(float axisValue) {
	this->movementSettings.moveForwardValue = axisValue;
	if (this->movementSettings.moveForwardValue != 0.0f) {
		const FRotator rotation = this->Controller->GetControlRotation();
		const FRotator yawRotation = FRotator(0.0f, rotation.Yaw, 0.0f);
		const FVector direction = FVector(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
		if (this->movementSettings.ADSEnabled) this->movementSettings.moveForwardValue *= this->movementSettings.adsMoveForwardWalkSpeedMultiplier;
		this->AddMovementInput(direction, this->movementSettings.moveForwardValue);
		this->HandleCameraShake();
	}
}

void AFPSCharacter::MoveRight(float axisValue) {
	this->movementSettings.moveRightValue = axisValue;
	if (this->movementSettings.moveRightValue != 0.0f) {
		const FRotator rotation = this->Controller->GetControlRotation();
		const FRotator yawRotation = FRotator(0.0f, rotation.Yaw, 0.0f);
		const FVector direction = FVector(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
		if (this->movementSettings.ADSEnabled) this->movementSettings.moveRightValue *= this->movementSettings.adsMoveRightWalkSpeedMultiplier;
		if (this->movementSettings.isSprinting) this->movementSettings.moveRightValue *= this->movementSettings.sprintMoveRightStrafeSpeedMultiplier;
		this->AddMovementInput(direction, this->movementSettings.moveRightValue);
		this->HandleCameraShake();
	}
}

void AFPSCharacter::Turn(float axisValue) {
	this->movementSettings.turnValue = axisValue;
	this->movementSettings.turnValue *= (this->sensitivitySettings.mouseSensitivity / 100.0f);
	if (this->movementSettings.ADSEnabled) this->movementSettings.turnValue *= this->sensitivitySettings.mouseADSSensitivityMultiplier;
	this->AddControllerYawInput(this->movementSettings.turnValue);
}

void AFPSCharacter::LookUp(float axisValue) {
	this->movementSettings.lookValue = axisValue;
	this->movementSettings.lookValue *= (this->sensitivitySettings.mouseSensitivity / 100.0f);
	if (this->movementSettings.ADSEnabled) this->movementSettings.lookValue *= this->sensitivitySettings.mouseADSSensitivityMultiplier;
	this->AddControllerPitchInput(this->movementSettings.lookValue);
}

void AFPSCharacter::TurnRate(float axisValue) {
	this->movementSettings.turnValueController = axisValue * (this->sensitivitySettings.controllerHorizontalSensitivity * 10.0f) * GetWorld()->GetDeltaSeconds();
	if (this->movementSettings.ADSEnabled) this->movementSettings.turnValueController *= this->sensitivitySettings.controllerADSSensitivityMultiplier;
	this->AddControllerYawInput(this->movementSettings.turnValueController);
}

void AFPSCharacter::LookUpRate(float axisValue) {
	this->movementSettings.lookValueController = axisValue * (this->sensitivitySettings.controllerHorizontalSensitivity * 10.0f) * GetWorld()->GetDeltaSeconds();
	if (this->movementSettings.ADSEnabled) this->movementSettings.lookValueController *= this->sensitivitySettings.controllerADSSensitivityMultiplier;
	this->AddControllerPitchInput(this->movementSettings.lookValueController);
}

void AFPSCharacter::CrouchButtonPressed() {
	if (this->SlideCancel()) return;
	if (this->GetVelocity().Size() >= this->movementSettings.crouchInitializeSpeed) {
		this->Slide();
	} else {
		this->ToggleCrouch(!this->movementSettings.isCrouching);
	}
}

void AFPSCharacter::ToggleCrouch(bool toggle) {
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->movementSettings.headBobSprintCameraShake, 1.25f);
	this->movementSettings.isProning = false;
	if (toggle) {
		this->movementSettings.isCrouching = true;
		this->Crouch();
	} else {
		this->movementSettings.isCrouching = false;
		this->UnCrouch();
	}
}

void AFPSCharacter::ProneButtonPressed() {
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->movementSettings.headBobSprintCameraShake, 1.25f);
	this->movementSettings.isProning = !this->movementSettings.isProning;
	if (this->movementSettings.isProning) {
		this->Crouch();
	} else {
		this->UnCrouch();
	}
}

void AFPSCharacter::JumpButtonPressed() {
	if (this->playerLoadout.bIsSwitchingWeapon) return;
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->movementSettings.headBobSprintCameraShake, 2.0f);
	this->Jump();
	this->SetupParkour();
	this->SlideCancel();
}

void AFPSCharacter::SprintButtonPressed() {
	this->movementSettings.runButtonPressed = true;
	this->SlideCancel();
}

void AFPSCharacter::SprintButtonReleased() {
	this->movementSettings.runButtonPressed = false;
}

void AFPSCharacter::ToggleSprint(bool toggle) {
	if (toggle) {
		this->GetCharacterMovement()->MaxWalkSpeed = this->movementSettings.sprintSpeed;
		this->movementSettings.isSprinting = true;
	} else {
		if (!this->movementSettings.isSliding) {
			this->GetCharacterMovement()->MaxWalkSpeed = this->movementSettings.walkSpeed;
			this->movementSettings.isSprinting = false;
		}
	}
}

void AFPSCharacter::ReloadButtonPressed(FKey keyPressed) {
	if (keyPressed.ToString().Equals("Gamepad_FaceButton_Left")) {
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, [&](){
			if (this->interactionSettings.bIsInteractionHeld) return; 
			if (this->movementSettings.isReloading || this->playerLoadout.bIsSwitchingWeapon || !this->equippedWeapon) return;
			if (!this->movementSettings.isSprinting || !this->movementSettings.ADSEnabled) {
				FWeaponMontages weaponMontages = this->equippedWeapon->GetWeaponMontages();
				if (!weaponMontages.reloadMontage) return;
				float animTime = this->PlayAnimMontage(weaponMontages.reloadMontage);
				this->movementSettings.isReloading = true;
				this->equippedWeapon->StopShooting();
				GetWorldTimerManager().SetTimer(this->movementSettings.reloadTimerHandle, [&](){
					this->movementSettings.isReloading = false;
				}, animTime, false);
			}
		}, 0.2f, false);
		return;
	}
	if (this->movementSettings.isReloading || this->playerLoadout.bIsSwitchingWeapon || !this->equippedWeapon) return;
	if (!this->movementSettings.isSprinting || !this->movementSettings.ADSEnabled) {
		// TODO: - Configure animTime for shotguns and snipers (animations that require chambers)
		// MaxAmmo / How much left in chamber
		FWeaponMontages weaponMontages = this->equippedWeapon->GetWeaponMontages();
		if (!weaponMontages.reloadMontage) return;
		float animTime = this->PlayAnimMontage(weaponMontages.reloadMontage);
		this->movementSettings.isReloading = true;
		this->equippedWeapon->StopShooting();
		GetWorldTimerManager().SetTimer(this->movementSettings.reloadTimerHandle, [&](){
			this->movementSettings.isReloading = false;
		}, animTime, false);
	}
}

void AFPSCharacter::Lean(float axisValue) {
	if (axisValue <= -1.0f) {
		this->movementSettings.leanValue = FMath::FInterpTo(this->movementSettings.leanValue, -this->movementSettings.leanDistance, GetWorld()->DeltaTimeSeconds, this->movementSettings.leanInterpTime);
	} else if (axisValue >= 1.0f) {
		this->movementSettings.leanValue = FMath::FInterpTo(this->movementSettings.leanValue, this->movementSettings.leanDistance, GetWorld()->DeltaTimeSeconds, this->movementSettings.leanInterpTime);
	} else {
		this->movementSettings.leanValue = FMath::FInterpTo(this->movementSettings.leanValue, 0.0f, GetWorld()->DeltaTimeSeconds, this->movementSettings.leanInterpTime);
	}
}

void AFPSCharacter::ADSButtonPressed() {
	this->movementSettings.bAdsButtonPressed = true;
	if (this->equippedWeapon && this->equippedWeapon->GetWeaponStats().bIsClipping || this->playerLoadout.bIsSwitchingWeapon) { this->movementSettings.ADSEnabled = false; return; }
	this->movementSettings.ADSEnabled = true;
}

void AFPSCharacter::ADSButtonReleased() {
	this->movementSettings.bAdsButtonPressed = false;
	this->movementSettings.ADSEnabled = false;
}

void AFPSCharacter::SwitchWeaponButtonPressed(FKey keyPressed) {
	if (this->movementSettings.isReloading || this->movementSettings.isClimbing || this->movementSettings.isVaulting || this->movementSettings.isSliding || 
		this->GetCharacterMovement()->IsFalling() || this->playerLoadout.weaponClasses.Num() <= 1 || this->playerLoadout.bIsSwitchingWeapon) return;
	float animSpeed = 1.70f;
	float animRate = 0.9f;
	float divisor = 2.0f;
	FString keyName = keyPressed.ToString();
	if (keyName.Equals("One")) {
		if (!this->playerLoadout.primaryWeapon || this->playerLoadout.weaponIndex == 0) return;
		this->playerLoadout.bIsSwitchingWeapon = true;
		this->movementSettings.isReloading = false;
		this->movementSettings.ADSEnabled = false;
		if (this->equippedWeapon) this->equippedWeapon->StopShooting();
		this->playerLoadout.weaponIndex = 0;
		animSpeed = this->PlayAnimMontage(this->playerLoadout.weaponSwitchMontage, animRate);
		GetWorldTimerManager().SetTimer(this->playerLoadout.weaponSwitchTimerHandle_1, [&](){ 
			this->playerLoadout.bIsSwitchingWeapon = false;
		}, animSpeed, false);
		GetWorldTimerManager().SetTimer(this->playerLoadout.weaponSwitchTimerHandle_2, [&](){ 
			this->SpawnWeapon(this->playerLoadout.primaryWeapon, EWeaponSlot::EWS_Primary);
		}, animSpeed / divisor, false);
	} else if (keyName.Equals("Two")) {
		if (!this->playerLoadout.secondaryWeapon || this->playerLoadout.weaponIndex == 1) return;
		this->playerLoadout.bIsSwitchingWeapon = true;
		this->movementSettings.isReloading = false;
		this->movementSettings.ADSEnabled = false;
		if (this->equippedWeapon) this->equippedWeapon->StopShooting();
		this->playerLoadout.weaponIndex = 1;
		animSpeed = this->PlayAnimMontage(this->playerLoadout.weaponSwitchMontage, animRate);
		GetWorldTimerManager().SetTimer(this->playerLoadout.weaponSwitchTimerHandle_1, [&](){ 
			this->playerLoadout.bIsSwitchingWeapon = false;
		}, animSpeed, false);
		GetWorldTimerManager().SetTimer(this->playerLoadout.weaponSwitchTimerHandle_2, [&](){ 
			this->SpawnWeapon(this->playerLoadout.secondaryWeapon, EWeaponSlot::EWS_Secondary);
		}, animSpeed / divisor, false);
	} else if (keyName.Equals("Three")) {
		if (!this->playerLoadout.holsterWeapon || this->playerLoadout.weaponIndex == 2) return;
		this->playerLoadout.bIsSwitchingWeapon = true;
		this->movementSettings.isReloading = false;
		this->movementSettings.ADSEnabled = false;
		if (this->equippedWeapon) this->equippedWeapon->StopShooting();
		this->playerLoadout.weaponIndex = 2;
		animSpeed = this->PlayAnimMontage(this->playerLoadout.weaponSwitchMontage, animRate);
		GetWorldTimerManager().SetTimer(this->playerLoadout.weaponSwitchTimerHandle_1, [&](){ 
			this->playerLoadout.bIsSwitchingWeapon = false;
		}, animSpeed, false);
		GetWorldTimerManager().SetTimer(this->playerLoadout.weaponSwitchTimerHandle_2, [&](){ 
			this->SpawnWeapon(this->playerLoadout.holsterWeapon, EWeaponSlot::EWS_Holster);
		}, animSpeed / divisor, false);
	} else {
		this->playerLoadout.bIsSwitchingWeapon = true;
		this->movementSettings.isReloading = false;
		this->movementSettings.ADSEnabled = false;
		if (this->equippedWeapon) this->equippedWeapon->StopShooting();
		animSpeed = this->PlayAnimMontage(this->playerLoadout.weaponSwitchMontage, animRate);
		GetWorldTimerManager().SetTimer(this->playerLoadout.weaponSwitchTimerHandle_1, [&](){ 
			this->playerLoadout.bIsSwitchingWeapon = false;
		}, animSpeed, false);
		GetWorldTimerManager().SetTimer(this->playerLoadout.weaponSwitchTimerHandle_2, [this, keyName](){ 
		if (keyName.Equals("MouseScrollUp") || keyName.Equals("Gamepad_FaceButton_Top")) {
			this->playerLoadout.weaponIndex++;
			if (this->playerLoadout.weaponIndex >= 3) this->playerLoadout.weaponIndex = 0;
			if (this->playerLoadout.weaponIndex == 0) {
				if (!this->playerLoadout.primaryWeapon) { this->SpawnWeapon(this->playerLoadout.secondaryWeapon, EWeaponSlot::EWS_Secondary); this->playerLoadout.weaponIndex = 1; }
				else this->SpawnWeapon(this->playerLoadout.primaryWeapon, EWeaponSlot::EWS_Primary);
			} else if (this->playerLoadout.weaponIndex == 1) {
				if (!this->playerLoadout.secondaryWeapon) { this->SpawnWeapon(this->playerLoadout.holsterWeapon, EWeaponSlot::EWS_Holster); this->playerLoadout.weaponIndex = 2;  }
				else this->SpawnWeapon(this->playerLoadout.secondaryWeapon, EWeaponSlot::EWS_Secondary);
			} else if (this->playerLoadout.weaponIndex == 2) {
				if (!this->playerLoadout.holsterWeapon) { this->SpawnWeapon(this->playerLoadout.primaryWeapon, EWeaponSlot::EWS_Primary); this->playerLoadout.weaponIndex = 0; }
				else this->SpawnWeapon(this->playerLoadout.holsterWeapon, EWeaponSlot::EWS_Holster);
			}
		} else if (keyName.Equals("MouseScrollDown")) {
			this->playerLoadout.weaponIndex--;
			if (this->playerLoadout.weaponIndex < 0) this->playerLoadout.weaponIndex = 2;
			if (this->playerLoadout.weaponIndex == 0) {
				if (!this->playerLoadout.primaryWeapon) { this->SpawnWeapon(this->playerLoadout.holsterWeapon, EWeaponSlot::EWS_Holster); this->playerLoadout.weaponIndex = 2; }
				else this->SpawnWeapon(this->playerLoadout.primaryWeapon, EWeaponSlot::EWS_Primary);
			} else if (this->playerLoadout.weaponIndex == 1) {
				if (!this->playerLoadout.secondaryWeapon) { this->SpawnWeapon(this->playerLoadout.primaryWeapon, EWeaponSlot::EWS_Primary); this->playerLoadout.weaponIndex = 0;  }
				else this->SpawnWeapon(this->playerLoadout.secondaryWeapon, EWeaponSlot::EWS_Secondary);
			} else if (this->playerLoadout.weaponIndex == 2) {
				if (!this->playerLoadout.holsterWeapon) { this->SpawnWeapon(this->playerLoadout.secondaryWeapon, EWeaponSlot::EWS_Secondary); this->playerLoadout.weaponIndex = 1; }
				else this->SpawnWeapon(this->playerLoadout.holsterWeapon, EWeaponSlot::EWS_Holster);
			}
		}		
		}, animSpeed / divisor, false);
	}
}

void AFPSCharacter::Interact() {
	if (this->movementSettings.isClimbing || this->movementSettings.isVaulting || this->movementSettings.isSliding || 
		this->GetCharacterMovement()->IsFalling() ||this->playerLoadout.bIsSwitchingWeapon) return;
	AController* PlayerController = this->GetController();
	FVector start;
	FRotator rotation;
	 this->GetController()->GetPlayerViewPoint(start, rotation);
	const FVector end = start + (rotation.Vector() * this->interactionSettings.interactReachDistance);
	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(PlayerController->GetPawn());
	if (this->GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, params)) {
		// TODO: - Play Pickup Animation.
		this->movementSettings.isReloading = false;
		this->CancelReloadUpdate();
		DrawDebugLine(this->GetWorld(), start, end, FColor::Blue, false, 1.0f);
		DrawDebugPoint(GetWorld(), hitResult.ImpactPoint, 10.0f, FColor::Green, false, 3.f);
	}
}

void AFPSCharacter::InteractButtonPressed(FKey keyPressed) {
	if (keyPressed.ToString().Equals("Gamepad_FaceButton_Left")) return;
	this->Interact();
}

void AFPSCharacter::InteractButtonHeld(FKey keyPressed) {
	if (!keyPressed.ToString().Equals("Gamepad_FaceButton_Left") || this->interactionSettings.bIsInteractionHeld) return;
	this->interactionSettings.bIsInteractionHeld = true;
	this->Interact();
}

void AFPSCharacter::InteractButtonReleased(FKey keyPressed) {
	if (!keyPressed.ToString().Equals("Gamepad_FaceButton_Left") && this->interactionSettings.bIsInteractionHeld != 1) return;
	this->interactionSettings.bIsInteractionHeld = false;
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
			this->CancelTimer(this->playerLoadout.weaponSwitchTimerHandle_1);
			this->CancelTimer(this->playerLoadout.weaponSwitchTimerHandle_2);
			this->playerLoadout.bIsSwitchingWeapon = false;	
			if (boxExtents.Z <= (this->capsuleComp->GetScaledCapsuleHalfHeight() / 2.0f)) {
				this->Vault();
			} else {
				this->Climb();
			}
		}
	}
}

void AFPSCharacter::Vault() {
	if (this->movementSettings.isVaulting) return;
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
		if (angle < this->movementSettings.playerWallVaultAngleDegrees || !this->movementSettings.isSprinting || !wallIsThick) {
			FVector origin;
			FVector boxExtents;
			if (hitResult.GetActor()) hitResult.GetActor()->GetActorBounds(false, origin, boxExtents, false);
			this->SetActorLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, boxExtents.Z + 10.0f));
			this->PlayMantleAnimation(this->movementSettings.climbMontage, 1.17f, this->movementSettings.isClimbing);
		} else {
			this->PlayMantleAnimation(this->movementSettings.vaultMontage, 0.7f, this->movementSettings.isVaulting);
		}
		this->movementSettings.isReloading = false;
		this->CancelTimer(this->movementSettings.reloadTimerHandle);
	}
}

void AFPSCharacter::Climb() {
	if (this->movementSettings.isClimbing) return;
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
			this->movementSettings.isReloading = false;
			this->CancelTimer(this->movementSettings.reloadTimerHandle);
			this->SetActorLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, boxExtents.Z + 50.0f));
			this->PlayMantleAnimation(this->movementSettings.climbMontage, 1.17f, this->movementSettings.isClimbing);
		}
	}
}

void AFPSCharacter::Slide() {
	this->CancelTimer(this->playerLoadout.weaponSwitchTimerHandle_1);
	this->CancelTimer(this->playerLoadout.weaponSwitchTimerHandle_2);
	this->playerLoadout.bIsSwitchingWeapon = false;	
	this->movementSettings.isSliding = true;
	this->movementSettings.isSprinting = false;
	this->movementSettings.runButtonPressed = false;
	GetWorldTimerManager().SetTimer(this->movementSettings.slideTimerHandle, [&](){
		this->movementSettings.isSliding = false;
	}, 1.53f, false);
}

bool AFPSCharacter::SlideCancel() {
	if (this->movementSettings.isSliding && this->movementSettings.slideTimerHandle.IsValid()) {
		this->movementSettings.isSliding = false;
		this->CancelTimer(this->movementSettings.slideTimerHandle);
		return true;
	}
	return false;
}

void AFPSCharacter::PlayMantleAnimation(UAnimMontage* montageAnim, float animTime, bool& inAnimBool) {
	this->capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Cast<UCharacterMovementComponent>(this->GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Flying);
	bool isClimbingMontage = this->movementSettings.climbMontage == montageAnim;
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

float AFPSCharacter::GetTurnValue() const {
	if (this->movementSettings.turnValue != 0.0f && this->movementSettings.turnValueController == 0.0f) return this->movementSettings.turnValue;
	if (this->movementSettings.turnValue == 0.0f && this->movementSettings.turnValueController != 0.0f) return this->movementSettings.turnValueController;
	return this->movementSettings.turnValue;
}

float AFPSCharacter::GetLookValue() const {
	if (this->movementSettings.lookValue != 0.0f && this->movementSettings.lookValueController == 0.0f) return this->movementSettings.lookValue;
	if (this->movementSettings.lookValue == 0.0f && this->movementSettings.lookValueController != 0.0f) return this->movementSettings.lookValueController;
	return this->movementSettings.lookValue; 
}



