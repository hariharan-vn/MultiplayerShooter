// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultiplayerChallenge : ModuleRules
{
    public MultiplayerChallenge(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "MultiplayerSessions" });

        //PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Plugins/MultiplayerSessions"));
    }
}
