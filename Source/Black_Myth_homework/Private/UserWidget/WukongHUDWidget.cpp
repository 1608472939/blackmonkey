// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget/WukongHUDWidget.h"
#include "Components/ProgressBar.h"

void UWukongHUDWidget::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    if (HealthProgressBar && MaxHealth > 0.f)
    {
        // 设置进度条百分比 (0.0 - 1.0)
        HealthProgressBar->SetPercent(CurrentHealth / MaxHealth);
    }
}

void UWukongHUDWidget::UpdateStaminaBar(float CurrentStamina, float MaxStamina)
{
    if (StaminaProgressBar && MaxStamina > 0.f)
    {
        // 设置进度条百分比 (0.0 - 1.0)
        StaminaProgressBar->SetPercent(CurrentStamina / MaxStamina);
    }
}
