#pragma once

#include "CoreMinimal.h"
#include "MyBaseCharacter.h" // 1. 引入父类
#include "Wukong_State.h"    // 确保包含状态枚举定义
#include "Wukong.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class AWukong_Weapon;
class USphereComponent;

UCLASS()
class BM_API AWukong : public AMyBaseCharacter // 2. 修改继承关系
{
	GENERATED_BODY()

public:
	AWukong();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetAbilityCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	// --- 接口实现 ---
	// 重写父类的受击逻辑（体现多态）
	virtual void GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation) override;

	// 注意：已删除 MaxHealth, CurrentHealth, AttackDamage (父类已包含)
	// 注意：已删除 ICombatInterface 声明 (父类已继承)

	// 状态机变量 (悟空特有)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EWukongState WukongState;

protected:
	virtual void BeginPlay() override;

	// --- 重写父类虚函数 ---
	virtual void Die() override; // 重写死亡逻辑
	virtual void OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted) override; // 重写受击恢复

	// --- 输入处理 ---
	void Turn(float value);
	void LookUp(float value);
	void Attack();
	void Ability();
	void Dodge();

	UFUNCTION(BlueprintCallable)
	void AbilityDamage();

	// 播放蒙太奇
	void PlayAttackMontage();
	void PlayAbilityMontage();
	void PlayDodgeMontage();
	// PlayReactMontage 已由父类处理，这里移除或改为 override

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void AbilityEnd();

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	// --- 移动逻辑 ---
	void MoveForward(float value);
	void MoveRight(float value);
	void MoveForward_Run(float value);

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWukong_Weapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<class AWukong_Weapon> DefaultWeaponClass;

private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere, Category = "Ability Properties")
	USphereComponent* AbilityBox;

	// --- 蒙太奇资源 ---
	// 悟空特有的攻击动作
	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* AbilityMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* DodgeMontage;

	// HitReactMontage 已删除，使用父类的 "Base Combat" -> HitReactMontage
	// 请在蓝图中重新赋值父类的 HitReactMontage
};