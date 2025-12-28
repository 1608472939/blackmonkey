// Fill out your copyright notice in the Description page of Project Settings.


#include "hw_Actor.h"

#include "Black_Myth_homework/debug_black_myth.h"

// Sets default values
Ahw_Actor::Ahw_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void Ahw_Actor::BeginPlay()
{
	Super::BeginPlay();
	
	//UE_LOG(LogTemp, Warning, TEXT("Begin Play Called"));

	//UWorld* world = GetWorld();

	//FVector location = GetActorLocation();
	//FVector forward = GetActorForwardVector();
	//draw_sphere(location);
	////draw_line(location, location + forward * 100)
	//draw_vector(location, location + forward * 100);
}

// Called every frame
void Ahw_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);

	AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));
	draw_sphere_single(GetActorLocation());
	draw_vector_single(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100);

}

