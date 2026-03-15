using UnrealBuildTool;

public class Heretic2Remake : ModuleRules
{
    public Heretic2Remake(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] { }
        );

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AnimGraphRuntime",
            "EnhancedInput",
            "UMG",
            "Slate",
            "SlateCore",

            // AI
            "AIModule",
            "NavigationSystem",
            "GameplayTasks"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}