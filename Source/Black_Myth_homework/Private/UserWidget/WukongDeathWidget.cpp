// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget/WukongDeathWidget.h"
#include "Kismet/GameplayStatics.h"

bool UWukongDeathWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	// 绑定按钮点击事件
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UWukongDeathWidget::OnMainMenuClicked);
	}

	if (DeathText)
	{
		DeathText->SetText(FText::FromString(TEXT("DEATH")));
	}

	return true;
}

void UWukongDeathWidget::OnMainMenuClicked()
{
	// 这里的 "MainMenuMap" 换成你真实的主菜单关卡名
	UGameplayStatics::OpenLevel(this, FName("TestLevel"));
}
