// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.IO.Compression;

// Tools.DotNETCommon ;
// EpicGames.Core ;
// using EpicGames.Core;

public class CattleVideoCore : ModuleRules
{
	public CattleVideoCore(ReadOnlyTargetRules Target) : base(Target)
	{
        //OptimizeCode = CodeOptimization.Never;
        bPrecompile = true;
        bEnableExceptions = true;
        bUsePrecompiled = !bPrecompile;
        if (bUsePrecompiled)
        {
            PrecompileForTargets = PrecompileTargetsType.None;
        }
		else{
		    PrecompileForTargets = PrecompileTargetsType.Any;
		}
        //if(isDependPlugin("OpenCV")) {// use for test
        //    PrivateDependencyModuleNames.AddRange( new string[] { "OpenCV", "OpenCVHelper" }  );
        //}
		Int64 ver = Target.Version.MajorVersion * 10000 + Target.Version.MinorVersion * 100 + Target.Version.PatchVersion;
        PublicDefinitions.Add("UE_FF_VERSION=" + ver); //


        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDefinitions.Add("FFMPEG_TEST=" + 0 ); //

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
				"Slate",
				"SlateCore",
				"Core",
				"CoreUObject",
				"ApplicationCore",
				"RHI",
				"RenderCore",
				"InputCore",
				"Serialization",
				"MediaUtils",
				"OpenSSL",
				"ImageWrapper",
				"Engine",
				"FFmpegForUe",
                "Projects",
                "AudioMixer",
                //"Launch",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
    }
/*
    bool isDependPlugin(string plugin)
    {
        bool hasDep = false;
        FileReference pluginFile = new FileReference(Path.Combine(PluginDirectory, "CattleVideo.uplugin"));
        PluginInfo Plugin = new PluginInfo(pluginFile, PluginType.Project);
        foreach (PluginReferenceDescriptor desc in Plugin.Descriptor.Plugins)
        {
            if (desc.Name != plugin) continue;
            hasDep = desc.bEnabled;
            break;
        }
        return hasDep;
    }*/
}
