// Denis Igorevich Frolov did all this. Once there. All things reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RoguelikeThingEditorTarget : TargetRules
{
	public RoguelikeThingEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "RoguelikeThing" } );
	}
}
