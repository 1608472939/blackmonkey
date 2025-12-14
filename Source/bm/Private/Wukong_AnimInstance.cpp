// Fill out your copyright notice in the Description page of Project Settings.


#include "Wukong_AnimInstance.h"
#include "Wukong.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UWukong_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Wukong = Cast<AWukong>(TryGetPawnOwner());
	if (Wukong) {
		WukongMovement = Wukong->GetCharacterMovement();
	}
}

void UWukong_AnimInstance::NativeUpdateAnimation(float deltatime)
{
	Super::NativeUpdateAnimation(deltatime);

	if (WukongMovement) {
		GroundSpeed = UKismetMathLibrary::VSizeXY(WukongMovement->Velocity);
		IsFalling = WukongMovement->IsFalling();
	}
}
