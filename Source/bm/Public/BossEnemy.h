#pragma once
#include "CoreMinimal.h"
#include "Enemy.h"
#include "BossEnemy.generated.h"

UCLASS()
class BM_API ABossEnemy : public AEnemy
{
    GENERATED_BODY()

public:
    // 重写受击函数，加入二阶段判断
    virtual void GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
    bool bIsPhaseTwo = false;

    // 二阶段变身动画
    UPROPERTY(EditDefaultsOnly, Category = "Boss")
    UAnimMontage* PhaseChangeMontage;
}; 