using UnrealBuildTool;

public class SupermanLoisTarget : TargetRules
{
	public SupermanLoisTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5Latest;
		ExtraModuleNames.Add("SupermanLois");
	}
}
