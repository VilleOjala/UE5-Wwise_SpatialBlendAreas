/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#include "WwiseBlendAreaEvent.h"

void UWwiseBlendAreaEvent::BeginPlay()
{
	Super::BeginPlay();	

	// Set starting weight to zero, in case the default value has not been set to that inside the authoring tool
	SetWeight(0.f);
	SetStopWhenOwnerDestroyed(true);

	if (AkAudioEvent != nullptr && !bStopWhenZeroWeight)
	{
		PostAkEvent(AkAudioEvent);		
	}
}

void UWwiseBlendAreaEvent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWwiseBlendAreaEvent::SetWeight(const float& Weight)
{
	if (IsValid(BlendParameter))
	{
		// Wwise has a convention of using the 0 to 100 range for RTPCs, so let's follow that.
		float Percentage = Weight * 100;
		this->SetRTPCValue(BlendParameter, Percentage, 0, FString());
	}

	if (Weight > 0 && AkAudioEvent != nullptr && !HasActiveEvents())
	{
		PostAkEvent(AkAudioEvent);
	}
	else if (Weight == 0 && bStopWhenZeroWeight && HasActiveEvents())
	{
		Stop();
	}
}

bool UWwiseBlendAreaEvent::GetWeight(float& OutWeight) const
{
	if (IsValid(BlendParameter))
	{
		ERTPCValueType OutValueType;
		this->GetRTPCValue(BlendParameter, ERTPCValueType::GameObject, OutWeight, OutValueType, FString());
		return true;
	}

	return false;
}

const TSet<const ABlendArea*>& UWwiseBlendAreaEvent::GetBlendAreas() const
{
	return BlendAreas;
}
