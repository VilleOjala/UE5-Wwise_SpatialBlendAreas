/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#include "VerticalBlendArea.h"

AVerticalBlendArea::AVerticalBlendArea()
	:BlendStartHeight(0.)
{
}

void AVerticalBlendArea::BeginPlay()
{
	Super::BeginPlay();
}

void AVerticalBlendArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AVerticalBlendArea::GetBlendWeight(const FVector& Point) const
{
	if (!IsInside(Point))
	{
		return 0;
	}

	const double& Height = Point.Z;

	if (Height < BlendStartHeight)
	{
		return 0;
	}
	
	const double BlendDist = BlendDistance >= 0 ? BlendDistance : 0.;
	const double BlendMaxHeight = BlendStartHeight + BlendDist;

	if (Height >= BlendMaxHeight)
	{
		return 1;
	}

	if (BlendDist > 0)
	{
		return (Height - BlendStartHeight) / BlendDist;
	}

	return 1;
}

#if WITH_EDITOR

void AVerticalBlendArea::DebugDraw() const
{
	Super::DebugDraw();

	if (bEditorDebugVerticalBlend)
	{
		for (const auto& Point : Points)
		{
			FVector Min = FVector(Point.X, Point.Y, BlendStartHeight);
			FVector Max = FVector(Point.X, Point.Y, BlendStartHeight + (BlendDistance >= 0 ? BlendDistance : 0.0));

			DrawDebugLine(this->GetWorld(), Min, Max, EditorDebugBlendColor);
			DrawDebugPoint(this->GetWorld(), Min, 10.f, EditorDebugBlendColor);
			DrawDebugPoint(this->GetWorld(), Max, 10.f, EditorDebugBlendColor);
		}
	}
}
#endif
