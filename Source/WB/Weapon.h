#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h" // 【注意1】这里必须是 文件名.generated.h

// 前置声明
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class WB_API AWeapon : public AActor // 【注意2】YOURGAME_API 必须改成你的项目名 WB_API，类名改成 AWeapon
{
    GENERATED_BODY()

public:
    AWeapon(); // 构造函数名字也要改

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // --- 组件定义 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
    UBoxComponent* CollisionBox;

    // --- 属性定义 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float BaseDamage;

    // --- 功能函数 ---
    void Equip(USceneComponent* InParent, FName InSocketName);
    void StartAttack();
    void StopAttack();

protected:
    UFUNCTION()
    void OnWeaponOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

private:
    TArray<AActor*> IgnoreActors;
};