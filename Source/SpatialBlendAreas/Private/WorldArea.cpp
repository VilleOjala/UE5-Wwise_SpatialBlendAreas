/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#include "WorldArea.h"
#include "Components/SplineComponent.h"

AWorldArea::AWorldArea()
{
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITOR
	PrimaryActorTick.bCanEverTick = true;
#endif

	InitializeSplineComponent();
	SetRootComponent(SplineComponent);
	SetCanBeDamaged(false);
}

void AWorldArea::InitializeSplineComponent()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(FName(TEXT("Spline")));

	// A closed spline loop defines the polygon area.
	SplineComponent->SetClosedLoop(true);

	// Interpolation mode should always be 'Linear' for the containment algorithm to work correctly.
	// Do not change this setting for any of the spline input keys from the editor properties.
	for (auto& Point : SplineComponent->SplineCurves.Position.Points)
	{
		Point.InterpMode = EInterpCurveMode::CIM_Linear;
	}

	for (auto& point : SplineComponent->SplineCurves.Rotation.Points)
	{
		point.InterpMode = EInterpCurveMode::CIM_Linear;
	}

	for (auto& point : SplineComponent->SplineCurves.Scale.Points)
	{
		point.InterpMode = EInterpCurveMode::CIM_Linear;
	}

	SplineComponent->SplineCurves.UpdateSpline();
}

void AWorldArea::BeginPlay()
{
	Super::BeginPlay();
	InitializeArea();
}

void AWorldArea::InitializeArea()
{
	if (!IsValid(SplineComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldArea is missing its SplineComponent."))
		return;
	}

	const int32 PointCount = SplineComponent->GetNumberOfSplinePoints();

	if (PointCount < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("A minimum of three spline points is required for defining a WorldArea."))
		return;
	}

	// For some reason, the editor automatically adds three FVector2D elements to a TArray tagged with a UPROPERTY(),
	// so first empty the container before retrieving the spline points.
	Points.Empty();

	// Store the world positions of spline input keys as 2D vectors, since the containment tests are done on an XY-plane.
	for (int32 Index = 0; Index < PointCount; Index++)
	{
		const FVector Position3D = SplineComponent->GetLocationAtSplineInputKey(Index, ESplineCoordinateSpace::World);
		const FVector2D Position2D = FVector2D(Position3D);
		Points.Emplace(Position2D);
	}
}

void AWorldArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	DebugDraw();
#endif
}

bool AWorldArea::IsInside(const FVector2D& Point) const
{
	return IsInsideWorldArea(Point);
}

bool AWorldArea::IsInside(const FVector& Point) const
{
	return IsInsideWorldArea(FVector2D(Point.X, Point.Y));
}

bool AWorldArea::IsInsideWorldArea(const FVector2D& Point) const
{
	if (Points.Num() < 3)
	{
		return false;
	}

	// Solve the point-in-polygon problem with the even-odd rule algorithm: https://en.wikipedia.org/wiki/Point_in_polygon
	// If a ray shot from the point to the infinity (in a practical sense) crosses an odd number of polygon line segments,
	// the point resides inside the polygon.

	const FVector2D& A1 = Point;
	const FVector2D A2 = FVector2D(A1.X, RayLength);
	uint32 IntersectCount = 0;

	for (int32 Index = 0; Index < Points.Num(); Index++)
	{
		const FVector2D& B1 = Points[Index];
		const FVector2D& B2 = Points[Index == Points.Num() - 1 ? 0 : Index + 1];

		// Line intersection algorithm: https://www.dcs.gla.ac.uk/~pat/52233/slides/Geometry1x1.pdf

		if (AreIntersecting(A1, A2, B1, B2))
		{
			if (GetOrientation(B1, A1, B2) == EOrientation::Colinear)
			{
				return IsPointOnLine(A2, B2, A1);
			}

			IntersectCount++;
		}
	}

	return IntersectCount % 2 == 1;
}

