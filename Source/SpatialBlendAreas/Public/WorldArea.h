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
#include "GameFramework/Actor.h"
#include "WorldArea.generated.h"

UCLASS()
class SPATIALBLENDAREAS_API AWorldArea : public AActor
{
	GENERATED_BODY()
	
public:	

	AWorldArea();

private:

	UPROPERTY(VisibleAnywhere)
	class USplineComponent* SplineComponent;

	const double RayLength = 100000000.0;

	void InitializeSplineComponent();
	void InitializeArea();

protected:

	UPROPERTY()
	TArray<FVector2D> Points;

	virtual void BeginPlay() override;

private:

	/** The orientation of three sequential points in 2D space.*/
	enum class EOrientation : uint8
	{
		Colinear,
		Clockwise,
		Counterclockwise
	};

	bool IsInsideWorldArea(const FVector2D& Point) const;
	bool AreIntersecting(const FVector2D& A1, const FVector2D& A2,const FVector2D& B1, const FVector2D& B2) const;
	EOrientation GetOrientation(const FVector2D& P1, const FVector2D& P2, const FVector2D& P3) const;
	bool IsPointOnLine(const FVector2D& LineStart, const FVector2D& LineEnd, const FVector2D& Point) const;
	
public:	

	virtual void Tick(float DeltaTime) override;

	/**
	 * Checks if a point is inside the defined polygon. Tests on an XY-plane.
	 *
	 * @param Point - Point on the XY-plane
	 */
	bool IsInside(const FVector2D& Point) const;
	bool IsInside(const FVector& Point) const;

	/** 
	* Finds the closest point on the polygon boundary from the provided point and outputs the squared distance to it.
	*
	* @return true if the polygon has at least one defined vertex
	*/
	bool GetClosestPointAndDistanceSquared(const FVector2D& Point, FVector2D& OutClosestPoint, double& OutDistanceSquared) const;

#if WITH_EDITORONLY_DATA
protected:

	UPROPERTY(EditAnywhere)
	bool bEditorDebugDrawArea = false;

	UPROPERTY(EditAnywhere)
	double EditorDrawHeight;

	UPROPERTY(EditAnywhere)
	FColor EditorDrawColor = FColor::Cyan;

#endif

#if WITH_EDITOR
protected:

	virtual void DebugDraw() const;

#endif
};
