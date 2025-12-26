// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BlackMythGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLACK_MYTH_HOMEWORK_API ABlackMythGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	//ABlackMythGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UWukongUserWidget> MainMenuWidgetClass;

	virtual void BeginPlay() override;
	//void PlayerDeath(AController* DeathController);
};
