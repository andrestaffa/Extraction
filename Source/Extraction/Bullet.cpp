// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Weapon.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	this->RootComponent = this->rootComp;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay() {
	Super::BeginPlay();

	if (AWeapon* ownerWeapon = Cast<AWeapon>(this->GetOwner())) {
		this->weapon = ownerWeapon;
		this->bulletProperties.velocity = this->weapon->BulletDirection() * this->weapon->GetWeaponStats().bulletSpeed;
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[ABULLET]: AWeapon* is NULL")), false);
	}

	this->NullChecks();
}

void ABullet::NullChecks() {
	if (!this->rootComp) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[ABULLET]: rootComp* is NULL")), false);
	if (!this->weapon) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[ABULLET]: weapon* is NULL")), false);
	if (!this->bulletProperties.impactParticles) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[ABULLET]: impactParticles* is NULL")), false);
}

// Called every frame
void ABullet::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FHitResult hitResult;
	FVector start = this->GetActorLocation();
	FVector end = start + (this->bulletProperties.velocity * DeltaTime);
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility, params)) {
		if (this->bulletProperties.impactParticles && this->weapon) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->bulletProperties.impactParticles, hitResult.ImpactPoint, this->weapon->GetActorForwardVector().Rotation());
		this->Destroy();
	} else {
		this->bulletProperties.bulletLifeSpan += DeltaTime;;
		SetActorLocation(end);
		this->bulletProperties.velocity += FVector(0.0f, 0.0f, -981.0f) * DeltaTime;
	}
	if (this->bulletProperties.bulletLifeSpan > this->bulletProperties.bulletLifeTime) this->Destroy();
}

