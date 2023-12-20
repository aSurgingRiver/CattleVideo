// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using System.Text;
using System;
using UnrealBuildTool;

public class CattleVideo : ModuleRules
{
	public CattleVideo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"FFmpegForUe",
				"CattleVideoBuildGuide",
				"CattleVideoCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		if (Target.bBuildEditor == true)
		{
			PrivateIncludePathModuleNames.AddRange(
				new string[] {
						"UnrealEd",
				}
			);
			PrivateDependencyModuleNames.AddRange(
				new string[] {
						"UnrealEd",
				}
			);
		}
        string projFile = Target.ProjectFile.ToString();
        ConfigGame(Path.GetDirectoryName(projFile));
    }
	void ConfigGame(string ProjectDir) {
        string GamePath = Path.Combine(ProjectDir, "Config");
        string GameCfg = Path.Combine(GamePath, "DefaultGame.ini");
        if (!Directory.Exists(GamePath))
        {
            Directory.CreateDirectory(GamePath);
        }
        if (!File.Exists(GameCfg))
        {
            File.Create(GameCfg);
        }
        //if( File.OpenWrite(GameCfg)) return ;
        string content;
        try { content = File.ReadAllText(GameCfg/*, Encoding.UTF8*/); }
        catch
        {//
            return;
        }
        string licensePak = "+DirectoriesToAlwaysStageAsUFS=(Path=\"license\")";
        string licenseNode = "[/Script/UnrealEd.ProjectPackagingSettings]";
        if (content.Contains(licenseNode))
        {
            if (content.Contains(licensePak))
            {
                Console.WriteLine(GameCfg + " has configure!");
                return;//
            }
            content = content.Replace(licenseNode, licenseNode + "\n" + licensePak);
        }
        else
        {
            content += "\n\n" + licenseNode + "\n" + licensePak;
        }
        File.WriteAllText(GameCfg, content, Encoding.UTF8);
    }
}
