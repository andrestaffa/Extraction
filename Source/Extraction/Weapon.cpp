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
	if (!this->weaponStats.bCanShoot || !this->playerController) return;
	this->playerController->AddPitchInput(-this->weaponStats.verticalRecoil * GetWorld()->GetDeltaSeconds());
	int rand = FMath::RandRange(0, 1);
	float yaw = (rand == 1) ? FMath::RandRange(this->weaponStats.minHorizontalRecoil, this->weaponStats.maxHorizontalRecoil) : FMath::RandRange(-this->weaponStats.maxHorizontalRecoil, -this->weaponStats.minHorizontalRecoil);
	this->playerController->AddYawInput(yaw * GetWorld()->GetDeltaSeconds());
}

void AWeapon::Shoot() {
	if (!this->barrelSocket) {
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: barrelSocket* is NULL")), false);
		return;
	}
	FTransform socketTransform = this->barrelSocket->GetSocketTransform(this->GetItemMesh());
	FActorSpawnParameters params;
	params.Owner = this;
	GetWorld()->SpawnActor<ABullet>((this->bulletClass) ? this->bulletClass : ABullet::StaticClass(), socketTransform.GetLocation(), this->GetActorForwardVector().Rotation(), params);
	if (this->recoilCameraShakeClass) GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->recoilCameraShakeClass);
	this->weaponStats.bCanShoot = true;
	FTimerHandle autoFireTimerHandle;
	GetWorldTimerManager().SetTimer(autoFireTimerHandle, [&](){
		if (this->weaponStats.bCanShoot) this->Shoot();
	}, this->weaponStats.fireRate, false);
}

void AWeapon::StopShooting() {
	this->weaponStats.bCanShoot = false;
}

const FVector AWeapon::BulletDirection() {
	FVector forwardVector = this->GetActorForwardVector() * -1;
	float multiplier = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(1.0f, 0.0f), this->weaponStats.adsValue);
	float minBulletSpread = this->weaponStats.minBulletSpread * multiplier;
	float maxBulletSpread = this->weaponStats.maxBulletSpread * multiplier;
	float x = forwardVector.X + FMath::RandRange(minBulletSpread, maxBulletSpread);
	float y = forwardVector.Y + FMath::RandRange(minBulletSpread, maxBulletSpread);
	float z = forwardVector.Z + FMath::RandRange(minBulletSpread, maxBulletSpread);
	return FVector(x, y, z);
}

