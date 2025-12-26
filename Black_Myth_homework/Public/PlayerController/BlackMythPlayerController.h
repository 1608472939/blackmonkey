// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlackMythPlayerController.generated.h"

class UPauseUserWidget;

/**
 * 
 */
UCLASS()
class BLACK_MYTH_HOMEWORK_API ABlackMythPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPauseUserWidget> PauseMenuWidgetClass;

	void TogglePause();

protected:
	virtual void SetupInputComponent() override;

private:
	UPROPERTY()
	UUserWidget* PauseMenuInstance;

};
