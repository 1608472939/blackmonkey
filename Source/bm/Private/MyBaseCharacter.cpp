#include "MyBaseCharacter.h"
#include "Components/CapsuleComponent.h"

AMyBaseCharacter::AMyBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMyBaseCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth; // 初始化血量
}

void AMyBaseCharacter::GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation)
{
    if (IsDead()) return;

    // 1. 扣血 (通用逻辑)
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

    // 2. 死亡判定
    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
    else
    {
        // 3. 播放受击动画 (通用逻辑)
        PlayHitReactMontage();
    }
}

void AMyBaseCharacter::Die()
{
    // 通用死亡逻辑：禁用碰撞
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }

    // 注意：不要在这里SetLifeSpan，因为悟空死了不能销毁，要重开。
    // 敌人的销毁逻辑可以在AEnemy里重写追加。
}

void AMyBaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
    if (HitReactMontage)
    {
        PlayAnimMontage(HitReactMontage, 1.0f, SectionName);

        // 绑定结束回调
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &AMyBaseCharacter::OnHitReactMontageEnded);
        GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, HitReactMontage);
    }
}

void AMyBaseCharacter::OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 子类负责重置状态 (例如: Stunned -> Idle)
}

bool AMyBaseCharacter::IsDead() const
{
    return CurrentHealth <= 0.0f;
}