// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class EXTRACTION_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* itemSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* itemStaticMesh;


// MARK: - Getters and Setters
public:
	FORCEINLINE USkeletalMeshComponent* GetItemSkeletalMesh() const { return this->itemSkeletalMesh; }
	FORCEINLINE UStaticMeshComponent* GetItemStaticMesh() const { return this->itemStaticMesh; }

};
