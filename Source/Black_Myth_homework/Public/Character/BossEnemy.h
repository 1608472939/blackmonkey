// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy.h"
#include "Components/SphereComponent.h"
#include "BossEnemy.generated.h"

class UHealthBarComponent;

/**
 * 
 */
UCLASS()
class BLACK_MYTH_HOMEWORK_API ABossEnemy : public AEnemy
{
	GENERATED_BODY()
	
public:
    ABossEnemy();

    // 重写受击接口 (不调用父类，完全自己接管)
    virtual void GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation) override;

public:
    // --- 霸体/韧性系统 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Stats")
    float MaxPoise = 100.0f; // 最大韧性值

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Stats")
    float CurrentPoise;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Stats")
    float PoiseRecoveryRate = 5.0f; // 韧性恢复速度 (可选)

    // --- 二阶段系统 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Phase")
    bool bIsPhaseTwo = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Stats")
    float PhaseTwoHealthThreshold = 0.5f; // 血量低于 50% 进二阶段

    // --- 动画资源 ---
    // 二阶段变身动画（吼叫）
    UPROPERTY(EditDefaultsOnly, Category = "Boss Combat")
    UAnimMontage* PhaseChangeMontage;

    // 韧性被打崩时的动画 (大硬直)
    UPROPERTY(EditDefaultsOnly, Category = "Boss Combat")
    UAnimMontage* StunMontage;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    UHealthBarComponent* HealthBarWidget;
};
