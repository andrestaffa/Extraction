// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Weapon.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
ABullet::ABullet() : 
	velocity(0.0f), 
	bulletLifeSpan(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	this->RootComponent = this->rootComp;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay() {
	Super::BeginPlay();

	if (AWeapon* weapon = Cast<AWeapon>(this->GetOwner())) {
		this->velocity = weapon->BulletDirection() * weapon->GetWeaponStats().bulletSpeed;
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("[ABULLET]: AWeapon* is NULL")));
	}
}

// Called every frame
void ABullet::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FHitResult hitResult;
	FVector start = this->GetActorLocation();
	FVector end = start + (this->velocity * DeltaTime);
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility, params)) {
		if (this->impactParticles) 
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->impactParticles, hitResult.ImpactPoint);
		DrawDebugPoint(GetWorld(), hitResult.ImpactPoint, 10.0f, FColor::Red, false, 3.f);
		this->Destroy();
	} else {
		this->bulletLifeSpan += DeltaTime;
		DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 3.f);
		SetActorLocation(end);
		this->velocity += FVector(0.0f, 0.0f, -981.0f) * DeltaTime;
	}
	if (this->bulletLifeSpan > 10.0f) this->Destroy();
}

