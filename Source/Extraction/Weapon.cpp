// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Bullet.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Camera/CameraShakeBase.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->weaponScope.ironSightPosition = FVector(0.0f, -14.2f, 0.0f);
	this->weaponBarrel.barrelPosition = FVector::ZeroVector;
	this->weaponGrip.gripPosition = FVector(10.0f, 0.0f, 25.702934f);

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay() {
	Super::BeginPlay();

	this->playerController = UGameplayStatics::GetPlayerController(this, 0);
	this->barrelSocket = this->GetItemSkeletalMesh()->GetSocketByName("Muzzle");
	this->weaponStats.currentFiringMode = this->GetWeaponStats().availableFiringModes[0];

	this->DefaultSocketLocations();

	this->SetAttachment(EWeaponAttachment::EWA_Scope, this->weaponScope.scopeClass);
	this->SetAttachment(EWeaponAttachment::EWA_Barrel, this->weaponBarrel.barrelClass);
	this->SetAttachment(EWeaponAttachment::EWA_Grip, this->weaponGrip.gripClass);

	this->NullChecks();
}

// Called every frame
void AWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->RecoilUpdate();
}

void AWeapon::NullChecks() {
	if (!this->playerController) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: playerController* is NULL")), false);
	if (!this->barrelSocket) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: barrelSocket* is NULL")), false);
	if (!this->bulletClass) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: bulletClass* is NULL")), false);
	if (!this->recoilCameraShakeClass) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: recoilCameraShakeClass* is NULL")), false);
}

void AWeapon::DefaultSocketLocations() {
	if (USkeletalMeshSocket* ironSightSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName("IronSight")))
		ironSightSocket->RelativeLocation = this->weaponScope.ironSightPosition;
	if (USkeletalMeshSocket* leftHandPlacementSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName("LeftHandPlacement")))
		leftHandPlacementSocket->RelativeLocation = FVector(6.860041f, -5.002760f, 21.294327f);
}

void AWeapon::RecoilUpdate() {
	if (!this->isShooting() || !this->playerController) return;
	this->AddRecoil();
}

void AWeapon::Shoot() {
	if (!this->barrelSocket) { GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: barrelSocket* is NULL")), false); return; }
	FTransform socketTransform = this->barrelSocket->GetSocketTransform(this->GetItemSkeletalMesh());
	FActorSpawnParameters params;
	params.Owner = this;
	FTimerHandle fireTimerHandle;
	if (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_FullAuto) {
		this->weaponStats.isShootingFullAuto = true;
		GetWorld()->SpawnActor<ABullet>((this->bulletClass) ? this->bulletClass : ABullet::StaticClass(), socketTransform.GetLocation(), this->GetActorForwardVector().Rotation(), params);
		if (this->recoilCameraShakeClass) GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->recoilCameraShakeClass);
		GetWorldTimerManager().SetTimer(fireTimerHandle, [&](){
			if (this->GetWeaponStats().isShootingFullAuto) this->Shoot();
		}, this->GetWeaponStats().fireRate, false);
	} else if (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_Single) {
		if (!this->GetWeaponStats().bCanShootSingle) return;
		this->weaponStats.bCanShootSingle = false;
		this->FireSingle(socketTransform, params);
		GetWorldTimerManager().SetTimer(fireTimerHandle, [&](){
			this->weaponStats.bCanShootSingle = true;
		}, this->GetWeaponStats().fireRate, false);
	} else if (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_Burst) {
		if (this->GetWeaponStats().bursts == 3) {
			this->weaponStats.isShootingBurst = false;
			this->weaponStats.bursts = 0;
			return;
		}
		this->weaponStats.isShootingBurst = true;
		this->weaponStats.bursts++;
		this->FireSingle(socketTransform, params);
		GetWorldTimerManager().SetTimer(fireTimerHandle, [&](){
			if (this->GetWeaponStats().isShootingBurst) this->Shoot();
		}, this->GetWeaponStats().fireRate, false);
	}
}

void AWeapon::StopShooting() {
	this->weaponStats.isShootingFullAuto = false;
}

void AWeapon::AddRecoil() {
	float verticalMult = (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_FullAuto) ? 1.0f : 1.0f;
	float horizontalMult = (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_FullAuto) ? 1.0f: 1.0f;
	this->playerController->AddPitchInput(-this->GetWeaponStats().verticalRecoil * GetWorld()->GetDeltaSeconds() * verticalMult);
	int rand = FMath::RandRange(0, 1);
	float yaw = (rand == 1) ? FMath::RandRange(this->GetWeaponStats().minHorizontalRecoil * horizontalMult, this->GetWeaponStats().maxHorizontalRecoil * horizontalMult) : FMath::RandRange(-this->GetWeaponStats().maxHorizontalRecoil * horizontalMult, -this->GetWeaponStats().minHorizontalRecoil * horizontalMult);
	this->playerController->AddYawInput(yaw * GetWorld()->GetDeltaSeconds());	
}

