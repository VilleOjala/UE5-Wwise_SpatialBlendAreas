/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#include "BlendArea.h"

ABlendArea::ABlendArea()
	:BlendDistance(0.0), Priority(0)
{
}

void ABlendArea::BeginPlay()
{
	Super::BeginPlay();	
	BlendDistance = BlendDistance < 0 ? 0.0 : BlendDistance;
}

void ABlendArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
