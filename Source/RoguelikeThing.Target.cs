// Denis Igorevich Frolov did all this. Once there. All things reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RoguelikeThingTarget : TargetRules
{
	public RoguelikeThingTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "RoguelikeThing" } );
	}
}
