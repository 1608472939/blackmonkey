// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "hw_Actor.generated.h"

UCLASS()
class BLACK_MYTH_HOMEWORK_API Ahw_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Ahw_Actor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	float RunningTime;
	float Amplitude = 0.25f;
	float TimeConstant = 5.0f;
};
