#include "Enemy.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h" // 用于画调试线，作业加分项

// 构造函数：初始化组件和默认值
AEnemy::AEnemy()
{
    // 开启Tick，因为我们需要在每帧检查距离
    PrimaryActorTick.bCanEverTick = true;

    // 1. 初始化感知组件
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SightRadius = 1500.0f; // 看得见的距离
    PawnSensingComp->SetPeripheralVisionAngle(60.0f); // 视野角度

    // 2. 调整移动速度
    GetCharacterMovement()->MaxWalkSpeed = 450.0f;

    // 3. 确保AI控制器能自动接管
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// 游戏开始
void AEnemy::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    // 绑定“看见人”的事件
    if (PawnSensingComp)
    {
        PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePlayer);
    }
}

// 每帧更新
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 只有当有目标且还没死的时候才运行逻辑
    if (TargetActor && CurrentHealth > 0)
    {
        // 计算与目标的距离
        float Distance = GetDistanceTo(TargetActor);

        // 如果距离小于攻击范围，且冷却完毕
        if (Distance <= AttackRange && bCanAttack)
        {
            AttackTarget();
        }
    }
}

// 看见玩家的处理逻辑
void AEnemy::OnSeePlayer(APawn* SeenPawn)
{
    // 确认看见的是玩家
    if (SeenPawn && SeenPawn->IsPlayerControlled())
    {
        TargetActor = SeenPawn;

        // 获取自身的AI控制器
        AAIController* AIController = Cast<AAIController>(GetController());
        if (AIController)
        {
            // 命令AI走到玩家位置
            AIController->MoveToActor(SeenPawn, 100.0f); // 100是停止距离
        }

        // (可选) 在头顶画个红球，方便老师看出来敌人看见你了
        DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 20.0f, 12, FColor::Red, false, 0.5f);
    }
}

// 攻击逻辑
void AEnemy::AttackTarget()
{
    if (!TargetActor) return;

    // 1. 造成伤害
    UGameplayStatics::ApplyDamage(
        TargetActor,
        AttackDamage,
        GetController(),
        this,
        UDamageType::StaticClass()
    );

    // 2. 输出日志（证明代码跑通了）
    UE_LOG(LogTemp, Warning, TEXT("Enemy hits Player for %f damage!"), AttackDamage);
    DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 100), TEXT("ATTACK!"), nullptr, FColor::Red, 1.0f);

    // 3. 进入冷却
    bCanAttack = false;
    GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemy::ResetAttack, 1.5f, false);
}

void AEnemy::ResetAttack()
{
    bCanAttack = true;
}

// 受伤逻辑
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth -= ActualDamage;
    UE_LOG(LogTemp, Warning, TEXT("Enemy took damage. Current HP: %f"), CurrentHealth);

    // 死亡检测
    if (CurrentHealth <= 0)
    {
        // 停止移动
        AAIController* AIController = Cast<AAIController>(GetController());
        if (AIController) AIController->StopMovement();

        // 变成布娃娃（如果模型支持）
        GetMesh()->SetSimulatePhysics(true);
        GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

        // 3秒后销毁
        SetLifeSpan(3.0f);
    }

    return ActualDamage;
}