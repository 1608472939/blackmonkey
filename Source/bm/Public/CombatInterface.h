#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class BM_API ICombatInterface
{
	GENERATED_BODY()

public:
	// 纯虚函数：受伤接口
	// HitTag: 用于区分左劈、右砍等，播放不同受击动画
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void GetHit(float DamageAmount, AActor* Attacker, const FVector& HitLocation);
};