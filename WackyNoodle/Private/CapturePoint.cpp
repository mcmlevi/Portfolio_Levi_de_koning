// Fill out your copyright notice in the Description page of Project Settings.


#include "CapturePoint.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "WackyNoodle/WackyNoodleCharacter.h"
// Sets default values
ACapturePoint::ACapturePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerCaptureZone"));
	RootComponent = Zone;

	Zone->OnComponentBeginOverlap.AddDynamic(this, &ACapturePoint::OverlapBegin);
	Zone->OnComponentEndOverlap.AddDynamic(this, &ACapturePoint::OverlapEnd);
}

// Called when the game starts or when spawned
void ACapturePoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACapturePoint::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority() == false)
		return;

	AWackyNoodleCharacter* PlayerEntered = Cast<AWackyNoodleCharacter>(OtherActor);
	if(PlayerEntered)
	{
		TeamMembersOnPoint[PlayerEntered->PlayerTeam]++;
	}
}

void ACapturePoint::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (HasAuthority() == false)
		return;

	AWackyNoodleCharacter* PlayerEntered = Cast<AWackyNoodleCharacter>(OtherActor);
	if (PlayerEntered)
	{
		TeamMembersOnPoint[PlayerEntered->PlayerTeam]--;
	}
}

// Called every frame
void ACapturePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority() == false)
		return;

	
	float mod = { TeamMembersOnPoint[0] > TeamMembersOnPoint[1] ? 1.f : -1.f };
	if (TeamMembersOnPoint[0] == TeamMembersOnPoint[1])
		mod = 0.f;
	
	CurrentCaptureStatus +=  ((CaptureTime * DeltaTime)/100.f) * mod;
	CurrentCaptureStatus = FMath::Clamp(CurrentCaptureStatus, -100.f, 100.f);

	OwningTeam = CurrentCaptureStatus <= -99.f ? 0: CurrentCaptureStatus >= 99.f ? 1 : -1;

}



void ACapturePoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACapturePoint, CurrentCaptureStatus);
	DOREPLIFETIME(ACapturePoint, TeamMembersOnPoint);
	DOREPLIFETIME(ACapturePoint, OwningTeam);
}
