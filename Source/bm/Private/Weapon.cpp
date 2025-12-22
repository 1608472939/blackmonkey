#include "Weapon.h" // 引用头文件要改
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h" 

// 构造函数名改成 AWeapon
AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(GetRootComponent());
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);

    BaseDamage = 20.0f;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponOverlap);
}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
    WeaponMesh->AttachToComponent(InParent, AttachRules, InSocketName);
    SetOwner(InParent->GetOwner());
}

void AWeapon::StartAttack()
{
    IgnoreActors.Empty();
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::StopAttack()
{
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor == nullptr || GetOwner() == nullptr) return;
    if (OtherActor == GetOwner()) return;

    if (IgnoreActors.Contains(OtherActor)) return;

    UGameplayStatics::ApplyDamage(
        OtherActor,
        BaseDamage,
        GetOwner()->GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );

    IgnoreActors.Add(OtherActor);
}