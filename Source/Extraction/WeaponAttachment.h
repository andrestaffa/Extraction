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

public:
	FORCEINLINE EWeaponAttachment GetAttachmentType() const { return this->attachmentType; }
	FORCEINLINE bool GetIsRangedScope() const { return this->bIsRangedScope; }

};
