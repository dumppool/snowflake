// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TestTL.generated.h"

UCLASS()
class HELLO410_API ATestTL : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestTL();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category="TestTL")
		void SetLocAnimCurve(UCurveFloat* FloatCurve);

	UFUNCTION(BlueprintCallable, Category = "TestTL")
		void SetColorAnimCurve(UCurveLinearColor* ColorCurve);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCurveFloat* MoveInterpCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCurveLinearColor* ColorAnimCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* ClientActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector StartLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector StopLoc;

	UFUNCTION(BlueprintCallable, Category="TestTL")
	void MoveClientActor(float Output);

protected:

	UTimelineComponent* TLComp;
	UCameraComponent* CamComp;

	FOnTimelineFloat MoveInterpDelegate;
};