void AWeapon::FireSingle(FTransform& socketTransform, FActorSpawnParameters& params) {
	GetWorld()->SpawnActor<ABullet>((this->bulletClass) ? this->bulletClass : ABullet::StaticClass(), socketTransform.GetLocation(), this->GetActorForwardVector().Rotation(), params);
	if (this->recoilCameraShakeClass) GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->recoilCameraShakeClass);
}

const FVector AWeapon::BulletDirection() {
	FVector forwardVector = this->GetActorForwardVector() * -1;
	float multiplier = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(1.0f, 0.0f), this->GetWeaponStats().adsValue);
	float minBulletSpread = this->GetWeaponStats().minBulletSpread * multiplier;
	float maxBulletSpread = this->GetWeaponStats().maxBulletSpread * multiplier;
	float x = forwardVector.X + FMath::RandRange(minBulletSpread, maxBulletSpread);
	float y = forwardVector.Y + FMath::RandRange(minBulletSpread, maxBulletSpread);
	float z = forwardVector.Z + FMath::RandRange(minBulletSpread, maxBulletSpread);
	return FVector(x, y, z);
}

void AWeapon::ChangeFiringMode() {
	bool cond = this->GetWeaponStats().currentFiringMode == this->GetWeaponStats().availableFiringModes[0];
	this->weaponStats.currentFiringMode = (cond) ? this->GetWeaponStats().availableFiringModes[1] : this->GetWeaponStats().availableFiringModes[0];
}

void AWeapon::SetAttachment(EWeaponAttachment attachment, TSubclassOf<AActor> attachmentClass) {
	if (attachment == EWeaponAttachment::EWA_Scope) {
		if (!attachmentClass) { 
			if (this->weaponScope.currentEquippedScope) 
				this->weaponScope.currentEquippedScope->Destroy();
			if (USkeletalMeshSocket* ironSightSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName("IronSight")))
				ironSightSocket->RelativeLocation = this->weaponScope.ironSightPosition;
			this->weaponScope.scopeClass = nullptr;  
			return;
		}
		if (this->weaponScope.currentEquippedScope) this->weaponScope.currentEquippedScope->Destroy();
		if (AActor* scope = GetWorld()->SpawnActor<AActor>(attachmentClass, FVector::ZeroVector, FRotator::ZeroRotator)) {
			this->weaponScope.currentEquippedScope = scope;
			this->weaponScope.scopeClass = attachmentClass;
			this->weaponScope.currentEquippedScope->SetActorEnableCollision(false);
			this->weaponScope.currentEquippedScope->AttachToComponent(this->GetItemSkeletalMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Scope"));
			if (USkeletalMeshSocket* ironSightSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName("IronSight")))
				ironSightSocket->RelativeLocation = this->weaponScope.scopePosition;
		}
	} else if (attachment == EWeaponAttachment::EWA_Barrel) {
		if (!attachmentClass) {
			if (this->weaponBarrel.currentEquippedBarrel) this->weaponBarrel.currentEquippedBarrel->Destroy();
			this->weaponBarrel.barrelClass = nullptr;
			return;
		}
		if (this->weaponBarrel.currentEquippedBarrel) this->weaponBarrel.currentEquippedBarrel->Destroy();
		if (AActor* barrel = GetWorld()->SpawnActor<AActor>(attachmentClass, FVector::ZeroVector, FRotator::ZeroRotator)) {
			this->weaponBarrel.currentEquippedBarrel = barrel;
			this->weaponBarrel.barrelClass = attachmentClass;
			this->weaponBarrel.currentEquippedBarrel->SetActorEnableCollision(false);
			this->weaponBarrel.currentEquippedBarrel->AttachToComponent(this->GetItemSkeletalMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Muzzle"));
		}
	} else if (attachment == EWeaponAttachment::EWA_Grip) {
		if (!attachmentClass) {
			if (this->weaponGrip.currentEquippedGrip) this->weaponGrip.currentEquippedGrip->Destroy();
			if (USkeletalMeshSocket* leftHandPlacementSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName("LeftHandPlacement")))
				leftHandPlacementSocket->RelativeLocation = FVector(6.860041f, -5.002760f, 21.294327f);
			this->weaponGrip.gripClass = nullptr;
			return;
		}
		if (this->weaponGrip.currentEquippedGrip) this->weaponGrip.currentEquippedGrip->Destroy();
		if (AActor* grip = GetWorld()->SpawnActor<AActor>(attachmentClass, FVector::ZeroVector, FRotator::ZeroRotator)) {
			this->weaponGrip.currentEquippedGrip = grip;
			this->weaponGrip.gripClass = attachmentClass;
			this->weaponGrip.currentEquippedGrip->SetActorEnableCollision(false);
			this->weaponGrip.currentEquippedGrip->AttachToComponent(this->GetItemSkeletalMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Grip"));
			if (USkeletalMeshSocket* leftHandPlacementSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName("LeftHandPlacement")))
				leftHandPlacementSocket->RelativeLocation = this->weaponGrip.gripPosition;
		}
	}
}

