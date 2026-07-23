// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CH3_Team12 : ModuleRules
{
	public CH3_Team12(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags", "AIModule", "UMG",
			"AudioModulation", "Slate", "SlateCore", "Niagara", "NavigationSystem"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
