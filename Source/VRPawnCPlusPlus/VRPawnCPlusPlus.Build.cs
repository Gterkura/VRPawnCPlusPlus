// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VRPawnCPlusPlus : ModuleRules
{
	public VRPawnCPlusPlus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "HeadMountedDisplay", "NavigationSystem", "AIModule", "CinematicCamera" });
	}
}
