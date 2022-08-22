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

	this->itemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	this->itemMesh->SetupAttachment(this->root);

}

// Called when the game starts or when spawned
void AItem::BeginPlay() {
	Super::BeginPlay();

	this->Character = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	
}

// Called every frame
void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

