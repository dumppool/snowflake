// Fill out your copyright notice in the Description page of Project Settings.

#include "Hello410.h"
#include "TestTL.h"

DEFINE_LOG_CATEGORY_STATIC(TestTL, Log, All)

// Sets default values
ATestTL::ATestTL()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Components...
	TLComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TLComponent"));
	//AddOwnedComponent(TLComp);
	
	//MoveInterpDelegate.BindRaw(this, &ATestTL::MoveClientActor);
	MoveInterpDelegate.BindUFunction(this, FName(TEXT("MoveClientActor")));
}

// Called when the game starts or when spawned
void ATestTL::BeginPlay()
{
	Super::BeginPlay();
	
	if (TLComp && MoveInterpCurve)
	{
		UE_LOG(TestTL, Log, TEXT("Begin Play ********%d*****%ls***"), 
			GetComponentsByClass(UTimelineComponent::StaticClass()).Num(),
			MoveInterpDelegate.IsBound()?TEXT("True"):TEXT("False"));
		TLComp->Activate(true);
		//TLComp->AddInterpFloat
		TLComp->AddInterpFloat(MoveInterpCurve, MoveInterpDelegate, FName("MoveInterp"));
		TLComp->PlayFromStart();
	}
}

// Called every frame
void ATestTL::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ATestTL::SetLocAnimCurve(UCurveFloat* FloatCurve)
{
	MoveInterpCurve = FloatCurve;
}

void ATestTL::SetColorAnimCurve(UCurveLinearColor* ColorCurve)
{
	ColorAnimCurve = ColorCurve;
}

void ATestTL::MoveClientActor(float Output)
{
	if (ClientActor)
	{
		FVector Loc = FMath::Lerp(StartLoc, StopLoc, Output);
		ClientActor->SetActorLocation(Loc, false, nullptr, ETeleportType::TeleportPhysics);
	}
}