#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h" // 这里的名字必须和文件名一致

class UPawnSensingComponent; // 前向声明

UCLASS()
class BM_API AEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    // 构造函数
    AEnemy();

protected:
    // 游戏开始时执行
    virtual void BeginPlay() override;

public:
    // 每帧执行
    virtual void Tick(float DeltaTime) override;

    // --- 属性定义 ---

    // 敌人最大血量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float MaxHealth = 100.0f;

    // 当前血量
    UPROPERTY(VisibleAnywhere, Category = "Enemy Stats")
    float CurrentHealth;

    // 攻击力
    UPROPERTY(EditAnywhere, Category = "Enemy Stats")
    float AttackDamage = 10.0f;

    // 攻击距离
    UPROPERTY(EditAnywhere, Category = "Enemy Stats")
    float AttackRange = 150.0f;

    // --- 组件定义 ---

    // 感知组件（用来作为敌人的“眼睛”）
    UPROPERTY(VisibleAnywhere, Category = "AI")
    UPawnSensingComponent* PawnSensingComp;

    // --- 函数定义 ---

    // 受伤函数（重写UE自带的）
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
    // 当看见玩家时触发
    UFUNCTION()
    void OnSeePlayer(APawn* SeenPawn);

    // 攻击玩家
    void AttackTarget();

    // 攻击冷却相关的变量
    bool bCanAttack = true;
    FTimerHandle AttackTimerHandle;
    void ResetAttack();

    // 记录锁定的目标
    UPROPERTY()
    AActor* TargetActor;
};