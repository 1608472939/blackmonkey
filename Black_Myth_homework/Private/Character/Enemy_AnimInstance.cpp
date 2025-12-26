// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"


void UEnemy_AnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // 获取拥有这个动画组件的角色
    EnemyCharacter = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemy_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 如果引用丢失，尝试重新获取
    if (!EnemyCharacter)
    {
        EnemyCharacter = Cast<AEnemy>(TryGetPawnOwner());
    }

    if (EnemyCharacter)
    {
        // 1. 计算速度 (只取水平速度)
        FVector Velocity = EnemyCharacter->GetVelocity();
        Velocity.Z = 0;
        Speed = Velocity.Size();

        // 2. 获取死亡状态 (需要你在 Enemy.h 把 EnemyState 设为 public 或者提供 Get 方法)
        // 假设你在 Enemy.h 里把 EnemyState 放在了 public
        bIsDead = (EnemyCharacter->EnemyState == EEnemyState::Dead);
    }
}