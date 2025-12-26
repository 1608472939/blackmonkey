#pragma once

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccpuied"),
    EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_UsingAbility UMETA(DisplayName = "UsingAbility")
};

UENUM(BlueprintType)
enum class EWukongState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Chasing     UMETA(DisplayName = "Chasing"),
	Attacking   UMETA(DisplayName = "Attacking"),
	Stunned     UMETA(DisplayName = "Stunned"), // ÊÜ»÷Ó²Ö±
	Dead        UMETA(DisplayName = "Dead")
};