// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Weapon.h"

#include "Misc/OutputDeviceNull.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	isSliding(false),
	isClimbing(false),
	isSprinting(false),
	runButtonPressed(false),
	isReloading(false),
	ADSEnabled(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay() {
	Super::BeginPlay();

	TArray<UCapsuleComponent*> components;
	this->GetComponents(components, false);
	this->capsuleComp = components[0];
	
	this->SpawnDefaultWeapon();
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->CancelReloadUpdate();
	this->MovementUpdate();
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AFPSCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AFPSCharacter::LookUp);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AFPSCharacter::JumpButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AFPSCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Prone"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ProneButtonPressed);

	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AFPSCharacter::SprintButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AFPSCharacter::SprintButtonReleased);

	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ReloadButtonPressed);
}

void AFPSCharacter::HandleCameraShake() {
	// TODO: Write This function in C++
	FOutputDeviceNull ar;
	const FString command = FString::Printf(TEXT("HandleCameraShake"));
	this->CallFunctionByNameWithArguments(*command, ar, NULL, true);
}

void AFPSCharacter::SpawnDefaultWeapon() {
	this->equippedWeapon = GetWorld()->SpawnActor<AWeapon>(this->weaponClass, this->GetActorTransform());
	if (this->equippedWeapon) {
		this->equippedWeapon->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RightHand"));
	}
}

void AFPSCharacter::MovementUpdate() {
	if (this->ADSEnabled || this->moveForwardValue <= -1.0f || this->moveRightValue != 0.0f || this->isVaulting) {
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
	if (this->ADSEnabled || this->isSprinting) {
		this->isReloading = false;
		GetWorldTimerManager().ClearTimer(this->reloadTimerHandle);
	}
}

void AFPSCharacter::MoveForward(float axisValue) {
	this->moveForwardValue = axisValue;
	if (this->moveForwardValue != 0.0f) {
		const FRotator rotation = this->Controller->GetControlRotation();
		const FRotator yawRotation = FRotator(0.0f, rotation.Yaw, 0.0f);
		const FVector direction = FVector(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
		if (this->ADSEnabled) this->moveForwardValue *= 0.6f;
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
		if (this->ADSEnabled) this->moveRightValue *= 0.6f;
		this->AddMovementInput(direction, this->moveRightValue);
		this->HandleCameraShake();
	}
}

void AFPSCharacter::Turn(float axisValue) {
	this->turnValue = axisValue;
	this->turnValue *= 0.5f;
	this->AddControllerYawInput(this->turnValue);
}

void AFPSCharacter::LookUp(float axisValue) {
	this->lookValue = axisValue;
	this->lookValue *= 0.5f;
	this->AddControllerPitchInput(this->lookValue);
}

void AFPSCharacter::CrouchButtonPressed() {
	this->ToggleCrouch(!this->isCrouching);
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
	this->Vault();
	this->Climb();
}

void AFPSCharacter::SprintButtonPressed() {
	this->runButtonPressed = true;
}

void AFPSCharacter::SprintButtonReleased() {
	this->runButtonPressed = false;
}

void AFPSCharacter::ToggleSprint(bool toggle) {
	if (toggle) {
		this->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		this->isSprinting = true;
	} else {
		this->GetCharacterMovement()->MaxWalkSpeed = 350.0f;
		this->isSprinting = false;
	}
}

void AFPSCharacter::ReloadButtonPressed() {
	if (this->isReloading) return;
	if (!this->isSprinting && !this->ADSEnabled) {
		this->isReloading = true;
		GetWorldTimerManager().SetTimer(this->reloadTimerHandle, [&](){
			this->isReloading = false;
		}, 2.17f, false);
	}
}

void AFPSCharacter::Vault() {
	FHitResult hitResult;
	FVector start = this->GetActorLocation();
	FVector end = start + this->GetActorRotation().Vector() * 200.0f;
	FCollisionObjectQueryParams params;
	params.ObjectTypesToQuery = params.AllStaticObjects;
	if (!GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, params)) {
		FHitResult vaultHitResult;
		FVector startVaultPos = this->GetActorLocation() + FVector(0.0f, 0.0f, -70.0f);
		FVector endVaultPos = start + this->GetActorRotation().Vector() * 200.0f;
		FCollisionObjectQueryParams paramsVault;
		paramsVault.ObjectTypesToQuery = paramsVault.AllStaticObjects;
		if (GetWorld()->LineTraceSingleByObjectType(vaultHitResult, startVaultPos, endVaultPos, paramsVault)) {
			this->capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Cast<UCharacterMovementComponent>(this->GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Flying);
			UAnimInstance* animInstance = this->GetMesh()->GetAnimInstance();
			if (animInstance && this->vaultMontage) {
				this->isVaulting = true;
				animInstance->Montage_Play(this->vaultMontage);
				FTimerHandle timerHandle;
				GetWorldTimerManager().SetTimer(timerHandle, [&](){ 
					this->capsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					Cast<UCharacterMovementComponent>(this->GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Walking);
					this->isVaulting = false;
				 }, 1.17f, false);
			} 
		}
	}
}

void AFPSCharacter::Slide() {
	
}

void AFPSCharacter::Climb() {
	
}


