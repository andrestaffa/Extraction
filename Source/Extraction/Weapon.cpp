// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Bullet.h"
#include "FPSCharacter.h"
#include "WeaponAttachment.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Camera/CameraShakeBase.h"
#include "Particles/ParticleSystemComponent.h"

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
	this->playerCharacter = Cast<AFPSCharacter>(this->GetOwner());
	this->barrelSocket = this->GetItemSkeletalMesh()->GetSocketByName(FName("Muzzle"));
	this->weaponStats.currentFiringMode = this->GetWeaponStats().availableFiringModes[0];

	// Delete these when I replace the MAC-11, Benelli M3 and Kar98k
	this->GetItemSkeletalMesh()->HideBoneByName(FName("b_gun_rem01"), EPhysBodyOp::PBO_None);
	this->GetItemSkeletalMesh()->HideBoneByName(FName("bullet"), EPhysBodyOp::PBO_None);
	this->GetItemSkeletalMesh()->HideBoneByName(FName("bullet-2"), EPhysBodyOp::PBO_None);
	this->GetItemSkeletalMesh()->HideBoneByName(FName("bullet-3"), EPhysBodyOp::PBO_None);

	this->NullChecks();
	this->SetDefaultSocketLocations();
	this->SetDefaultAttachments();

	this->OnDestroyed.AddDynamic(this, &AWeapon::ActorDestroyed);

	if (!this->playerCharacter || !this->playerCharacter) return;
	FVector rightHandVector = (this->weaponType == EWeaponType::EWT_Sniper) ? FVector(-25.0f, 0.0f, 10.0f) : FVector(25.0f, 0.0f, 0.0f);
	this->clippingSettings.intialClipPostion = this->rightHandEffectorLocation;
	this->clippingSettings.targetGunClipPostion = this->rightHandEffectorLocation + rightHandVector;
}

// Called every frame
void AWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	this->RecoilUpdate();
	this->DetectClipping();
}

void AWeapon::NullChecks() {
	if (!this->playerController) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: playerController* is NULL")), false);
	if (!this->playerCharacter) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: playerCharacter* is NULL")), false);
	if (!this->barrelSocket) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: barrelSocket* is NULL")), false);
	if (!this->bulletClass) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: bulletClass* is NULL")), false);
	if (!this->recoilCameraShakeClass) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: recoilCameraShakeClass* is NULL")), false);
	if (!this->particleSystems.muzzleFlashParticleSystem) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: muzzleFlashParticleSystem* is NULL")), false);
}

void AWeapon::SetDefaultSocketLocations() {
	if (USkeletalMeshSocket* ironSightSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName(FName("IronSight")))) 
		ironSightSocket->RelativeLocation = this->scope.defaultScopePosition;
	if (USkeletalMeshSocket* leftHandPlacementSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName(FName("LeftHandPlacement"))))
		leftHandPlacementSocket->RelativeLocation = this->grip.defaultGripPosition;
}

void AWeapon::SetDefaultAttachments() {
	if (this->scope.attachmentClass) this->SetAttachment(this->scope.attachmentClass);
	if (this->barrel.attachmentClass) this->SetAttachment(this->barrel.attachmentClass);
	if (this->grip.attachmentClass) this->SetAttachment(this->grip.attachmentClass);
}

void AWeapon::RecoilUpdate() {
	if (!this->IsShooting() || (!this->playerCharacter || !this->playerCharacter)) return;
	this->AddRecoil();
}

void AWeapon::DetectClipping() {
	if (!this->playerCharacter || !this->playerCharacter) return;
	FTransform socketTransform = this->barrelSocket->GetSocketTransform(this->GetItemSkeletalMesh());
	FVector start = socketTransform.GetLocation();
	FVector end = start + (this->GetActorRotation().Vector() * 55.0f * -1.0f);
	FHitResult hitResult;
	if (this->GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility)) {
		float distance = (hitResult.ImpactPoint - start).Length();
		this->clippingSettings.normalizedDistanceRange = 1.0f - ((distance - 0.0f) / (55.0f - 0.0f));
		this->clippingSettings.clippingLerpValue = FMath::FInterpTo(this->clippingSettings.clippingLerpValue, this->clippingSettings.normalizedDistanceRange, this->GetWorld()->GetDeltaSeconds(), 10.5f);
		FVector targetLerp = FMath::Lerp<FVector>(this->clippingSettings.intialClipPostion, this->clippingSettings.targetGunClipPostion, this->clippingSettings.clippingLerpValue);
		this->rightHandEffectorLocation = targetLerp;
		this->weaponStats.bIsClipping = true;
		this->playerCharacter->GetMovementSettings().ADSEnabled = false;
		if (this->playerCharacter->GetMovementSettings().isReloading)
			this->playerCharacter->SetActorLocation(this->playerCharacter->GetActorLocation() - this->playerCharacter->GetActorForwardVector());
	} else {
		this->clippingSettings.clippingLerpValue = FMath::FInterpTo(this->clippingSettings.clippingLerpValue, 0.0f, GetWorld()->GetDeltaSeconds(), 10.5f);
		FVector initialLerp = FMath::Lerp<FVector>(this->clippingSettings.intialClipPostion, this->clippingSettings.targetGunClipPostion, this->clippingSettings.clippingLerpValue);
		this->rightHandEffectorLocation = initialLerp;
		this->weaponStats.bIsClipping = false;
	}
}

