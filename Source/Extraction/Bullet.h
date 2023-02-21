// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

USTRUCT(BlueprintType)
struct FBulletProperties {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UParticleSystem* impactParticles;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float bulletLifeTime = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector velocity = FVector(0.0f, 0.0f, 0.0f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float bulletLifeSpan = 0.0f;

};

UCLASS()
class EXTRACTION_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* rootComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	FBulletProperties bulletProperties;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	class AWeapon* weapon;



private:
	void NullChecks();
	
};
