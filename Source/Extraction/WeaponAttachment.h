// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "WeaponAttachment.generated.h"

UENUM(BlueprintType)
enum class EWeaponAttachment : uint8 {
	EWA_Scope UMETA(DisplayName = "Scope"),
	EWA_Barrel UMETA(DisplayName = "Barrel"),
	EWA_Grip UMETA(DisplayName = "Grip"),
	EWA_None UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType)
struct FWeaponAttachmentConfig 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float fireRateMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float bulletSpeedMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float verticalRecoilMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float minHorizontalRecoilMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxHorizontalRecoilMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float minBulletSpreadMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxBulletSpreadMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float adsSpeedMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float blueprint_ADS_SpeedMultiplier = 1.0;	

};


UCLASS()
class EXTRACTION_API AWeaponAttachment : public AItem
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	AWeaponAttachment();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment Statistics", meta = (AllowPrivateAccess = "true"))
	EWeaponAttachment attachmentType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment Statistics", meta = (AllowPrivateAccess = "true"))
	bool bIsRangedScope;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment Statistics", meta = (AllowPrivateAccess = "true"))
	FWeaponAttachmentConfig attachmentConfig;

private:
	UFUNCTION()
	void ActorDestroyed(AActor* Act);
	
public:
	FORCEINLINE EWeaponAttachment GetAttachmentType() const { return this->attachmentType; }
	FORCEINLINE bool GetIsRangedScope() const { return this->bIsRangedScope; }

};
