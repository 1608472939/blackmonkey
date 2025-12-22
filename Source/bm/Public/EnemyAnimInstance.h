#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy.h" // 引入你的敌人头文件
#include "EnemyAnimInstance.generated.h"

UCLASS()
class BM_API UEnemyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // 类似于蓝图的 Event Blueprint Update Animation
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    // 类似于蓝图的 Event Blueprint Initialize Animation
    virtual void NativeInitializeAnimation() override;

protected:
    // 暴露给动画蓝图的变量
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Animation")
    float Speed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Animation")
    bool bIsDead;

    // 引用我们要控制的敌人
    UPROPERTY()
    class AEnemy* EnemyCharacter;
};