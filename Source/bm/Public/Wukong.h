// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Wukong.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class BM_API AWukong : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWukong();

    virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Turn(float value);
	void LookUp(float value);

private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	void MoveForward(float value);
	void MoveRight(float value);
};
