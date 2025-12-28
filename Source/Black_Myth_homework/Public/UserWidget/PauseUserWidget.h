// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLACK_MYTH_HOMEWORK_API UPauseUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* ResumeButton;

    virtual bool Initialize() override;

    UFUNCTION()
    void OnResumeClicked(); // 点击“继续”后的逻辑
};
