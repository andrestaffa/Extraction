// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponAttachment.h"
#include "Weapon.h"


AWeaponAttachment::AWeaponAttachment() :
    attachmentType(EWeaponAttachment::EWA_None),
    bIsRangedScope(false)
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWeaponAttachment::BeginPlay() {
    Super::BeginPlay();

}

void AWeaponAttachment::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}
