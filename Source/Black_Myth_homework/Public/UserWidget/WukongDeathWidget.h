// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "WukongDeathWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLACK_MYTH_HOMEWORK_API UWukongDeathWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// meta = (BindWidget) 会自动将 C++ 变量与蓝图中的同名控件绑定
	// 这样你只需在蓝图中摆放位置，逻辑全部写在 C++ 里
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathText;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuButton;

	// 初始化逻辑（类似 BeginPlay）
	virtual bool Initialize() override;

	// 按钮点击回调
	UFUNCTION()
	void OnMainMenuClicked();
};
