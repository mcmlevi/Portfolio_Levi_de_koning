// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CapturePoint.generated.h"

class UBoxComponent;

UCLASS()
class WACKYNOODLE_API ACapturePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACapturePoint();
	UPROPERTY(BlueprintReadOnly, Replicated)
		float CurrentCaptureStatus = 0.f;
	UPROPERTY(BlueprintReadOnly, Replicated)
		TArray<int> TeamMembersOnPoint{ 0,0 };
	UPROPERTY(BlueprintReadOnly, Replicated)
		int OwningTeam = -1;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	UBoxComponent* Zone;
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Time In MS for the point to switch side
	UPROPERTY(EditAnywhere)
	float CaptureTime = 1000.f;


	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
};
