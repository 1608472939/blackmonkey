// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Wukong.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Weapon/Wukong_Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UserWidget/WukongHUDWidget.h"
//#include "Interfaces/CombatInterface.h"  已在父类中包含

int32 selection = 0;

// Sets default values
AWukong::AWukong()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	AbilityBox = CreateDefaultSubobject<USphereComponent>(TEXT("AbilityBox"));
	AbilityBox->SetupAttachment(GetRootComponent());
	AbilityBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WukongState = EWukongState::Idle;
}

// Called when the game starts or when spawned
void AWukong::BeginPlay()
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

	// 生成 HUD
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UWukongHUDWidget>(PC, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			// 初始化血条百分比
			// 注意：MaxHealth 和 CurrentHealth 假设在父类 AMyBaseCharacter 中
			HUDWidget->UpdateHealthBar(CurrentHealth, MaxHealth);
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AWukong::OnHitReactMontageEnded);
	}

}

// Called every frame
void AWukong::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWukong::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("Turn"), this, &AWukong::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AWukong::LookUp);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AWukong::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AWukong::MoveRight);
	PlayerInputComponent->BindAxis(FName("MoveForward_Run"), this, &AWukong::MoveForward_Run);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Dodge"), IE_Pressed, this, &AWukong::Dodge);

	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AWukong::Attack);
	PlayerInputComponent->BindAction(FName("Ability"), IE_Pressed, this, &AWukong::Ability);
}

void AWukong::SetAbilityCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	AbilityBox->SetCollisionEnabled(CollisionEnabled);
}

void AWukong::WeaponCollisionEnabled()
{
	EquippedWeapon->StartAttack();
}

void AWukong::WeaponCollisionDisabled()
{
	EquippedWeapon->StopAttack();
}


//待完善
void AWukong::GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation)
{
	if (WukongState == EWukongState::Dead) return;

	// 1. 调用父类：扣血、死亡检测、播放基础受击动画
	Super::GetHit_Implementation(DamageAmount, Attacker, HitLocation);

	// 每次扣血后同步 UI
	if (HUDWidget)
	{
		HUDWidget->UpdateHealthBar(CurrentHealth, MaxHealth);
	}

	// 2. 悟空特有逻辑
	if (!IsDead()) // IsDead() 来自父类
	{
		// 打断当前的攻击或技能动作
		if (AttackMontage) StopAnimMontage(AttackMontage);
		if (AbilityMontage) StopAnimMontage(AbilityMontage);

		// 设置状态为受击/硬直
		WukongState = EWukongState::Stunned;

		// 父类会自动播放 HitReactMontage
		// 如果你需要像同伴代码里那样播放特定的 "HitReact1" 等 Section，
		// 你可以在父类调用完之后，再次手动覆盖播放，或者重写 PlayHitReactMontage 虚函数。
		// 这里为了简化，我们信任父类的通用受击动画。
	}
}

// --- 核心修改：死亡逻辑 ---
void AWukong::Die()
{
	WukongState = EWukongState::Dead;

	// 调用父类：禁用碰撞，播放死亡动画
	Super::Die();

	// 可以在这里添加：显示“菜”字UI，或者“胜败乃兵家常事”
	// ShowGameOverWidget();
}

// --- 核心修改：受击恢复 ---
void AWukong::OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 恢复状态
	if (WukongState != EWukongState::Dead)
	{
		WukongState = EWukongState::Idle; // 恢复为闲置
		ActionState = EActionState::EAS_Unoccupied; // 重要：允许再次攻击

		if (GetCharacterMovement())
		{
			// 将移动模式重置为行走（Walking）
			// 这一步会解除受击过程中可能产生的移动锁定
			GetCharacterMovement()->SetDefaultMovementMode();

			// 如果你在 GetHit 里用了 StopMovementImmediately，
			// 也可以在这里确保速度不再受限
		}

		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("悟空移动已恢复"));
	}
	
}



