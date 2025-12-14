#pragma once

#include "CoreMinimal.h"
#include "Weapon.h" // 引入父类头文件
#include "GoldenCudgel.generated.h"

UCLASS()
class WB_API AGoldenCudgel : public AWeapon
{
    GENERATED_BODY()

public:
    AGoldenCudgel(); // 只需要构造函数
};