// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Wukong_State.h"
#include "MyBaseCharacter.h" // 1. 引入父类
#include "Wukong.generated.h"

class ATargetPoint;
class UWukongHUDWidget;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class AWukong_Weapon;
class USphereComponent;

UCLASS()
class BLACK_MYTH_HOMEWORK_API AWukong : public AMyBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWukong();

    virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetAbilityCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	void WeaponCollisionEnabled();

	UFUNCTION(BlueprintCallable)
	void WeaponCollisionDisabled();

	// --- 接口实现 ---
	// 重写父类的受击逻辑（体现多态）
	virtual void GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation) override;

	// --- 核心属性 ---
	// 已删除 MaxHealth, CurrentHealth, AttackDamage (父类已包含)
	// 已删除 ICombatInterface 声明 (父类已继承)

	// 状态机变量
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EWukongState WukongState;

protected:
	// 在蓝图中指定 HUD 材质类
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UWukongHUDWidget> HUDWidgetClass;

	// 存储生成的 HUD 实例
	UPROPERTY()
	UWukongHUDWidget* HUDWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UWukongDeathWidget> DeathScreenClass;

	UPROPERTY()
	UUserWidget* DeathScreenInstance;

	// 用于存储转移的目的地（在编辑器中指定 Target Point）
	UPROPERTY(EditAnywhere, Category = "Teleport")
	TArray<AActor*> AllTeleportPoints;

	// 执行转移的函数
	void TeleportToTarget(AActor* DestinationActor);

	void TeleportToSpecificIndex();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// --- 重写父类虚函数 ---
	virtual void Die() override; // 重写死亡逻辑

	virtual void OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted) override; // 重写受击恢复

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxStamina = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentStamina;

	float DodgeStaminaCost = 10.f;
	float StaminaReload = 5.f;
	float HealthReload = 5.f;

	void Turn(float value);
	void LookUp(float value);
	void Attack();
	void Ability();
	void Dodge();

	// 是否正在冷却中
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	bool bIsAbilityOnCooldown = false;

	// 技能冷却时间（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	float AbilityCooldownDuration = 5.0f;

	// 定时器句柄
	FTimerHandle AbilityCooldownTimerHandle;

	// 冷却结束后的回调函数
	void ResetAbilityCooldown();

	UFUNCTION(BlueprintCallable)
	void AbilityDamage();

	//play montage
	void PlayAttackMontage();
	void PlayAbilityMontage();
	void PlayDodgeMontage();
	//void PlayReactMontage();  已由父类处理

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void AbilityEnd();

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWukong_Weapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<class AWukong_Weapon> DefaultWeaponClass;

private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	void MoveForward(float value);
	void MoveRight(float value);
	void MoveForward_Run(float value);

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere, Category = "Ability Properties")
	USphereComponent* AbilityBox;

	//蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* AbilityMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* DodgeMontage;

	//HitReactMontage由父类实现

};
