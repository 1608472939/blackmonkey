#include "Enemy.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AEnemy::AEnemy()
{
    // MyBaseCharacter 构造函数已经处理了 Tick 设置

    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SightRadius = 1500.0f;
    PawnSensingComp->SetPeripheralVisionAngle(60.0f);

    GetCharacterMovement()->MaxWalkSpeed = 450.0f;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    EnemyState = EEnemyState::Idle;
}

void AEnemy::BeginPlay()
{
    Super::BeginPlay(); // 此时父类会初始化 MaxHealth -> CurrentHealth

    EnemyController = Cast<AAIController>(GetController());

    if (PawnSensingComp)
    {
        PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePlayer);
    }
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsDead() || EnemyState == EEnemyState::Stunned) return;

    if (EnemyState == EEnemyState::Chasing && TargetActor)
    {
        float Distance = GetDistanceTo(TargetActor);
        if (Distance <= AttackRange)
        {
            AttackTarget();
        }
        else
        {
            if (EnemyController) EnemyController->MoveToActor(TargetActor, AttackRange - 10.0f);
        }
    }
}

// 核心重构：受击逻辑
void AEnemy::GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation)
{
    // 1. 设置仇恨 (AI 特有)
    if (Attacker) TargetActor = Attacker;

    // 2. 调用父类 (处理扣血、通用死亡检查、播放受击动画)
    // 注意：父类会自动调用 PlayHitReactMontage
    Super::GetHit_Implementation(DamageAmount, Attacker, HitLocation);

    // 3. 更新 AI 状态
    if (!IsDead())
    {
        if (EnemyController) EnemyController->StopMovement();

        // 打断攻击
        if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
        {
            StopAnimMontage(AttackMontage);
        }

        EnemyState = EEnemyState::Stunned;
        // 接下来父类会播放受击动画，并触发 OnHitReactMontageEnded
    }
}

// 核心重构：死亡逻辑
void AEnemy::Die()
{
    EnemyState = EEnemyState::Dead;
    if (EnemyController) EnemyController->StopMovement();

    // 调用父类播放死亡动画
    Super::Die();

    // AI 特有：5秒后消失
    SetLifeSpan(5.0f);
}

// 核心重构：受击结束恢复
void AEnemy::OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!IsDead())
    {
        // 只有没死才恢复追逐
        EnemyState = EEnemyState::Chasing;
    }
}

void AEnemy::OnSeePlayer(APawn* SeenPawn)
{
    if (IsDead() || EnemyState == EEnemyState::Stunned) return;

    if (SeenPawn && SeenPawn->IsPlayerControlled())
    {
        TargetActor = SeenPawn;
        if (EnemyState == EEnemyState::Idle)
        {
            EnemyState = EEnemyState::Chasing;
        }
    }
}

void AEnemy::AttackTarget()
{
    if (EnemyState == EEnemyState::Attacking) return;

    EnemyState = EEnemyState::Attacking;
    if (EnemyController) EnemyController->StopMovement();

    if (AttackMontage)
    {
        PlayAnimMontage(AttackMontage);

        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &AEnemy::OnAttackMontageEnded);
        GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, AttackMontage);
    }
}

void AEnemy::CheckCombatHit()
{
    if (EnemyState != EEnemyState::Attacking) return;

    FVector Start = GetActorLocation() + GetActorForwardVector() * 50.0f;
    FVector End = Start + GetActorForwardVector() * AttackRange;
    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);

    bool bHit = UKismetSystemLibrary::SphereTraceMulti(
        this, Start, End, AttackTraceRadius,
        UEngineTypes::ConvertToTraceType(ECC_Pawn),
        false, ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        HitResults, true
    );

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            // 使用接口造成伤害
            if (HitActor && HitActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
            {
                // 注意这里调用接口的方式
                ICombatInterface::Execute_GetHit(HitActor, AttackDamage, this, Hit.ImpactPoint);
            }
        }
    }
}

void AEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!IsDead() && EnemyState != EEnemyState::Stunned)
    {
        EnemyState = EEnemyState::Chasing;
    }
}