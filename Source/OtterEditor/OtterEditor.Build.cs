// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OtterEditor : ModuleRules
{
    public OtterEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"OtterEditor"
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);

		PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "EditorFramework",
                "UnrealEd",
				"PhysicsCore",
				"GameplayTagsEditor",
				"GameplayTasksEditor",
				"GameplayAbilities",
				"GameplayAbilitiesEditor",
				"StudioTelemetry",
				"OtterGame",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
				"InputCore",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"EditorStyle",
				"DataValidation",
				"MessageLog",
				"Projects",
				"DeveloperToolSettings",
				"CollectionManager",
				"SourceControl",
				"Chaos"
			}
        );

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
			}
		);
		// Generate compile errors if using DrawDebug functions in test/shipping builds.
		PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
    }
}
