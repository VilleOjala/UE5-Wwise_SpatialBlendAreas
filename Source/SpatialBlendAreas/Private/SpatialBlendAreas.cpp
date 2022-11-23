/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#include "SpatialBlendAreas.h"

#define LOCTEXT_NAMESPACE "FSpatialBlendAreasModule"

void FSpatialBlendAreasModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSpatialBlendAreasModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSpatialBlendAreasModule, SpatialBlendAreas)