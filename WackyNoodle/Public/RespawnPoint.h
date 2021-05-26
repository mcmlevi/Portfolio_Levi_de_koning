// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Classes/GameFramework/PlayerStart.h"
#include "RespawnPoint.generated.h"

UCLASS()
class WACKYNOODLE_API ARespawnPoint : public APlayerStart
{
	GENERATED_BODY()
	
public:	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	


};