void AWeapon::Shoot() {
	if (!this->barrelSocket) { GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("[AWeapon]: barrelSocket* is NULL")), false); return; }
	FTransform socketTransform = this->barrelSocket->GetSocketTransform(this->GetItemSkeletalMesh());
	this->PlayParticleSystem(this->particleSystems.muzzleFlashParticleSystem, socketTransform.GetLocation(), this->GetActorForwardVector().Rotation());
	FActorSpawnParameters params;
	params.Owner = this;
	FTimerHandle fireTimerHandle;
	if (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_FullAuto) {
		this->weaponStats.isShootingFullAuto = true;
		GetWorld()->SpawnActor<ABullet>((this->bulletClass) ? this->bulletClass : ABullet::StaticClass(), socketTransform.GetLocation(), this->GetActorForwardVector().Rotation(), params);
		if (this->recoilCameraShakeClass) GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->recoilCameraShakeClass);
		GetWorldTimerManager().SetTimer(fireTimerHandle, [&](){
			if (this->GetWeaponStats().isShootingFullAuto) this->Shoot();
		}, this->GetWeaponStats().fireRate, false);
	} else if (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_Single) {
		if (!this->GetWeaponStats().bCanShootSingle) return;
		this->weaponStats.bCanShootSingle = false;
		this->FireSingle(socketTransform, params);
		GetWorldTimerManager().SetTimer(fireTimerHandle, [&](){
			this->weaponStats.bCanShootSingle = true;
		}, this->GetWeaponStats().fireRate, false);
	} else if (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_Burst) {
		if (this->GetWeaponStats().bursts == 3) {
			this->weaponStats.isShootingBurst = false;
			this->weaponStats.bursts = 0;
			return;
		}
		this->weaponStats.isShootingBurst = true;
		this->weaponStats.bursts++;
		this->FireSingle(socketTransform, params);
		GetWorldTimerManager().SetTimer(fireTimerHandle, [&](){
			if (this->GetWeaponStats().isShootingBurst) this->Shoot();
		}, this->GetWeaponStats().fireRate, false);
	}
}

void AWeapon::StopShooting() {
	this->weaponStats.isShootingFullAuto = false;
}

void AWeapon::AddRecoil() {
	float verticalMult = (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_FullAuto) ? 1.0f : 1.0f;
	float horizontalMult = (this->GetWeaponStats().currentFiringMode == EFireMode::EFM_FullAuto) ? 1.0f: 1.0f;
	this->playerController->AddPitchInput(-this->GetWeaponStats().verticalRecoil * GetWorld()->GetDeltaSeconds() * verticalMult);
	int rand = FMath::RandRange(0, 1);
	float yaw = (rand == 1) ? FMath::RandRange(this->GetWeaponStats().minHorizontalRecoil * horizontalMult, this->GetWeaponStats().maxHorizontalRecoil * horizontalMult) : FMath::RandRange(-this->GetWeaponStats().maxHorizontalRecoil * horizontalMult, -this->GetWeaponStats().minHorizontalRecoil * horizontalMult);
	this->playerController->AddYawInput(yaw * GetWorld()->GetDeltaSeconds());	
}

void AWeapon::FireSingle(FTransform& socketTransform, FActorSpawnParameters& params) {
	GetWorld()->SpawnActor<ABullet>((this->bulletClass) ? this->bulletClass : ABullet::StaticClass(), socketTransform.GetLocation(), this->GetActorForwardVector().Rotation(), params);
	if (this->recoilCameraShakeClass) GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(this->recoilCameraShakeClass);
}

const FVector AWeapon::BulletDirection() {
	FVector forwardVector = this->GetActorForwardVector() * -1;
	float multiplier = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(1.0f, 0.0f), this->GetWeaponStats().adsValue);
	float minBulletSpread = this->GetWeaponStats().minBulletSpread * multiplier;
	float maxBulletSpread = this->GetWeaponStats().maxBulletSpread * multiplier;
	float x = forwardVector.X + FMath::RandRange(minBulletSpread, maxBulletSpread);
	float y = forwardVector.Y + FMath::RandRange(minBulletSpread, maxBulletSpread);
	float z = forwardVector.Z + FMath::RandRange(minBulletSpread, maxBulletSpread);
	return FVector(x, y, z);
}

