// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponAttachment.h"
#include "Weapon.h"


AWeaponAttachment::AWeaponAttachment() :
    attachmentType(EWeaponAttachment::EWA_None),
    bIsRangedScope(false)
{
    PrimaryActorTick.bCanEverTick = this->bIsRangedScope;
}

void AWeaponAttachment::BeginPlay() {
    Super::BeginPlay();

    if (AWeapon* weapon = Cast<AWeapon>(this->GetOwner())) {
        FWeaponStats weaponStats = weapon->GetWeaponStats();
        weaponStats.fireRate *= this->attachmentConfig.fireRateMultiplier;
	    weaponStats.bulletSpeed *= this->attachmentConfig.bulletSpeedMultiplier;
	    weaponStats.verticalRecoil *= this->attachmentConfig.verticalRecoilMultiplier;
	    weaponStats.minHorizontalRecoil *= this->attachmentConfig.minHorizontalRecoilMultiplier;
	    weaponStats.maxHorizontalRecoil *= this->attachmentConfig.maxHorizontalRecoilMultiplier;
	    weaponStats.minBulletSpread *= this->attachmentConfig.minBulletSpreadMultiplier;
	    weaponStats.maxBulletSpread *= this->attachmentConfig.maxBulletSpreadMultiplier;
	    weaponStats.adsSpeed *= this->attachmentConfig.adsSpeedMultiplier;
	    weaponStats.blueprint_ADS_Speed *= this->attachmentConfig.blueprint_ADS_SpeedMultiplier;
        weapon->SetWeaponStats(weaponStats);
    }

    this->OnDestroyed.AddDynamic(this, &AWeaponAttachment::ActorDestroyed);
}

void AWeaponAttachment::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

void AWeaponAttachment::ActorDestroyed(AActor* Act) {
     if (AWeapon* weapon = Cast<AWeapon>(this->GetOwner())) {
        FWeaponStats weaponStats = weapon->GetWeaponStats();
        weaponStats.fireRate /= this->attachmentConfig.fireRateMultiplier;
	    weaponStats.bulletSpeed /= this->attachmentConfig.bulletSpeedMultiplier;
	    weaponStats.verticalRecoil /= this->attachmentConfig.verticalRecoilMultiplier;
	    weaponStats.minHorizontalRecoil /= this->attachmentConfig.minHorizontalRecoilMultiplier;
	    weaponStats.maxHorizontalRecoil /= this->attachmentConfig.maxHorizontalRecoilMultiplier;
	    weaponStats.minBulletSpread /= this->attachmentConfig.minBulletSpreadMultiplier;
	    weaponStats.maxBulletSpread /= this->attachmentConfig.maxBulletSpreadMultiplier;
	    weaponStats.adsSpeed /= this->attachmentConfig.adsSpeedMultiplier;
	    weaponStats.blueprint_ADS_Speed /= this->attachmentConfig.blueprint_ADS_SpeedMultiplier;
        weapon->SetWeaponStats(weaponStats);
    }
}   