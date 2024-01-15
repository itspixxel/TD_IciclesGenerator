// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TD_IciclesGenerator : ModuleRules
{
	public TD_IciclesGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
