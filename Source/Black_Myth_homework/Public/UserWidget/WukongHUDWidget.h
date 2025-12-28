// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WukongHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLACK_MYTH_HOMEWORK_API UWukongHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 更新血条百分比
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);
    // 更新耐力百分比
    void UpdateStaminaBar(float CurrentStamina, float MaxStamina);

protected:
    // 使用 BindWidget 自动关联蓝图中的 ProgressBar 控件
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthProgressBar;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* StaminaProgressBar;
};
