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
#include "AkComponent.h"
#include "BlendWeightInterface.h"
#include "BlendArea.h"
#include "WwiseBlendAreaEvent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WWISEINTEGRATION_API UWwiseBlendAreaEvent : public UAkComponent, public IBlendWeightInterface
{
	GENERATED_BODY()

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetWeight(const float& Weight) override;
	virtual bool GetWeight(float& OutWeight) const override;
	const virtual TSet<const ABlendArea*>& GetBlendAreas() const override;

	UPROPERTY(EditAnywhere)
	bool bStopWhenZeroWeight = false;

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UAkRtpc* BlendParameter;

	UPROPERTY(EditAnywhere)
	TSet<const ABlendArea*> BlendAreas;
};
