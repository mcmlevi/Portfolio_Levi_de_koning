// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThePoolNoodle.generated.h"

UCLASS()
class WACKYNOODLE_API AThePoolNoodle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThePoolNoodle();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	AActor* ThePlayer;



};
