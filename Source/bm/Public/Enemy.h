#pragma once

#include "CoreMinimal.h"
#include "MyBaseCharacter.h" // 继承新父类
#include "Enemy.generated.h"

// 状态枚举
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Chasing     UMETA(DisplayName = "Chasing"),
    Attacking   UMETA(DisplayName = "Attacking"),
    Stunned     UMETA(DisplayName = "Stunned"),
    Dead        UMETA(DisplayName = "Dead")
};

class UPawnSensingComponent;

UCLASS()
class BM_API AEnemy : public AMyBaseCharacter
{
    GENERATED_BODY()

public:
    AEnemy();
    virtual void Tick(float DeltaTime) override;

    // 初始化
    virtual void BeginPlay() override;

    // 重写父类的受击，添加仇恨逻辑
    virtual void GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation) override;

public:
    // --- 状态机 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EEnemyState EnemyState;

    // --- AI 独有动画 ---
    // 只有攻击动画是 AI 独有的（因为悟空有连招，AI可能只有一段）
    UPROPERTY(EditDefaultsOnly, Category = "AI Combat")
    UAnimMontage* AttackMontage;

    // --- AI 感知 ---
    UPROPERTY(VisibleAnywhere, Category = "AI")
    UPawnSensingComponent* PawnSensingComp;

    UPROPERTY(EditAnywhere, Category = "AI Combat")
    float AttackTraceRadius = 50.0f;

    UPROPERTY(EditAnywhere, Category = "AI Combat")
    float AttackRange = 150.0f;

protected:
    UPROPERTY(VisibleAnywhere, Category = "AI")
    AActor* TargetActor;

    UPROPERTY()
    class AAIController* EnemyController;

    UFUNCTION()
    void OnSeePlayer(APawn* SeenPawn);

    virtual void AttackTarget();

    // 动画通知调用的判定函数
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void CheckCombatHit();

    // --- 重写父类虚函数 ---
    virtual void Die() override;
    virtual void OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;

    // 内部函数：攻击结束
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};