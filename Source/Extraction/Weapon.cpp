// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Bullet.h"

#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AWeapon::AWeapon() :
	bCanShoot(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AWeapon::Shoot() {
	const USkeletalMeshSocket* barrelSocket = this->GetItemMesh()->GetSocketByName("Muzzle");
	FTransform socketTransform = barrelSocket->GetSocketTransform(this->GetItemMesh());
	FActorSpawnParameters params;
	params.Owner = this;
	GetWorld()->SpawnActor<ABullet>((this->bulletClass) ? this->bulletClass : ABullet::StaticClass(), socketTransform.GetLocation(), this->GetActorRotation(), params);
	this->bCanShoot = true;
	FTimerHandle autoFireTimerHandle;
	GetWorldTimerManager().SetTimer(autoFireTimerHandle, [&](){
		if (this->bCanShoot) this->Shoot();
	}, this->weaponStats.fireRate, false);
}

void AWeapon::StopShooting() {
	this->bCanShoot = false;
}