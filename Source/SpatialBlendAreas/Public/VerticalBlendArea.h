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
#include "VerticalBlendArea.generated.h"

UCLASS()
class SPATIALBLENDAREAS_API AVerticalBlendArea : public ABlendArea
{
	GENERATED_BODY()
	
public:	

	AVerticalBlendArea();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	/** 
	* Returns a blend weight between 0 and 1 based on the world space height of the input point
	* in relation to the zero point (BlendStartHeight) and the max point (BlendStartHeight + BlendDistance). 
	* Returns 0 if the measurement point is outside the blend area on an XY-plane.
	*/
	virtual float GetBlendWeight(const FVector& Point) const override;

	/** The height in world space below and at which the blend weight is zero. */
	UPROPERTY(EditAnywhere)
	double BlendStartHeight;

#if WITH_EDITORONLY_DATA
protected:

	UPROPERTY(EditAnywhere)
	bool bEditorDebugVerticalBlend = false;

	UPROPERTY(EditAnywhere)
	FColor EditorDebugBlendColor = FColor::Magenta;

#endif

#if WITH_EDITOR
protected:

	virtual void DebugDraw() const override;

#endif
};
