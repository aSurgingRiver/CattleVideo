// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.IO.Compression;
using System.Reflection;

public class FFmpegForUe : ModuleRules
{
    public FFmpegForUe(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        //PrintConfig("FFmpegLib");
        string projFile = "" + Target.ProjectFile;
        if (Target.Platform == UnrealTargetPlatform.Win64) {
            //IniFFmpeg_Win("6.0", "" + Target.Platform, PluginDirectory);
            IniFFmpeg_Win("6", "0","" + Target.Platform, PluginDirectory);
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            IniFFmpeg_Linux("6", "0", "" + Target.Platform, PluginDirectory);
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            if ((""+Target.Architecture) == "Arm64")
            {
                IniFFmpeg_Android("4", "4", "Android", PluginDirectory);
            }
            else
            {
                IniFFmpeg_Android("4", "4", "Android_x86-64", PluginDirectory);
            }
        }
        else
        {
            PrintConfig("FFmpegForUe");
            return;
        }
    }
    
    void IniFFmpeg_Win(string major,string minor,string Plat, string projectDir)
    {
        string CEFVersion = major + "." + minor;
        string PlatRoot = Path.Combine(ModuleDirectory, CEFVersion, Plat);
        string LibraryPath = Path.Combine(PlatRoot, "lib");
        PublicSystemIncludePaths.Add(Path.Combine(PlatRoot, "include")); 
        PublicDefinitions.Add("FFMPEG_MAJOR=" + major + ""); //
        PublicDefinitions.Add("FFMPEG_MINOR=" + minor + ""); //
        PublicDefinitions.Add("FFMPEG_PLATFORM=\"" + Plat + "\""); //
        //string BinPath = Path.Combine(projectDir, "Binaries", Plat);
        //if (!Directory.Exists(BinPath)) {
        //    Directory.CreateDirectory(BinPath);
        //}
        foreach (string FileName in Directory.EnumerateFiles(LibraryPath, "*.dll", SearchOption.TopDirectoryOnly)) {
            //string BinFile = Path.Combine(BinPath, System.IO.Path.GetFileName(FileName));
            RuntimeDependencies.Add(FileName);
        }
    }
    void IniFFmpeg_Linux(string major, string minor, string Plat, string projectDir)
    {
        string CEFVersion = major + "." + minor;
        string PlatRoot = Path.Combine(ModuleDirectory, CEFVersion, Plat);
        string LibraryPath = Path.Combine(PlatRoot, "lib");
        PublicSystemIncludePaths.Add(Path.Combine(PlatRoot, "include"));
        PublicDefinitions.Add("FFMPEG_MAJOR=" + major + ""); //
        PublicDefinitions.Add("FFMPEG_MINOR=" + minor + ""); //
        PublicDefinitions.Add("FFMPEG_PLATFORM=\"" + Plat + "\""); //
        string BinPath = Path.Combine(projectDir, "Binaries", Plat);
        if (!Directory.Exists(BinPath))
        {
            Directory.CreateDirectory(BinPath);
        }
        foreach (string FileName in Directory.EnumerateFiles(LibraryPath, "*.so", SearchOption.TopDirectoryOnly))
        {
            RuntimeDependencies.Add(FileName);
        }
        foreach (string FileName in Directory.EnumerateFiles(LibraryPath, "*.so.1.1", SearchOption.TopDirectoryOnly))
        {
            RuntimeDependencies.Add(FileName);
        }
    }
    void IniFFmpeg_Android(string major, string minor, string Plat, string projectDir)
    {
        string CEFVersion = major + "." + minor;
        string PlatRoot = Path.Combine(ModuleDirectory, CEFVersion, Plat);
        string LibraryPath = Path.Combine(PlatRoot, "lib");
        PublicSystemIncludePaths.Add(Path.Combine(PlatRoot, "include"));
        PublicDefinitions.Add("FFMPEG_MAJOR=" + major + ""); //
        PublicDefinitions.Add("FFMPEG_MINOR=" + minor + ""); //
        PublicDefinitions.Add("FFMPEG_PLATFORM=\"" + Plat + "\""); //
        foreach (string FileName in Directory.EnumerateFiles(LibraryPath, "*.a", SearchOption.TopDirectoryOnly))
        {
            PublicAdditionalLibraries.Add(FileName);
            //RuntimeDependencies.Add(FileName);
        }
    }
    void PrintConfig(string Module) {
        //Console.WriteLine("==================="+ Module + " Begin ===========================");
        //Console.WriteLine("Name=" + Target.Name);
        //Console.WriteLine("Platform=" + Target.Platform);
        //Console.WriteLine("Configuration=" + Target.Configuration);
        //Console.WriteLine("Architecture=" + Target.Architecture);
        //Console.WriteLine("ProjectFile="  + Target.ProjectFile);
        //Console.WriteLine("Version.MajorVersion=" + Target.Version.MajorVersion);
        //Console.WriteLine("Version.MinorVersion=" + Target.Version.MinorVersion);
        //Console.WriteLine("Version.PatchVersion=" + Target.Version.PatchVersion);
        //Console.WriteLine("Type=" + Target.Type);
        //Console.WriteLine("DefaultBuildSettings=" + Target.DefaultBuildSettings);
        //Console.WriteLine("bUsesSteam=" + Target.bUsesSteam);
        //Console.WriteLine("bUsesCEF3=" + Target.bUsesCEF3);
        //Console.WriteLine("bUsesSlate=" + Target.bUsesSlate);
        //Console.WriteLine("bUseStaticCRT=" + Target.bUseStaticCRT);
        //Console.WriteLine("bDebugBuildsActuallyUseDebugCRT=" + Target.bDebugBuildsActuallyUseDebugCRT);
        //Console.WriteLine("bLegalToDistributeBinary=" + Target.bLegalToDistributeBinary);
        //Console.WriteLine("UndecoratedConfiguration=" + Target.UndecoratedConfiguration);
        //Console.WriteLine("bAllowHotReload=" + Target.bAllowHotReload);
        //Console.WriteLine("bBuildAllModules=" + Target.bBuildAllModules);
        //Console.WriteLine("PakSigningKeysFile=" + Target.PakSigningKeysFile);
        //Console.WriteLine("SolutionDirectory=" + Target.SolutionDirectory);
        //Console.WriteLine("bBuildInSolutionByDefault=" + Target.bBuildInSolutionByDefault);
        //Console.WriteLine("ExeBinariesSubFolder=" + Target.ExeBinariesSubFolder);
        //Console.WriteLine("GeneratedCodeVersion=" + Target.GeneratedCodeVersion);
        //Console.WriteLine("bEnableMeshEditor=" + Target.bEnableMeshEditor);
        //Console.WriteLine("bUseChaosMemoryTracking=" + Target.bUseChaosMemoryTracking);
        //Console.WriteLine("bUseChaosChecked=" + Target.bUseChaosChecked);
        //Console.WriteLine("bCompileICU=" + Target.bCompileICU);
        //Console.WriteLine("bCompileCEF3=" + Target.bCompileCEF3);
        //Console.WriteLine("bCompileISPC=" + Target.bCompileISPC);
        //Console.WriteLine("bUseChaosChecked=" + Target.bUseChaosChecked);
        //Console.WriteLine("bBuildEditor=" + Target.bBuildEditor);
        //Console.WriteLine("bBuildRequiresCookedData=" + Target.bBuildRequiresCookedData);
        //Console.WriteLine("bBuildWithEditorOnlyData=" + Target.bBuildWithEditorOnlyData);
        //Console.WriteLine("bBuildDeveloperTools=" + Target.bBuildDeveloperTools);
        //Console.WriteLine("bForceBuildTargetPlatforms=" + Target.bForceBuildTargetPlatforms);
        //Console.WriteLine("bForceBuildShaderFormats=" + Target.bForceBuildShaderFormats);
        //Console.WriteLine("bCompileCustomSQLitePlatform=" + Target.bCompileCustomSQLitePlatform);
        //Console.WriteLine("bUseCacheFreedOSAllocs=" + Target.bUseCacheFreedOSAllocs);
        //Console.WriteLine("bCompileAgainstEngine=" + Target.bCompileAgainstEngine);
        //Console.WriteLine("bCompileAgainstCoreUObject=" + Target.bCompileAgainstCoreUObject);
        //Console.WriteLine("bCompileAgainstApplicationCore=" + Target.bCompileAgainstApplicationCore);
        //Console.WriteLine("bCompileRecast=" + Target.bCompileRecast);
        //Console.WriteLine("bCompileNavmeshSegmentLinks=" + Target.bCompileNavmeshSegmentLinks);
        //Console.WriteLine("bCompileNavmeshClusterLinks=" + Target.bCompileNavmeshClusterLinks);
        //Console.WriteLine("bCompileSpeedTree=" + Target.bCompileSpeedTree);
        //Console.WriteLine("bForceEnableExceptions=" + Target.bForceEnableExceptions);
        //Console.WriteLine("bForceEnableObjCExceptions=" + Target.bForceEnableObjCExceptions);
        //Console.WriteLine("bForceEnableRTTI=" + Target.bForceEnableRTTI);
        //Console.WriteLine("bUseInlining=" + Target.bUseInlining);
        //Console.WriteLine("bWithServerCode=" + Target.bWithServerCode);
        //Console.WriteLine("bWithPushModel=" + Target.bWithPushModel);
        //Console.WriteLine("bCompileWithStatsWithoutEngine=" + Target.bCompileWithStatsWithoutEngine);
        //Console.WriteLine("bCompileWithPluginSupport=" + Target.bCompileWithPluginSupport);
        //Console.WriteLine("bIncludePluginsForTargetPlatforms=" + Target.bIncludePluginsForTargetPlatforms);
        //Console.WriteLine("bCompileWithAccessibilitySupport=" + Target.bCompileWithAccessibilitySupport);
        //Console.WriteLine("bWithPerfCounters=" + Target.bWithPerfCounters);
        //Console.WriteLine("bWithLiveCoding=" + Target.bWithLiveCoding);
        //Console.WriteLine("bUseDebugLiveCodingConsole=" + Target.bUseDebugLiveCodingConsole);
        //Console.WriteLine("bWithDirectXMath=" + Target.bWithDirectXMath);
        //Console.WriteLine("bUseLoggingInShipping=" + Target.bUseLoggingInShipping);
        //Console.WriteLine("bLoggingToMemoryEnabled=" + Target.bLoggingToMemoryEnabled);
        //Console.WriteLine("bUseLauncherChecks=" + Target.bUseLauncherChecks);
        //Console.WriteLine("bUseChecksInShipping=" + Target.bUseChecksInShipping);
        //Console.WriteLine("bUseEstimatedUtcNow=" + Target.bUseEstimatedUtcNow);
        //Console.WriteLine("bCompileFreeType=" + Target.bCompileFreeType);
        //Console.WriteLine("bForceCompileDevelopmentAutomationTests=" + Target.bForceCompileDevelopmentAutomationTests);
        //Console.WriteLine("bForceCompilePerformanceAutomationTests=" + Target.bForceCompilePerformanceAutomationTests);
        //Console.WriteLine("bUseXGEController=" + Target.bUseXGEController);
        //Console.WriteLine("bEventDrivenLoader=" + Target.bEventDrivenLoader);
        //Console.WriteLine("bIWYU=" + Target.bIWYU);
        //Console.WriteLine("bEnforceIWYU=" + Target.bEnforceIWYU);
        //Console.WriteLine("bHasExports=" + Target.bHasExports);
        //Console.WriteLine("bPrecompile=" + Target.bPrecompile);
        //Console.WriteLine("bEnableOSX109Support=" + Target.bEnableOSX109Support);
        //Console.WriteLine("bIsBuildingConsoleApplication=" + Target.bIsBuildingConsoleApplication);
        //Console.WriteLine("bBuildAdditionalConsoleApp=" + Target.bBuildAdditionalConsoleApp);
        //Console.WriteLine("bDisableSymbolCache=" + Target.bDisableSymbolCache);
        //Console.WriteLine("bUseUnityBuild=" + Target.bUseUnityBuild);
        //Console.WriteLine("bAdaptiveUnityDisablesOptimizations=" + Target.bAdaptiveUnityDisablesOptimizations);
        //Console.WriteLine("bAdaptiveUnityDisablesPCH=" + Target.bAdaptiveUnityDisablesPCH);
        //Console.WriteLine("bAdaptiveUnityDisablesPCHForProject=" + Target.bAdaptiveUnityDisablesPCHForProject);
        //Console.WriteLine("bAdaptiveUnityCreatesDedicatedPCH=" + Target.bAdaptiveUnityCreatesDedicatedPCH);
        //Console.WriteLine("bAdaptiveUnityEnablesEditAndContinue=" + Target.bAdaptiveUnityEnablesEditAndContinue);
        //Console.WriteLine("MinGameModuleSourceFilesForUnityBuild=" + Target.MinGameModuleSourceFilesForUnityBuild);
        //Console.WriteLine("ShadowVariableWarningLevel=" + Target.ShadowVariableWarningLevel);
        //Console.WriteLine("UnsafeTypeCastWarningLevel=" + Target.UnsafeTypeCastWarningLevel);
        //Console.WriteLine("bUndefinedIdentifierErrors=" + Target.bUndefinedIdentifierErrors);
        //Console.WriteLine("bUseFastMonoCalls=" + Target.bUseFastMonoCalls);
        //Console.WriteLine("NumIncludedBytesPerUnityCPP=" + Target.NumIncludedBytesPerUnityCPP);
        //Console.WriteLine("bStressTestUnity=" + Target.bStressTestUnity);
        //Console.WriteLine("bDisableDebugInfo=" + Target.bDisableDebugInfo);
        //Console.WriteLine("bDisableDebugInfoForGeneratedCode=" + Target.bDisableDebugInfoForGeneratedCode);
        //Console.WriteLine("bOmitPCDebugInfoInDevelopment=" + Target.bOmitPCDebugInfoInDevelopment);
        //Console.WriteLine("bUsePDBFiles=" + Target.bUsePDBFiles);
        //Console.WriteLine("bUsePCHFiles=" + Target.bUsePCHFiles);
        //Console.WriteLine("bPreprocessOnly=" + Target.bPreprocessOnly);
        //Console.WriteLine("MinFilesUsingPrecompiledHeader=" + Target.MinFilesUsingPrecompiledHeader);
        //Console.WriteLine("bForcePrecompiledHeaderForGameModules=" + Target.bForcePrecompiledHeaderForGameModules);
        //Console.WriteLine("bUseIncrementalLinking=" + Target.bUseIncrementalLinking);
        //Console.WriteLine("bAllowLTCG=" + Target.bAllowLTCG);
        //Console.WriteLine("bPGOProfile=" + Target.bPGOProfile);
        //Console.WriteLine("bPGOOptimize=" + Target.bPGOOptimize);
        //Console.WriteLine("bSupportEditAndContinue=" + Target.bSupportEditAndContinue);
        //Console.WriteLine("bOmitFramePointers=" + Target.bOmitFramePointers);
        //Console.WriteLine("bUseMallocProfiler=" + Target.bUseMallocProfiler);
        //Console.WriteLine("bUseSharedPCHs=" + Target.bUseSharedPCHs);
        //Console.WriteLine("bUseShippingPhysXLibraries=" + Target.bUseShippingPhysXLibraries);
        //Console.WriteLine("bCheckLicenseViolations=" + Target.bCheckLicenseViolations);
        //Console.WriteLine("bBreakBuildOnLicenseViolation=" + Target.bBreakBuildOnLicenseViolation);
        //Console.WriteLine("bUseFastPDBLinking=" + Target.bUseFastPDBLinking);
        //Console.WriteLine("bCreateMapFile=" + Target.bCreateMapFile);
        //Console.WriteLine("bAllowRuntimeSymbolFiles=" + Target.bAllowRuntimeSymbolFiles);
        //Console.WriteLine("BundleVersion=" + Target.BundleVersion);
        //Console.WriteLine("bDeployAfterCompile=" + Target.bDeployAfterCompile);
        //Console.WriteLine("bAllowRemotelyCompiledPCHs=" + Target.bAllowRemotelyCompiledPCHs);
        //Console.WriteLine("bCheckSystemHeadersForModification=" + Target.bCheckSystemHeadersForModification);
        //Console.WriteLine("bDisableLinking=" + Target.bDisableLinking);
        //Console.WriteLine("bFormalBuild=" + Target.bFormalBuild);
        //Console.WriteLine("bUseAdaptiveUnityBuild=" + Target.bUseAdaptiveUnityBuild);
        //Console.WriteLine("bFlushBuildDirOnRemoteMac=" + Target.bFlushBuildDirOnRemoteMac);
        //Console.WriteLine("bPrintToolChainTimingInfo=" + Target.bPrintToolChainTimingInfo);
        //Console.WriteLine("bParseTimingInfoForTracing=" + Target.bParseTimingInfoForTracing);
        //Console.WriteLine("bPublicSymbolsByDefault=" + Target.bPublicSymbolsByDefault);
        //Console.WriteLine("ToolChainName=" + Target.ToolChainName);
        //Console.WriteLine("bLegacyPublicIncludePaths=" + Target.bLegacyPublicIncludePaths);
        //Console.WriteLine("CppStandard=" + Target.CppStandard);
        //Console.WriteLine("BuildVersion=" + Target.BuildVersion);
        //Console.WriteLine("LinkType=" + Target.LinkType);
        //Console.WriteLine("LaunchModuleName=" + Target.LaunchModuleName);
        //Console.WriteLine("ExportPublicHeader=" + Target.ExportPublicHeader);
        //Console.WriteLine("BuildEnvironment=" + Target.BuildEnvironment);
        //Console.WriteLine("bOverrideBuildEnvironment=" + Target.bOverrideBuildEnvironment);
        //Console.WriteLine("AdditionalCompilerArguments=" + Target.AdditionalCompilerArguments);
        //Console.WriteLine("AdditionalLinkerArguments=" + Target.AdditionalLinkerArguments);
        //Console.WriteLine("GeneratedProjectName=" + Target.GeneratedProjectName);
        //Console.WriteLine("AndroidPlatform=" + Target.AndroidPlatform);
        //Console.WriteLine("LinuxPlatform=" + Target.LinuxPlatform);
        //Console.WriteLine("IOSPlatform=" + Target.IOSPlatform);
        //Console.WriteLine("MacPlatform=" + Target.MacPlatform);
        //Console.WriteLine("WindowsPlatform=" + Target.WindowsPlatform);
        ////Console.WriteLine("HoloLensPlatform=" + Target.HoloLensPlatform);
        //Console.WriteLine("bShouldCompileAsDLL=" + Target.bShouldCompileAsDLL);
        //Console.WriteLine("bGenerateProjectFiles=" + Target.bGenerateProjectFiles);
        //Console.WriteLine("bIsEngineInstalled=" + Target.bIsEngineInstalled);
        //Console.WriteLine("RelativeEnginePath=" + Target.RelativeEnginePath);
        //Console.WriteLine("UEThirdPartySourceDirectory=" + Target.UEThirdPartySourceDirectory);
        //Console.WriteLine("UEThirdPartyBinariesDirectory=" + Target.UEThirdPartyBinariesDirectory);
        //Console.WriteLine("===================" + Module + " End ===========================");
    }

}
