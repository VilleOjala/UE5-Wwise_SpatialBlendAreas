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
#include "UObject/Interface.h"
#include "BlendArea.h"
#include "BlendWeightInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBlendWeightInterface : public UInterface
{
	GENERATED_BODY()
};

class SPATIALBLENDAREAS_API IBlendWeightInterface
{
	GENERATED_BODY()

public:

	const virtual TSet<const ABlendArea*>& GetBlendAreas() const = 0;
	virtual void SetWeight(const float& Weight) = 0;
	virtual bool GetWeight(float& OutWeight) const = 0; 
};
