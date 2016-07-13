using UnrealBuildTool;

public class RewindableReplayExtensionEditor : ModuleRules
{
    public RewindableReplayExtensionEditor(TargetInfo Target)
    {
        PrivateIncludePaths.AddRange(
            new string[] {
                "RewindableReplayExtension/Private",
                // ... add other private include paths required here ...
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "UnrealEd",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "InputCore",
                "SlateCore",
                "PropertyEditor",
                "LevelEditor",
                "Projects"
            }
        );
    }
}