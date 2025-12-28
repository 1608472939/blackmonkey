// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BlackMythGameMode.h"
#include "Character/Wukong.h"
#include "UserWidget/WukongUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

//ABlackMythGameMode::ABlackMythGameMode()
//{
//	DefaultPawnClass = AWukong::StaticClass();
//	//PlayerControllerClass = 
//}

void ABlackMythGameMode::BeginPlay() {
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && MainMenuWidgetClass)
	{
		// 创建 UI 实例
		UWukongUserWidget* MenuWidget = CreateWidget<UWukongUserWidget>(PC, MainMenuWidgetClass);
		if (MenuWidget)
		{
			// 将 UI 放到屏幕上
			MenuWidget->AddToViewport();

			// 关键：设置输入模式为“仅 UI”，并显示鼠标
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MenuWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
	else {
		// 如果没设 UI，至少打印一个警告而不是崩溃
		UE_LOG(LogTemp, Warning, TEXT("GameMode: MainMenuWidgetClass is NULL! 请在蓝图中指定 UI 类。"));
	}
}