void AWeapon::ChangeFiringMode() {
	if (this->GetWeaponStats().availableFiringModes[0] == this->GetWeaponStats().availableFiringModes[1]) return;
	bool cond = this->GetWeaponStats().currentFiringMode == this->GetWeaponStats().availableFiringModes[0];
	this->weaponStats.currentFiringMode = (cond) ? this->GetWeaponStats().availableFiringModes[1] : this->GetWeaponStats().availableFiringModes[0];
}

void AWeapon::SetAttachment(TSubclassOf<class AWeaponAttachment> attachmentClass) {
	if (!attachmentClass) return;
	FActorSpawnParameters params;
	params.Owner = this;
	if (AWeaponAttachment* spawnedAttachment = GetWorld()->SpawnActor<AWeaponAttachment>(attachmentClass, FVector::ZeroVector, FRotator::ZeroRotator, params)) {
		spawnedAttachment->SetActorEnableCollision(false);
		if (spawnedAttachment->GetAttachmentType() == EWeaponAttachment::EWA_Scope) {
			spawnedAttachment->AttachToComponent(this->GetItemSkeletalMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Scope"));
			if (USkeletalMeshSocket* ironSightSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName("IronSight"))) {
				ironSightSocket->RelativeLocation = (spawnedAttachment->GetIsRangedScope()) ? this->scope.opticalScopePosition : this->scope.scopePosition;
			}
		} else if (spawnedAttachment->GetAttachmentType() == EWeaponAttachment::EWA_Barrel) {
			spawnedAttachment->AttachToComponent(this->GetItemSkeletalMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Muzzle"));
		} else if (spawnedAttachment->GetAttachmentType() == EWeaponAttachment::EWA_Grip) {
			spawnedAttachment->AttachToComponent(this->GetItemSkeletalMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Grip"));
			if (USkeletalMeshSocket* leftHandPlacementSocket = const_cast<USkeletalMeshSocket*>(this->GetItemSkeletalMesh()->GetSocketByName(FName("LeftHandPlacement")))) {
				leftHandPlacementSocket->RelativeLocation = this->grip.gripPosition;
			}
		}
		this->attachments.Add(spawnedAttachment);
	}
}

void AWeapon::SetAttachment(AWeaponAttachment* attachment) {
	if (!attachment) return;
	UClass* attachmentClass = attachment->GetClass();
	TSubclassOf<AWeaponAttachment> attachmentSubClass = TSoftClassPtr<AWeaponAttachment>(attachmentClass).Get();
	if (AWeaponAttachment* a = this->HasAttachment(attachment))
		this->RemoveAttacment(a);
	this->SetAttachment(attachmentSubClass);
}

void AWeapon::RemoveAttacment(AWeaponAttachment* attachment) {
	if (!attachment || this->attachments.IsEmpty()) return;
	this->attachments.Remove(attachment);
	if (attachment->GetAttachmentType() == EWeaponAttachment::EWA_Scope) {
		if (USkeletalMeshSocket *ironSightSocket = const_cast<USkeletalMeshSocket *>(this->GetItemSkeletalMesh()->GetSocketByName(FName("IronSight")))) ironSightSocket->RelativeLocation = this->scope.defaultScopePosition;
	} else if (attachment->GetAttachmentType() == EWeaponAttachment::EWA_Grip) {
		if (USkeletalMeshSocket *leftHandPlacementSocket = const_cast<USkeletalMeshSocket *>(this->GetItemSkeletalMesh()->GetSocketByName(FName("LeftHandPlacement")))) leftHandPlacementSocket->RelativeLocation = this->grip.defaultGripPosition;
	}
	attachment->Destroy();
}

AWeaponAttachment* AWeapon::HasAttachment(AWeaponAttachment* other) {
	if (!other) return nullptr;
	for (AWeaponAttachment* attachment : this->attachments)
		if (attachment->GetAttachmentType() == other->GetAttachmentType()) return attachment;
	return nullptr;
}

bool AWeapon::HasGripAttachment() const {
	for (const AWeaponAttachment* attachment : this->attachments) {
		if (attachment->GetAttachmentType() == EWeaponAttachment::EWA_Grip) return true;
	}
	return false;
}

void AWeapon::ActorDestroyed(AActor* Act) {
	for (AWeaponAttachment* attachment : this->attachments) {
		attachment->Destroy();
	}
}

void AWeapon::PlayParticleSystem(UParticleSystem* particleSystem, FVector location, FRotator rotation) {
	if (!particleSystem) return;
	bool flag = false;
	for (const AWeaponAttachment* attachment : this->attachments) {
		if (attachment->GetAttachmentType() == EWeaponAttachment::EWA_Barrel) { flag = true; break; }
	}
	FVector newPos = (flag) ? location + (this->GetActorForwardVector() * -22.5f) : location;
	UParticleSystemComponent* spawnedParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(this->GetWorld(), particleSystem, newPos, rotation);
	spawnedParticleSystem->SetWorldScale3D(FVector(0.20f, 0.20f, 0.20f));
}
