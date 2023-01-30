// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	this->RootComponent = this->root;

	this->itemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	this->itemSkeletalMesh->SetupAttachment(this->root);

	this->itemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Static Mesh"));
	this->itemStaticMesh->SetupAttachment(this->root);
}

// Called when the game starts or when spawned
void AItem::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

