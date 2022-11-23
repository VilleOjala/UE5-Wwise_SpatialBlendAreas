/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSpatialBlendAreasModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
