// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class format3d : ModuleRules {
    public format3d(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "MeshDescription",
                "StaticMeshDescription",
                "UnrealEd",
                "AssetTools",
                "EditorFramework",
                "AssetRegistry"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "AssetTools",
                "EditorFramework"
            }
        );
    }
}