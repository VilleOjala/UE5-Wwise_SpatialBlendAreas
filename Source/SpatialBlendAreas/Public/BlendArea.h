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
#include "WorldArea.h"
#include "GameFramework/Actor.h"
#include "BlendArea.generated.h"

/** 
* An abstract base class for AHorizontalBlendArea and AVerticalBlendArea.
*/
UCLASS(Abstract)
class SPATIALBLENDAREAS_API ABlendArea : public AWorldArea
{
	GENERATED_BODY()
	
public:	

	ABlendArea();

protected:

	virtual void BeginPlay() override;

	/** 
	* The distance from a zero point at which the blend weight is 1 when the measurement point is inside the the blend area.
	* For horizontal blend areas, the zero point is the closest point on the 2D-polygon boundary, whereas for vertical
	* blend areas it is a user-defined height in world space.
	*/
	UPROPERTY(EditAnywhere)
	double BlendDistance;

public:	

	/** 
	* The relative priority of this blend area. Used when calculating the blend weight distribution between multiple areas. 
	* The larger the value, the higher the priority. 
	*/
	UPROPERTY(EditAnywhere)
	uint32 Priority;

	virtual void Tick(float DeltaTime) override;
	virtual float GetBlendWeight(const FVector& Point) const PURE_VIRTUAL(ABlendArea::GetBlendWeight, return 0.0;);
};
