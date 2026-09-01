using UnrealBuildTool;

public class LocalServerConnect : ModuleRules
{
    public LocalServerConnect(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Slate",
            "SlateCore",
            "UMG",
            "FactoryGame",
            "SML",
            "OnlineIntegration"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Projects"
        });
    }
}
