using System.IO;
using UnrealBuildTool;

public class UAssetDifferEditor : ModuleRules
{
    public UAssetDifferEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine", "Slate", "SlateCore",  "Projects", "Kismet", "ApplicationCore", "InputCore",
                "ToolMenus", "MainFrame", "DesktopPlatform", "Blutility", "AssetTools", "ContentBrowser", "EditorStyle", "EditorSubsystem",
                "GraphEditor", "UnrealEd", "BlueprintGraph", "DataTableEditor", "DeveloperSettings", "UMG"
            }
        );
        
        string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
        PrivateIncludePaths.AddRange(new string[] {Path.Combine(EngineDir, "Source/Editor/DataTableEditor/Private")});
        PrivateIncludePaths.AddRange(new string[] {Path.Combine(EngineDir, "Source/Editor/Kismet/Private")});
    }
}