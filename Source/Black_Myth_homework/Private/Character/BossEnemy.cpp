// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BossEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "UserWidget/HealthBarComponent.h"

ABossEnemy::ABossEnemy()
{
    // Boss 通常体型大一点，血多一点
    MaxHealth = 1000.0f;
    MaxPoise = 100.0f;

    HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(GetRootComponent());
}

void ABossEnemy::BeginPlay()
{
    Super::BeginPlay();
    CurrentPoise = MaxPoise; // 初始化韧性

    if (HealthBarWidget) {
        HealthBarWidget->SetHealthPercent(1.f);
    }
}

void ABossEnemy::GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation)
{
    // 1. 如果已经死了，直接跳过
    if (CurrentHealth <= 0) return;

    // 2. 仇恨逻辑 (保留父类的 AI 逻辑，如果有的话，或者直接设置目标)
    if (Attacker) TargetActor = Attacker;

    // 3. 扣血 (手动扣，不调用 Super，以免触发父类的受击动画)
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

    HealthBarWidget->SetHealthPercent(CurrentHealth / MaxHealth);

    // 4. 死亡判定
    if (CurrentHealth <= 0)
    {
        Die(); // 调用父类的死亡
        return;
    }

    // ==========================================
    // 核心逻辑：二阶段检查
    // ==========================================
    if (!bIsPhaseTwo && CurrentHealth < (MaxHealth * PhaseTwoHealthThreshold))
    {
        bIsPhaseTwo = true;

        // 进二阶段：播放变身动画
        if (PhaseChangeMontage)
        {
            // 这里可以清除仇恨，或者无敌一小会儿
            EnemyState = EEnemyState::Stunned; // 借用 Stunned 状态防止移动
            PlayAnimMontage(PhaseChangeMontage);

            // 可以在这里加特效：生成火焰、震屏等
            // UGameplayStatics::SpawnEmitterAtLocation(...)
        }

        // 变身时霸体回满，或者变得更强
        MaxPoise *= 2.0f;
        CurrentPoise = MaxPoise;
        AttackDamage *= 1.5f;

        UE_LOG(LogTemp, Warning, TEXT("BOSS 进入二阶段！"));
        return; // 变身时无视这次的削韧
    }

    // ==========================================
    // 核心逻辑：霸体/削韧
    // ==========================================

    // 如果正在变身，或者已经处于大硬直中，就不再处理韧性
    if (EnemyState == EEnemyState::Stunned) return;

    CurrentPoise -= DamageAmount; // 或者减去特定的削韧值

    if (CurrentPoise <= 0)
    {
        // 韧性被打破！Boss 跪地/大硬直
        UE_LOG(LogTemp, Warning, TEXT("BOSS 破防了！"));

        EnemyState = EEnemyState::Stunned;

        // 打断当前的攻击
        StopAnimMontage(AttackMontage);

        if (StunMontage)
        {
            PlayAnimMontage(StunMontage);
            // 绑定结束回调（参考父类逻辑）
            FOnMontageEnded EndDelegate;
            EndDelegate.BindUObject(this, &ABossEnemy::OnHitReactMontageEnded);
            GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, StunMontage);
        }

        // 重置韧性
        CurrentPoise = MaxPoise;
    }
    else
    {
        // 霸体生效中！
        // 这里什么都不做，Boss 会继续它的攻击动作，不会被打断。
        // 这就是“霸体”的本质：只扣血，不播动画。
    }
}
