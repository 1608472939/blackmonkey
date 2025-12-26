// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h" // 包含接口
#include "MyBaseCharacter.generated.h"

UCLASS()
class BLACK_MYTH_HOMEWORK_API AMyBaseCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AMyBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // --- 虚函数 ---
    // 纯虚函数的变体，我们提供一个基础实现，但希望子类根据状态机去重写它
    virtual void Die();

    // 播放受击动画的辅助函数
    virtual void PlayHitReactMontage(const FName& SectionName = FName("HitReact"));

    UFUNCTION()
    virtual void OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 辅助：判断是否死亡
    bool IsDead() const;

public:	
    // --- 核心共用属性 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackDamage = 10.0f;

    // --- 共用动画资源 ---
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* HitReactMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* DeathMontage;

    // --- 接口实现 ---
    // 标记为 virtual，允许子类重写扩展功能（比如Boss二阶段，或者悟空的UI更新）
    virtual void GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation) override;

};
