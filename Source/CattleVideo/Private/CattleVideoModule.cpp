// Copyright Epic Games, Inc. All Rights Reserved.

#include "CattleVideoModule.h"
#include "CattleVideoControl.h"

#define LOCTEXT_NAMESPACE "FCattleVideoModule"

void FCattleVideoModule::StartupModule()
{
#if WITH_EDITOR
	FEditorDelegates::PausePIE.AddStatic(cattlevideo::OnEndPIE);
	FEditorDelegates::BeginPIE.AddStatic(cattlevideo::OnBeginPIE);
	FEditorDelegates::EndPIE.AddStatic(cattlevideo::OnEndPIE);
	FEditorDelegates::ResumePIE.AddStatic(cattlevideo::OnBeginPIE);
#endif
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FCattleVideoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCattleVideoModule, CattleVideo)