// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "Weapon/Wukong_Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h" // 用于射线检测

// 在这里添加这两行，禁用“过时”警告
PRAGMA_DISABLE_DEPRECATION_WARNINGS

// Sets default values
AEnemy::AEnemy()
{

    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SightRadius = 1500.0f;
    PawnSensingComp->SetPeripheralVisionAngle(60.0f);

    GetCharacterMovement()->MaxWalkSpeed = 450.0f;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    EnemyState = EEnemyState::Idle;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (World) {
        AWukong_Weapon* DefaultWeapon = World->SpawnActor<AWukong_Weapon>(DefaultWeaponClass);
        DefaultWeapon->Equip(GetMesh(), FName("Socket"));
        DefaultWeapon->SetOwner(this);
        DefaultWeapon->SetInstigator(this);
        EquippedWeapon = DefaultWeapon;
    }

    EnemyController = Cast<AAIController>(GetController());

    if (PawnSensingComp)
    {
        PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePlayer);
    }
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 如果死了或者正在挨打，什么都不做（硬直）
    if (EnemyState == EEnemyState::Dead || EnemyState == EEnemyState::Stunned) return;

    // 只有在追逐状态且有目标时才判断距离
    if (EnemyState == EEnemyState::Chasing && TargetActor)
    {
        float Distance = GetDistanceTo(TargetActor);
        if (Distance <= AttackRange)
        {
            AttackTarget();
        }
        else
        {
            // 如果距离拉开了，继续追
            if (EnemyController) EnemyController->MoveToActor(TargetActor, AttackRange - 10.0f);
        }
    }
}

// 开启武器碰撞
void AEnemy::WeaponCollisionEnabled()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->StartAttack();
    }
}

// 关闭武器碰撞
void AEnemy::WeaponCollisionDisabled()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->StopAttack();
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
    // 只有Idle状态下才会被激活，或者正在追的时候更新位置
    if (EnemyState == EEnemyState::Dead || EnemyState == EEnemyState::Stunned) return;

    if (SeenPawn && SeenPawn->IsPlayerControlled())
    {
        TargetActor = SeenPawn;
        // 如果之前是Idle，现在变成Chasing
        if (EnemyState == EEnemyState::Idle)
        {
            EnemyState = EEnemyState::Chasing;
        }
    }
}

void AEnemy::AttackTarget()
{
    // 如果已经在攻击了，就不要重复触发
    if (EnemyState == EEnemyState::Attacking) return;

    // 切换状态
    EnemyState = EEnemyState::Attacking;

    // 停止移动
    if (EnemyController) EnemyController->StopMovement();

    // 播放蒙太奇
    if (AttackMontage)
    {
        // 播放动画，并绑定结束回调（Lambda表达式或函数绑定）
        float Duration = PlayAnimMontage(AttackMontage);

        // 绑定蒙太奇结束事件，以便恢复状态
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &AEnemy::OnAttackMontageEnded);
        GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, AttackMontage);
    }
}

// 这是一个极其重要的函数，体现了“动作游戏”的判定
// 你需要在编辑器里，打开攻击动画，在挥刀的那一瞬间添加一个 AnimNotify，名字叫 CheckCombatHit
// 然后在蓝图中，AnimNotify 事件里调用这个 C++ 函数
void AEnemy::CheckCombatHit()
{
    if (EnemyState != EEnemyState::Attacking) return;

    // 射线检测起点和终点（通常是以前方为中心的一个球）
    FVector Start = GetActorLocation() + GetActorForwardVector() * 50.0f;
    FVector End = Start + GetActorForwardVector() * AttackRange;

    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this); // 忽略自己

    // 球形检测
    bool bHit = UKismetSystemLibrary::SphereTraceMulti(
        this, Start, End, AttackTraceRadius,
        UEngineTypes::ConvertToTraceType(ECC_Pawn), // 只检测Pawn
        false, ActorsToIgnore,
        EDrawDebugTrace::ForDuration, // 开启调试线（老师能看到红色球体，加分！）
        HitResults, true
    );

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            // 检查被打到的人有没有实现 CombatInterface
            if (HitActor && HitActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
            {
                // 调用接口函数造成伤害
                ICombatInterface::Execute_GetHit(HitActor, AttackDamage, this, Hit.ImpactPoint);
            }
        }
    }
}

void AEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 攻击动作做完了，如果还没死，且没被打断（受击），就恢复追逐
    if (EnemyState != EEnemyState::Dead && EnemyState != EEnemyState::Stunned)
    {
        EnemyState = EEnemyState::Chasing;
    }
}

