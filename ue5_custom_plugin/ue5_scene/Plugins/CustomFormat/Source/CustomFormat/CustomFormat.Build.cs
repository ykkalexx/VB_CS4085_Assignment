using UnrealBuildTool;

public class CustomFormat : ModuleRules {
    public CustomFormat(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
              "C:\\Assignments\\VB_CS4085_Assignment\\ue5_custom_plugin\\ue5_scene\\Plugins\\CustomFormat\\Source\\CustomFormat\\Public"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                "C:\\Assignments\\VB_CS4085_Assignment\\ue5_custom_plugin\\ue5_scene\\Plugins\\CustomFormat\\Source\\CustomFormat\\Private"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
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
                "MeshDescription",
                "StaticMeshDescription"
            }
        );
    }
}