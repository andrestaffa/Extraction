// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Bullet.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Camera/CameraShakeBase.h"

// Sets default values
AWeapon::AWeapon() :
	bCanShoot(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->RecoilUpdate();
}

void AWeapon::RecoilUpdate() {
	if (!this->bCanShoot || !UGameplayStatics::GetPlayerController(this, 0)) return;
	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
	controller->AddPitchInput(-this->weaponStats.verticalRecoil * GetWorld()->GetDeltaSeconds());
	int rand = FMath::RandRange(0, 1);
	float yaw = (rand == 1) ? FMath::RandRange(this->weaponStats.minHorizontalRecoil, this->weaponStats.maxHorizontalRecoil) : FMath::RandRange(-this->weaponStats.maxHorizontalRecoil, -this->weaponStats.minHorizontalRecoil);
	controller->AddYawInput(yaw * GetWorld()->GetDeltaSeconds());
}

void AWeapon::Shoot() {
	const USkeletalMeshSocket* barrelSocket = this->GetItemMesh()->GetSocketByName("Muzzle");
	FTransform socketTransform = barrelSocket->GetSocketTransform(this->GetItemMesh());
	FActorSpawnParameters params;
	params.Owner = this;
	GetWorld()->SpawnActor<ABullet>((this->bulletClass) ? this->bulletClass : ABullet::StaticClass(), socketTransform.GetLocation(), this->GetActorForwardVector().Rotation(), params);
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->recoilCameraShakeClass);
	this->bCanShoot = true;
	FTimerHandle autoFireTimerHandle;
	GetWorldTimerManager().SetTimer(autoFireTimerHandle, [&](){
		if (this->bCanShoot) this->Shoot();
	}, this->weaponStats.fireRate, false);
}

void AWeapon::StopShooting() {
	this->bCanShoot = false;
}

const FVector AWeapon::BulletDirection() {
	FVector forwardVector = this->GetActorForwardVector() * -1;
	float multiplier = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(1.0f, 0.0f), this->adsValue);
	float minBulletSpread = this->weaponStats.minBulletSpread * multiplier;
	float maxBulletSpread = this->weaponStats.maxBulletSpread * multiplier;
	float x = forwardVector.X + FMath::RandRange(minBulletSpread, maxBulletSpread);
	float y = forwardVector.Y + FMath::RandRange(minBulletSpread, maxBulletSpread);
	float z = forwardVector.Z + FMath::RandRange(minBulletSpread, maxBulletSpread);
	return FVector(x, y, z);
}

