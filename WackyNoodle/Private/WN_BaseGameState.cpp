// Fill out your copyright notice in the Description page of Project Settings.


#include "WN_BaseGameState.h"




#include "Net/UnrealNetwork.h"




void AWN_BaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWN_BaseGameState, timeLeft);
	DOREPLIFETIME(AWN_BaseGameState, Scores);
}

void AWN_BaseGameState::sortSet()
{
	Scores.Sort([](const FScore& a, const FScore& b) {return a.Points > b.Points; });

}

void AWN_BaseGameState::Tick(float DeltaSeconds)
{

}
