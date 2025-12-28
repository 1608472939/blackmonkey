// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget/PauseUserWidget.h"
#include "PlayerController/BlackMythPlayerController.h"
#include "Components/Button.h" 

bool UPauseUserWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    // 绑定暂停界面里的“继续”按钮
    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UPauseUserWidget::OnResumeClicked);
    }
    return true;
}

void UPauseUserWidget::OnResumeClicked()
{
    // 获取当前的 PlayerController 并调用我们等下要写的恢复函数
    if (ABlackMythPlayerController* PC = Cast<ABlackMythPlayerController>(GetOwningPlayer()))
    {
        PC->TogglePause(); // 再次切换以关闭
    }
}
