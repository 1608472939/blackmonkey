#include "GoldenCudgel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

AGoldenCudgel::AGoldenCudgel()
{
    // 1. 设置伤害
    BaseDamage = 80.0f;

    // 2. 【核心修改】加载 UE5 自带的圆柱体模型
    // 注意：这个路径是 UE 引擎内部内容的绝对路径，任何 UE 项目都能用
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

    if (MeshAsset.Succeeded())
    {
        WeaponMesh->SetStaticMesh(MeshAsset.Object);

        // 【修改1】把 Z轴缩放改成 2.0
        // 1米(基础) * 2.0 = 2米长，这才是金箍棒的长度
        WeaponMesh->SetRelativeScale3D(FVector(0.05f, 0.05f, 2.0f));

        // 【注意】UE自带的圆柱体中心点在中间。
        // 为了让角色的手握在棍子中间，我们暂时不需要移动它的位置 (0,0,0)
    }

    if (CollisionBox)
    {
        // 【修改2】调整碰撞盒大小
        // BoxExtent 是“半长”，所以设为 100。 全长 = 100 * 2 = 200cm = 2米
        // X, Y 设为 15，稍微比棍子粗一点点，保证容易打中
        CollisionBox->SetBoxExtent(FVector(15.0f, 15.0f, 50.0f));

        // 确保中心对齐
        CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

        // 保持显示碰撞盒以便调试
        CollisionBox->SetHiddenInGame(false);
    }
}