void AWukong::MoveForward(float value) {
	if (ActionState == EActionState::EAS_Attacking || WukongState == EWukongState::Stunned) {
		return;
	}
	if (Controller && (value != 0.f)) {
		//find forward
		//FVector Forward = GetActorForwardVector();
		///AddMovementInput(Forward, value);

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void AWukong::MoveForward_Run(float value) {
	if (ActionState == EActionState::EAS_Attacking || WukongState == EWukongState::Stunned) {
		return;
	}
	if (Controller && (value != 0.f)) {
		//find forward
		//FVector Forward = GetActorForwardVector();
		///AddMovementInput(Forward, value);

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void AWukong::MoveRight(float value) {
	if (ActionState == EActionState::EAS_Attacking || WukongState == EWukongState::Stunned) {
		return;
	}
	if (Controller && (value != 0.f)) {
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}

void AWukong::Turn(float value) {
	AddControllerYawInput(value);
}

void AWukong::LookUp(float value) {
	AddControllerPitchInput(value);
}

void AWukong::Attack() {
	if (ActionState == EActionState::EAS_Unoccupied) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AWukong::Ability() {
	if (ActionState == EActionState::EAS_Unoccupied) {
		PlayAbilityMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AWukong::Dodge()
{
	if (ActionState == EActionState::EAS_Unoccupied) {
		PlayDodgeMontage();

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector NewLocation = GetActorLocation() + Direction * 500;
		SetActorLocation(NewLocation, true); //防止穿墙

		ActionState = EActionState::EAS_Attacking;
	}
}

void AWukong::AbilityDamage()
{
	FVector Start = GetActorLocation();
	FVector End = Start;

	float Radius = 500.f;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 只检测 Pawn (敌人)

	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(this);

	TArray<FHitResult> HitResults;

	//执行球形检测
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorToIgnore,
		EDrawDebugTrace::ForDuration, // 开启调试红球，方便你看到范围 (发布时改为 None)
		HitResults,
		true
	);
	
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && HitActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
			{
				// 调用接口造成伤害 (假设技能伤害是 50)
				ICombatInterface::Execute_GetHit(HitActor, 50.0f, this, Hit.Location);
			}
		}
	}
}

void AWukong::AttackEnd() {
	ActionState = EActionState::EAS_Unoccupied;
}

void AWukong::AbilityEnd() {
	ActionState = EActionState::EAS_Unoccupied;
}

void AWukong::DodgeEnd() {
	ActionState = EActionState::EAS_Unoccupied;
}

void AWukong::PlayAttackMontage() {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage) {
		AnimInstance->Montage_Play(AttackMontage);
		FName SectionName = FName();
		selection++;
		switch (selection) {
		case 1:
			SectionName = FName("Attack1");
			break;
		case 2:
			SectionName = FName("Attack2");
			break;
		case 3:
			SectionName = FName("Attack3");
			break;
		case 4:
			SectionName = FName("Attack4");
			selection = 0;
			break;
		default:
			selection = 1;
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

//Ability
void AWukong::PlayAbilityMontage() {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AbilityMontage) {
		AnimInstance->Montage_Play(AbilityMontage);
		AnimInstance->Montage_JumpToSection(FName("E_Ability"), AbilityMontage);
	}
}

void AWukong::PlayDodgeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage) {
		AnimInstance->Montage_Play(DodgeMontage);
		AnimInstance->Montage_JumpToSection(FName("Dodge"), DodgeMontage);
	}
}


//待整合
//void AWukong::PlayReactMontage() {
//	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//	if (AnimInstance && HitReactMontage) {
//		AnimInstance->Montage_Play(HitReactMontage);
//		FName SectionName = FName();
//		int Deriaction = 0;
//		switch (Deriaction) {
//		case 1:
//			SectionName = FName("HitReact1");
//			break;
//		case 2:
//			SectionName = FName("HitReact2");
//			break;
//		case 3:
//			SectionName = FName("HitReact3");
//			break;
//		case 4:
//			SectionName = FName("HitReact4");
//			break;
//		default:
//			break;
//		}
//		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
//	}
//}
