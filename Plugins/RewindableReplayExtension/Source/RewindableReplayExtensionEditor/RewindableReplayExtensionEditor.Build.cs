using UnrealBuildTool;

public class RewindableReplayExtensionEditor : ModuleRules
{
    public RewindableReplayExtensionEditor(TargetInfo Target)
    {
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
                "LevelEditor"
            }
        );
    }
}