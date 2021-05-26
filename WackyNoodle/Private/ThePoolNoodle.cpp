// Fill out your copyright notice in the Description page of Project Settings.


#include "ThePoolNoodle.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
// Sets default values
AThePoolNoodle::AThePoolNoodle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AThePoolNoodle::BeginPlay()
{
	Super::BeginPlay();
	ThePlayer = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (ThePlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("ThePlayer isn't null"));
	}
	else 
		UE_LOG(LogTemp, Error, TEXT("ThePlayer is null"));

}

// Called every frame
void AThePoolNoodle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

