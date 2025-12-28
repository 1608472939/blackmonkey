// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/BlackMythPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UserWidget/PauseUserWidget.h"


void ABlackMythPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    if (InputComponent)
    {
        // "Pause" 必须和你在项目设置（Project Settings -> Input）里填写的名字完全一致
        InputComponent->BindAction("Pause", IE_Pressed, this, &ABlackMythPlayerController::TogglePause);
    }
}


void ABlackMythPlayerController::TogglePause()
{
    if (!PauseMenuWidgetClass) return;

    if (!PauseMenuInstance) // 第一次按下，创建 UI
    {
        PauseMenuInstance = CreateWidget<UPauseUserWidget>(this, PauseMenuWidgetClass);
    }

    if (PauseMenuInstance)
    {
        
        if (!PauseMenuInstance->IsInViewport())
        {
            // --- 暂停游戏逻辑 ---
            PauseMenuInstance->AddToViewport();
            bShowMouseCursor = true;

            // 设置输入模式：同时响应游戏和 UI（这样可以点按钮，也可以响应再次按 ESC）
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(PauseMenuInstance->TakeWidget());
            SetInputMode(InputMode);

            UGameplayStatics::SetGamePaused(this, true); // 物理暂停游戏
        }
        else
        {
            // --- 恢复游戏逻辑 ---
            PauseMenuInstance->RemoveFromParent();
            bShowMouseCursor = false;

            FInputModeGameOnly InputMode;
            SetInputMode(InputMode);

            UGameplayStatics::SetGamePaused(this, false); // 取消暂停
        }
    }
}
