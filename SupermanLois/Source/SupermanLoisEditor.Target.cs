using UnrealBuildTool;

public class SupermanLoisEditorTarget : TargetRules
{
	public SupermanLoisEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5Latest;
		ExtraModuleNames.Add("SupermanLois");
	}
}
