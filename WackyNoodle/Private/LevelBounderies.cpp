// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelBounderies.h"
#include "Components/BoxComponent.h"
// Sets default values
ALevelBounderies::ALevelBounderies()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LevelBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = LevelBounds;
	LevelBounds->SetBoxExtent({ 0.5f,0.5f,0.5f });
	LevelBounds->SetBoundsScale(100);
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ALevelBounderies::BeginPlay()
{
	Super::BeginPlay();
	
}

