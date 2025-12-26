// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseCharacter.h" // 继承新父类
#include "Enemy.generated.h"

// 定义状态枚举（体现C++状态管理）
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Chasing     UMETA(DisplayName = "Chasing"),
	Attacking   UMETA(DisplayName = "Attacking"),
	Stunned     UMETA(DisplayName = "Stunned"), // 受击硬直
	Dead        UMETA(DisplayName = "Dead")
};

class UPawnSensingComponent;

class AWukong_Weapon;

UCLASS()
class BLACK_MYTH_HOMEWORK_API AEnemy : public AMyBaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    // --- 接口实现 ---
    virtual void GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation) override;

    // --- 核心属性 ---
    //已在父类中声明

    // 状态机变量
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EEnemyState EnemyState;

    // --- 动画资源 (AI独有) ---
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* AttackMontage; // 攻击动作

    //UPROPERTY(EditDefaultsOnly, Category = "Combat")
    //UAnimMontage* HitReactMontage; // 受击硬直动作

    //UPROPERTY(EditDefaultsOnly, Category = "Combat")
    //UAnimMontage* DeathMontage; // 死亡动作

    // --- AI 感知 ---
    UPROPERTY(VisibleAnywhere, Category = "AI")
    UPawnSensingComponent* PawnSensingComp;

    // 攻击检测半径 (球形射线检测)
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackTraceRadius = 50.0f;

    // 攻击距离 (决定何时停下来攻击)
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 150.0f;

    UFUNCTION(BlueprintCallable)
    void WeaponCollisionEnabled();

    UFUNCTION(BlueprintCallable)
    void WeaponCollisionDisabled();

protected:
    // 目标
    UPROPERTY(VisibleAnywhere, Category = "AI")
    AActor* TargetActor;

    // AI控制器引用
    UPROPERTY()
    class AAIController* EnemyController;

    UFUNCTION()
    void OnSeePlayer(APawn* SeenPawn);

    // 真正的攻击逻辑（播放动画）
    virtual void AttackTarget();

    // 动画通知调用的函数：攻击判定帧（关键！体现动作游戏特性）
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void CheckCombatHit();

    // --- 重写父类虚函数 ---
    virtual void Die() override;
    virtual void OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;

    // 内部函数：攻击结束，重置状态
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    AWukong_Weapon* EquippedWeapon;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    TSubclassOf<class AWukong_Weapon> DefaultWeaponClass;

};
