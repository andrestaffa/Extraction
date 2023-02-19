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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float blueprint_ADS_Speed = 0.3f;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsClipping = false;
};

USTRUCT(BlueprintType)
struct FClippingSettings 
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector intialClipPostion = FVector(0.0f, 0.0f, 0.0f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector targetGunClipPostion = FVector(0.0f, 0.0f, 0.0f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float normalizedDistanceRange = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float clippingLerpValue = 0.0f;
};

USTRUCT(BlueprintType)
struct FWeaponScope 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AWeaponAttachment> attachmentClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector scopePosition = FVector(0.0f, -17.2f, 0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector opticalScopePosition = FVector(0.0f, -16.2f, 0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector defaultScopePosition = FVector(0.0f, -14.2f, 0.0f);
};

USTRUCT(BlueprintType)
struct FWeaponBarrel 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AWeaponAttachment> attachmentClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector barrelPosition = FVector(0.0f, 0.0f, 0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector defaultbarrelPosition = FVector(0.0f, 0.0f, 0.0f);
};

USTRUCT(BlueprintType)
struct FWeaponGrip 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AWeaponAttachment> attachmentClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector gripPosition = FVector(9.0f, 0.0f, 28.0f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector defaultGripPosition = FVector(6.860041f, -5.002760f, 21.294327f);
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
	
	class APlayerController* playerController;
	class AFPSCharacter* playerCharacter;
	const class USkeletalMeshSocket* barrelSocket;

	// Stats
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABullet> bulletClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCameraShakeBase> recoilCameraShakeClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	FWeaponStats weaponStats;
	
	// Clipping
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clipping", meta = (AllowPrivateAccess = "true"))
	FClippingSettings clippingSettings;

	// Attachments
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Attachments", meta = (AllowPrivateAccess = "true"))
	FWeaponScope scope;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Attachments", meta = (AllowPrivateAccess = "true"))
	FWeaponBarrel barrel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Attachments", meta = (AllowPrivateAccess = "true"))
	FWeaponGrip grip;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Attachments", meta = (AllowPrivateAccess = "true"))
	TSet<class AWeaponAttachment*> attachments;
	
public:
	void Shoot();
	void StopShooting();
	const FVector BulletDirection();

	void SetAttachment(class AWeaponAttachment* attachment);
	void RemoveAttacment(class AWeaponAttachment* attachment);

private:
	void NullChecks();

	void RecoilUpdate();
	void AddRecoil();
	void DetectClipping();
	void FireSingle(FTransform& socketTransform, FActorSpawnParameters& params);

	void SetDefaultSocketLocations();
	void SetAttachment(TSubclassOf<class AWeaponAttachment> attachmentClass);
	void SetDefaultAttachments();
	class AWeaponAttachment* HasAttachment(class AWeaponAttachment* other);

// MARK: - Getters and Setters
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FWeaponStats GetWeaponStats() const { return this->weaponStats; }
	FORCEINLINE void SetWeaponStats(FWeaponStats stats) { this->weaponStats = stats; }

	FORCEINLINE TSet<class AWeaponAttachment*> GetWeaponAttachments() const { return this->attachments; }

	FORCEINLINE bool isShooting() const { return this->weaponStats.isShootingFullAuto || !this->weaponStats.bCanShootSingle || this->weaponStats.isShootingBurst; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetADSValue(float value) { this->weaponStats.adsValue = value; }
	void ChangeFiringMode();

	UFUNCTION(BlueprintCallable)
	bool hasGripAttachment() const;

private:
	UFUNCTION()
	void ActorDestroyed(AActor* Act);

};
