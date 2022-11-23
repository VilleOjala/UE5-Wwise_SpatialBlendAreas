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
#include "BlendArea.h"
#include "HorizontalBlendArea.generated.h"

UCLASS()
class SPATIALBLENDAREAS_API AHorizontalBlendArea : public ABlendArea
{
	GENERATED_BODY()
	
public:	

	AHorizontalBlendArea();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	/**
	* Returns a blend weight between 0 and 1 by comparing the 'zero point - input point' -distance to
	* the 'zero point - max point' -distance. 'Zero point' is the closest point on the polygon boundary
	* from the input point. 'Max point' is determined by moving from the 'zero point' towards the input point 
	* the amount specified in the 'BlendDistance' -variable. All checks are done on an XY-plane.
	* If the input point is outside the blend area, the blend weight is 0.
	*/
	virtual float GetBlendWeight(const FVector& Point) const override;

#if WITH_EDITORONLY_DATA
protected:

	UPROPERTY(EditAnywhere)
	bool bEditorDebugHorizontalBlend = false;

	/**
	* Place a dummy Actor here to debug visualize horizonal blending at runtime in the Editor.
	*/
	UPROPERTY(EditAnywhere)
	AActor* TestActor;

#endif

#if WITH_EDITOR
protected:

	virtual void DebugDraw() const override;

#endif
};
