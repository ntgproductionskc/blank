using UnrealBuildTool;

public class SupermanLois : ModuleRules
{
	public SupermanLois(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore",
			"EnhancedInput",
			"UMG", "Slate", "SlateCore",
			"AIModule", "NavigationSystem",
			"GameplayAbilities", "GameplayTags", "GameplayTasks",
			"Niagara", "NiagaraCore",
			"PhysicsCore", "Chaos", "ChaosSolverEngine",
			"GeometryCollectionEngine", "FieldSystemEngine",
			"MovieScene", "LevelSequence", "CinematicCamera",
			"AudioMixer", "AudioModulation", "MetasoundEngine",
			"ControlRig", "AnimationCore", "AnimGraphRuntime",
			"PCG", "Landmass",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"RenderCore", "Renderer", "RHI",
			"Json", "JsonUtilities",
			"ApplicationCore",
			"Projects"
		});
	}
}
