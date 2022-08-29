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
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay() {
	Super::BeginPlay();

	this->playerController = UGameplayStatics::GetPlayerController(this, 0);
	this->barrelSocket = this->GetItemMesh()->GetSocketByName("Muzzle");
	this->weaponStats.currentFiringMode = this->GetWeaponStats().availableFiringModes[0]; 

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

void AWeapon::RecoilUpdate() {
	if (!this->isShooting() || !this->playerController) return;
	this->AddRecoil();
}

void AWeapon::Shoot() {
	if (!this->barrelSocket) { GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: barrelSocket* is NULL")), false); return; }
	FTransform socketTransform = this->barrelSocket->GetSocketTransform(this->GetItemMesh());
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