bool AWorldArea::AreIntersecting(const FVector2D& A1, const FVector2D& A2, const FVector2D& B1, const FVector2D& B2) const
{
	const EOrientation Ori1 = GetOrientation(A1, A2, B1);
	const EOrientation Ori2 = GetOrientation(A1, A2, B2);
	const EOrientation Ori3 = GetOrientation(B1, B2, A1);
	const EOrientation Ori4 = GetOrientation(B1, B2, A2);

	if (Ori1 != Ori2 && Ori3 != Ori4)
	{
		auto Cross2D = [](const FVector2D& lhs, const FVector2D& rhs)
		{
			return lhs.X * rhs.Y - lhs.Y * rhs.X;
		};

		const FVector2D R = A2 - A1;
		const FVector2D S = B2 - B1;

		// We already know that there is a default type of intersection between the two lines,
		// thus solve only for 'T' in 'A1 + T * R = B1 + U * R'  to get the intersection point.
		const double T = Cross2D((B1 - A1), S) / Cross2D(R, S);
		const FVector2D IntersectionPoint = A1 + T * R;

		// Check for the special case in which the ray passes directly through a vertex, 
		// and thus dy default would increment the intersection count twice.
		// Increment the intersection count only if the other end of the inspected segment is above the ray.
		if ((IntersectionPoint == B1 && B2.X < A1.X) || (IntersectionPoint == B2 && B1.X < A1.X))
		{
			return false;
		}

		return true;
	}	
	else if (Ori1 == EOrientation::Colinear && IsPointOnLine(A1, A2, B1))
	{
		return true;
	}
	else if (Ori2 == EOrientation::Colinear && IsPointOnLine(A1, A2, B2))
	{
		return true;
	}
	else if (Ori3 == EOrientation::Colinear && IsPointOnLine(B1, B2, A1))
	{
		return true;
	}
	else if (Ori4 == EOrientation::Colinear && IsPointOnLine(B1, B2, A2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

AWorldArea::EOrientation AWorldArea::GetOrientation(const FVector2D& P1, const FVector2D& P2, const FVector2D& P3) const
{
	const double Result = (P2.Y - P1.Y) * (P3.X - P2.X) - (P3.Y - P2.Y) * (P2.X - P1.X);

	if (Result == 0)
	{
		return EOrientation::Colinear;
	}
	else if (Result < 0)
	{
		return EOrientation::Counterclockwise;
	}
	else
	{
		return EOrientation::Clockwise;
	}
}

bool AWorldArea::IsPointOnLine(const FVector2D& LineStart, const FVector2D& LineEnd, const FVector2D& Point) const
{
	if (Point.X <= FMath::Max(LineStart.X, LineEnd.X) && Point.X >= FMath::Min(LineStart.X, LineEnd.X) &&
		Point.Y <= FMath::Max(LineStart.Y, LineEnd.Y) && Point.Y >= FMath::Min(LineStart.Y, LineEnd.Y))
	{
		return true;
	}

	return false;
}

bool AWorldArea::GetClosestPointAndDistanceSquared(const FVector2D& Point, FVector2D& OutClosestPoint, double& OutDistanceSquared) const
{
	if (Points.Num() == 0)
	{
		return false;
	}
	else if (Points.Num() == 1)
	{
		OutClosestPoint = Points[0];
		OutDistanceSquared = FVector2D::DistSquared(OutClosestPoint, Point);
		return true;
	}

	bool FirstEntryHandled = false;

	for (int32 Index = 0; Index < Points.Num(); Index++)
	{
		const FVector2D& LineStart = Points[Index];
		const FVector2D& LineEnd = Points[Index == Points.Num() - 1 ? 0 : Index + 1];

		FVector2D ClosestPointOnSegment = FMath::ClosestPointOnSegment2D(Point, LineStart, LineEnd);
		double DistanceSquared = FVector2D::DistSquared(Point, ClosestPointOnSegment);

		if (!FirstEntryHandled || DistanceSquared < OutDistanceSquared)
		{
			OutDistanceSquared = DistanceSquared;
			OutClosestPoint = ClosestPointOnSegment;
			FirstEntryHandled = true;
		}
	}

	return true;
}

#if WITH_EDITOR

void AWorldArea::DebugDraw() const
{
	if (bEditorDebugDrawArea && Points.Num() >= 3)
	{
		for (int32 Index = 0; Index < Points.Num(); Index++)
		{
			const FVector2D& PointA = Points[Index];
			const FVector2D& PointB = Points[Index == Points.Num() - 1 ? 0 : Index + 1];

			const FVector LineStart = FVector(PointA.X, PointA.Y, EditorDrawHeight);
			const FVector LineEnd = FVector(PointB.X, PointB.Y, EditorDrawHeight);

			DrawDebugLine(this->GetWorld(), LineStart, LineEnd, EditorDrawColor);
			DrawDebugPoint(this->GetWorld(), LineStart, 10.f, EditorDrawColor);
		}
	}
}
#endif
