/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#include "HorizontalBlendArea.h"

AHorizontalBlendArea::AHorizontalBlendArea()
{
}

void AHorizontalBlendArea::BeginPlay()
{
	Super::BeginPlay();	
}

void AHorizontalBlendArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AHorizontalBlendArea::GetBlendWeight(const FVector& Point) const 
{
	if (!IsInside(Point))
	{
		return 0;
	}
	if (BlendDistance <= 0)
	{
		return 1;
	}

	const FVector2D Point2D = FVector2D(Point.X, Point.Y);
	FVector2D ClosestPoint;
	double DistanceSquared = 0.0;
	GetClosestPointAndDistanceSquared(Point2D, ClosestPoint, DistanceSquared);
	double BlendDistanceSquared = FMath::Square(BlendDistance);
	return FMath::Clamp(DistanceSquared / BlendDistanceSquared, 0, 1);
}

#if WITH_EDITOR

void AHorizontalBlendArea::DebugDraw() const
{
	Super::DebugDraw();

	if (bEditorDebugHorizontalBlend && TestActor->IsValidLowLevel())
	{
		const FVector Location = TestActor->GetActorLocation();
		const FVector2D Location2D = FVector2D(Location.X, Location.Y);

		if (IsInside(Location2D))
		{
			const float BlendWeight = GetBlendWeight(Location);
			FVector2D ClosestPoint;
			double DistanceSquared = 0.0;

			if (GetClosestPointAndDistanceSquared(Location2D, ClosestPoint, DistanceSquared))
			{
				const double Distance = FMath::Sqrt(DistanceSquared);
				const FVector2D Direction = (Location2D - ClosestPoint).GetSafeNormal();
				const double BlendDist = BlendDistance >= 0 ? BlendDistance : 0.;
				const FVector2D FullWeightPosition = ClosestPoint + Direction * BlendDist;

				if (Distance >= BlendDist)
				{
					DrawDebugLine(this->GetWorld(), FVector(ClosestPoint.X, ClosestPoint.Y, EditorDrawHeight), 
													FVector(FullWeightPosition.X, FullWeightPosition.Y, EditorDrawHeight), FColor::Green);

					DrawDebugLine(this->GetWorld(), FVector(FullWeightPosition.X, FullWeightPosition.Y, EditorDrawHeight),
													FVector(Location2D.X, Location2D.Y, EditorDrawHeight), FColor::Transparent);
				}
				else
				{
					DrawDebugLine(this->GetWorld(), FVector(ClosestPoint.X, ClosestPoint.Y, EditorDrawHeight),
													FVector(Location2D.X, Location2D.Y, EditorDrawHeight), FColor::Green);

					DrawDebugLine(this->GetWorld(), FVector(Location2D.X, Location2D.Y, EditorDrawHeight),
													FVector(FullWeightPosition.X, FullWeightPosition.Y, EditorDrawHeight), FColor::Red);
				}

				DrawDebugPoint(this->GetWorld(), FVector(Location2D.X, Location2D.Y, EditorDrawHeight), 10.f, FColor::White);
			}
		}
	}
}
#endif
