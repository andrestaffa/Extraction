// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8 {
	EFM_FullAuto UMETA(DisplayName = "Full Auto"),
	EFM_Single UMETA(DisplayName = "Single"),
	EFM_Burst UMETA(DisplayName = "Burst"),
	EFM_MAX UMETA(DisplayName = "DefaultMAX")
};


USTRUCT(BlueprintType)
struct FWeaponStats 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float fireRate = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float bulletSpeed = 350.0f * 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float verticalRecoil = 8.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float minHorizontalRecoil = 2.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxHorizontalRecoil = 6.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float minBulletSpread = -0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxBulletSpread = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float adsSpeed = 7.0f;
	UPROPERTY(EditAnywhere)
	EFireMode availableFiringModes[2] = { EFireMode::EFM_FullAuto, EFireMode::EFM_Single };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EFireMode currentFiringMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isShootingFullAuto = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isShootingBurst = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCanShootSingle = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int bursts = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float adsValue = 0.0f;
};

UCLASS()
class EXTRACTION_API AWeapon : public AItem
{
	GENERATED_BODY()

	public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABullet> bulletClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCameraShakeBase> recoilCameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	FWeaponStats weaponStats;

	class APlayerController* playerController;
	const class USkeletalMeshSocket* barrelSocket;

public:
	void Shoot();
	void StopShooting();
	const FVector BulletDirection();

private:
	void NullChecks();

	void RecoilUpdate();
	void AddRecoil();
	void FireSingle(FTransform& socketTransform, FActorSpawnParameters& params);

// MARK: - Getters and Setters
public:
	FORCEINLINE FWeaponStats GetWeaponStats() const { return this->weaponStats; }
	FORCEINLINE bool isShooting() const { return this->weaponStats.isShootingFullAuto || !this->weaponStats.bCanShootSingle || this->weaponStats.isShootingBurst; }

	FORCEINLINE void SetADSValue(float value) { this->weaponStats.adsValue = value; }
	void ChangeFiringMode();

};
