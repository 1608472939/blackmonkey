// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Wukong_Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h" 
#include "Interfaces/CombatInterface.h"

// Sets default values
AWukong_Weapon::AWukong_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(GetRootComponent());
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);

    BaseDamage = 20.0f;
}

// Called when the game starts or when spawned
void AWukong_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWukong_Weapon::OnWeaponOverlap);
}

// Called every frame
void AWukong_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWukong_Weapon::Equip(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
    WeaponMesh->AttachToComponent(InParent, AttachRules, InSocketName);
    SetOwner(InParent->GetOwner());
}

void AWukong_Weapon::StartAttack()
{
    IgnoreActors.Empty();
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWukong_Weapon::StopAttack()
{
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWukong_Weapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("武器击中了: %s"), *OtherActor->GetName()));

    if (OtherActor == nullptr || GetOwner() == nullptr) return;
    if (OtherActor == GetOwner()) return;

    if (IgnoreActors.Contains(OtherActor)) return;

    if (OtherActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
    {
        ICombatInterface::Execute_GetHit(
            OtherActor,
            BaseDamage,
            GetOwner(),
            SweepResult.ImpactPoint // 传入击中位置用于特效或计算
        );

        // 成功触发接口后，打印绿字确认，调试使用
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("接口调用成功：已发送伤害请求"));
    }
    else
    {
        // 调试使用
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("警告：目标未实现 CombatInterface"));
    }

    /*UGameplayStatics::ApplyDamage(
        OtherActor,
        BaseDamage,
        GetOwner()->GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );*/

    IgnoreActors.Add(OtherActor);
}