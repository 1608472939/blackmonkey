// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wukong_Weapon.generated.h"

// 前置声明
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class BLACK_MYTH_HOMEWORK_API AWukong_Weapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWukong_Weapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
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
