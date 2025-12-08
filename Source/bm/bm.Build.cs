// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class bm : ModuleRules
{
	public bm(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
            "NavigationSystem"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"bm",
			"bm/Variant_Platforming",
			"bm/Variant_Platforming/Animation",
			"bm/Variant_Combat",
			"bm/Variant_Combat/AI",
			"bm/Variant_Combat/Animation",
			"bm/Variant_Combat/Gameplay",
			"bm/Variant_Combat/Interfaces",
			"bm/Variant_Combat/UI",
			"bm/Variant_SideScrolling",
			"bm/Variant_SideScrolling/AI",
			"bm/Variant_SideScrolling/Gameplay",
			"bm/Variant_SideScrolling/Interfaces",
			"bm/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
