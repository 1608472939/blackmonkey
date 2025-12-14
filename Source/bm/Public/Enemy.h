#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatInterface.h" // 引入接口
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

UCLASS()
class BM_API AEnemy : public ACharacter, public ICombatInterface
{
    GENERATED_BODY()

public:
    AEnemy();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // --- 接口实现 ---
    virtual void GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation) override;

    // --- 核心属性 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackDamage = 10.0f;

    // 状态机变量
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EEnemyState EnemyState;

    // --- 动画资源 (在蓝图中赋值) ---
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* AttackMontage; // 攻击动作

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* HitReactMontage; // 受击硬直动作

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* DeathMontage; // 死亡动作

    // --- AI 感知 ---
    UPROPERTY(VisibleAnywhere, Category = "AI")
    UPawnSensingComponent* PawnSensingComp;

    // 攻击检测半径 (球形射线检测)
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackTraceRadius = 50.0f;

    // 攻击距离 (决定何时停下来攻击)
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 150.0f;

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

    // 内部函数：死亡
    virtual void Die();

    // 内部函数：攻击结束，重置状态
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 内部函数：受击结束
    UFUNCTION()
    void OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};