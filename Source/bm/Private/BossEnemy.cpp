#include "BossEnemy.h"

void ABossEnemy::GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation)
{
    // 调用父类的扣血逻辑
    Super::GetHit(DamageAmount, Attacker, HitLocation);

    // 检查是否进入二阶段 (血量低于50%且未变身)
    if (!bIsPhaseTwo && CurrentHealth < (MaxHealth * 0.5f))
    {
        bIsPhaseTwo = true;

        // 狂暴：增加攻击力，变红，或者播放吼叫动画
        AttackDamage *= 2.0f;

        if (PhaseChangeMontage)
        {
            // 强制播放转阶段动画
            EnemyState = EEnemyState::Stunned; // 借用Stunned状态防止移动
            PlayAnimMontage(PhaseChangeMontage);

            // 这里可以加一个粒子特效或者震屏代码
        }

        UE_LOG(LogTemp, Warning, TEXT("BOSS ENTERS PHASE 2!"));
    }
}