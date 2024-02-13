// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Ray : ModuleRules
{
	public Ray(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","UMG","SlateCore","Slate"
		});
	}
}
