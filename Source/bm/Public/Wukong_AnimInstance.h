// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Wukong_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BM_API UWukong_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltatime) override;

	UPROPERTY(BlueprintReadOnly);
	class AWukong* Wukong;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	class UCharacterMovementComponent* WukongMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	bool IsFalling;
};
