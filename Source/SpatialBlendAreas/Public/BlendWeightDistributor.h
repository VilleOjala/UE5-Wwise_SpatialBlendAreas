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
#include "UObject/NoExportTypes.h"
#include "BlendArea.h"
#include "BlendWeightDistributor.generated.h"

UCLASS()
class SPATIALBLENDAREAS_API UBlendWeightDistributor : public UObject
{
	GENERATED_BODY()

public:

	UBlendWeightDistributor();

private:

	UPROPERTY()
	TMap<TWeakObjectPtr<const ABlendArea>, float> AreaWeights;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<const ABlendArea>> RelevantAreas;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<const ABlendArea>> SharedPriorityAreas;
	
	bool bIsInitialized = false;

public:

	enum class EResult
	{
		OK,
		ERR_UNINITIALIZED,
		ERR_ALREADY_INITIALIZED,
		ERR_INVALID_AREA,
		ERR_UNREGISTERED_AREA
	};

	static void LogResult(UBlendWeightDistributor::EResult Result);
		
	/** Register blend areas before starting to update or retrieve weight data.*/
	EResult Initialize(const TSet<const ABlendArea*>& AreasToRegister);

	/** Call this method before trying to retrieve weight data for some particular area.*/
	EResult UpdateWeightData(const FVector& Position);

	/** Returns weight data for a registered blend area calcuted on the latest update call.*/
	EResult GetWeight(const ABlendArea*& BlendArea, float& OutWeight);

	/** Returns weight data for all registered blend areas calcuted on the latest update call.*/
	EResult GetAllWeights(TMap<TWeakObjectPtr<const ABlendArea>, float>& OutWeights);
};
