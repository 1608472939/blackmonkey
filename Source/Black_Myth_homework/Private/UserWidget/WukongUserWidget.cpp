// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget/WukongUserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UWukongUserWidget::Initialize()
{
	if (!Super::Initialize()) {
		return false;
	}
	
	if (StartButton) {
		StartButton->OnClicked.AddDynamic(this, &UWukongUserWidget::OnStartClicked);
	}
	if (QuitButton) {
		QuitButton->OnClicked.AddDynamic(this, &UWukongUserWidget::OnQuitClicked);
	}

	return true;
}

void UWukongUserWidget::OnStartClicked() {
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		// 隐藏鼠标
		PC->bShowMouseCursor = false;
		// 将输入模式切回纯游戏模式
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	// --- 核心修复：移除当前主菜单 UI ---
	// 如果不移除，新地图加载后，这个 UI 依然会覆盖在最前面
	RemoveFromParent();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Start Button Clicked!"));

	// 执行关卡跳转
	UGameplayStatics::OpenLevel(this, FName("/Game/UE_1"));
}

void UWukongUserWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
