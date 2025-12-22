#include "Wukong.h" // 确保引用正确的头文件路径，例如 "Character/Wukong.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Weapon/Wukong_Weapon.h" // 确保路径正确
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
// #include "Interfaces/CombatInterface.h" // 父类已包含，这里可以省略，除非你需要直接使用接口静态函数

// 连招计数器 (保留你同伴的逻辑)
static int32 selection = 0;

AWukong::AWukong()
{
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

	// 初始化状态
	WukongState = EWukongState::Idle;
}

void AWukong::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World && DefaultWeaponClass) {
		AWukong_Weapon* DefaultWeapon = World->SpawnActor<AWukong_Weapon>(DefaultWeaponClass);
		if (DefaultWeapon)
		{
			DefaultWeapon->Equip(GetMesh(), FName("Socket"));
			DefaultWeapon->SetOwner(this);
			DefaultWeapon->SetInstigator(this);
			EquippedWeapon = DefaultWeapon;
		}
	}
}

void AWukong::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

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
	if (AbilityBox)
	{
		AbilityBox->SetCollisionEnabled(CollisionEnabled);
	}
}

// --- 核心修改：受击逻辑 ---
void AWukong::GetHit_Implementation(float DamageAmount, AActor* Attacker, const FVector& HitLocation)
{
	if (WukongState == EWukongState::Dead) return;

	// 1. 调用父类：扣血、死亡检测、播放基础受击动画
	Super::GetHit_Implementation(DamageAmount, Attacker, HitLocation);

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
	}
}

// --- 以下为移动和攻击逻辑 (基本保持不变) ---

void AWukong::MoveForward(float value) {
	if (ActionState == EActionState::EAS_Attacking || WukongState == EWukongState::Stunned || WukongState == EWukongState::Dead) {
		return;
	}
	if (Controller && (value != 0.f)) {
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void AWukong::MoveForward_Run(float value) {
	// 逻辑同上，可复用
	MoveForward(value);
}

void AWukong::MoveRight(float value) {
	if (ActionState == EActionState::EAS_Attacking || WukongState == EWukongState::Stunned || WukongState == EWukongState::Dead) {
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
	if (ActionState == EActionState::EAS_Unoccupied && WukongState != EWukongState::Dead && WukongState != EWukongState::Stunned) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AWukong::Ability() {
	if (ActionState == EActionState::EAS_Unoccupied && WukongState != EWukongState::Dead && WukongState != EWukongState::Stunned) {
		PlayAbilityMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AWukong::Dodge()
{
	if (ActionState == EActionState::EAS_Unoccupied && WukongState != EWukongState::Dead && WukongState != EWukongState::Stunned) {
		PlayDodgeMontage();

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// 简单的位移，建议改为由动画根运动(Root Motion)控制更平滑
		FVector NewLocation = GetActorLocation() + Direction * 500;
		SetActorLocation(NewLocation, true); // sweep = true 防止穿墙

		ActionState = EActionState::EAS_Attacking; // 闪避视为动作中
	}
}

void AWukong::AbilityDamage()
{
	FVector Start = GetActorLocation();
	FVector End = Start;
	float Radius = 500.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(this);

	TArray<FHitResult> HitResults;

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this, Start, End, Radius, ObjectTypes, false, ActorToIgnore,
		EDrawDebugTrace::ForDuration, HitResults, true
	);

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			// 使用接口调用，安全且支持多态
			if (HitActor && HitActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
			{
				// 造成50点伤害
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
		// 简单的连招逻辑
		switch (selection) {
			case 1: SectionName = FName("Attack1"); break;
			case 2: SectionName = FName("Attack2"); break;
			case 3: SectionName = FName("Attack3"); break;
			case 4: SectionName = FName("Attack4"); selection = 0; break;
			default: SectionName = FName("Attack1"); selection = 1; break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

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