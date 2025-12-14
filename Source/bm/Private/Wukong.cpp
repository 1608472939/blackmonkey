// Fill out your copyright notice in the Description page of Project Settings.


#include "Wukong.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AWukong::AWukong()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);
}

// Called when the game starts or when spawned
void AWukong::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWukong::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWukong::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("Turn"), this, &AWukong::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AWukong::LookUp);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AWukong::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AWukong::MoveRight);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
}

void AWukong::MoveForward(float value) {
	if (Controller && (value != 0.f)) {
		//find forward
		//FVector Forward = GetActorForwardVector();
		///AddMovementInput(Forward, value);

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void AWukong::MoveRight(float value) {
	if (Controller && (value != 0.f)) {
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}

void AWukong::Turn(float value) {
	AddControllerYawInput(value);
}

void AWukong::LookUp(float value) {
	AddControllerPitchInput(value);
}