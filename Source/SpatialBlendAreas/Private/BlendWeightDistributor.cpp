/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#include "BlendWeightDistributor.h"
#include "BlendArea.h"

UBlendWeightDistributor::UBlendWeightDistributor()
{
}

UBlendWeightDistributor::EResult UBlendWeightDistributor::Initialize(const TSet<const ABlendArea*>& Registrees) 
{
	if (bIsInitialized)
	{
		return EResult::ERR_ALREADY_INITIALIZED;
	}

	const uint32 AreaCount = Registrees.Num();
	AreaWeights.Reserve(AreaCount);
	RelevantAreas.Reserve(AreaCount);
	SharedPriorityAreas.Reserve(AreaCount);

	for (const auto& Area : Registrees)
	{
		if (!AreaWeights.Contains(Area))
		{
			AreaWeights.Add(Area, 0.f);
		}
	}

	bIsInitialized = true;
	return EResult::OK;
}

UBlendWeightDistributor::EResult UBlendWeightDistributor::GetWeight(const ABlendArea*& BlendArea, float& OutWeight)
{
	if (!bIsInitialized)
	{
		return EResult::ERR_UNINITIALIZED;
	}

	if (!IsValid(BlendArea))
	{
		return EResult::ERR_INVALID_AREA;
	}

	float* Weight = AreaWeights.Find(BlendArea);

	if (Weight == nullptr)
	{
		return EResult::ERR_UNREGISTERED_AREA;
	}

	OutWeight = *Weight;
	return EResult::OK;
}

UBlendWeightDistributor::EResult UBlendWeightDistributor::UpdateWeightData(const FVector& Position)
{
	if (!bIsInitialized)
	{
		return EResult::ERR_UNINITIALIZED;
	}

	RelevantAreas.Reset();

	for (auto& Item : AreaWeights)
	{
		if (!Item.Key.IsValid())
		{
			Item.Value = 0.f;
			continue;
		}

		// Get the blend weight for each area as an isolated case.
		const float BlendWeight = Item.Key->GetBlendWeight(Position);
		Item.Value = BlendWeight;

		// Ignore areas with zero blend weight.
		if (BlendWeight > 0)
		{
			RelevantAreas.Add(Item.Key);
		}
	}

	if (RelevantAreas.Num() <= 1)
	{
		return EResult::OK;
	}

	// Sort by priority, so that the higher priority blend areas consume the weight budget first.
	Algo::SortBy(RelevantAreas, [](const TWeakObjectPtr<const ABlendArea> Area) 
								{ return Area.IsValid() ? Area->Priority : 0; }, TGreater<uint32>());

	SharedPriorityAreas.Reset();
	float RemainingWeightBudget = 1.f;

	auto DistributeWeights = [&]()
	{
		float WeightsSum = 0.f;

		for (const auto& Area : SharedPriorityAreas)
		{
			WeightsSum += AreaWeights[Area];
		}

		// If the remaining weight budget does not cover the sum of weights in this priority group,
		// distribute the rest of the budget based on the relative importance of each area.
		if (WeightsSum > RemainingWeightBudget)
		{
			for (const auto& Area : SharedPriorityAreas)
			{
				float OriginalWeight = AreaWeights[Area];
				float AdjustedWeight = RemainingWeightBudget * OriginalWeight / WeightsSum;
				AreaWeights[Area] = AdjustedWeight;
			}
		}

		RemainingWeightBudget = FMath::Clamp((RemainingWeightBudget - WeightsSum), 0, 1);
	};
	
	// Distribute the overall weight budget (i.e. 1) by going through one priority group at a time.
	for (int32 Index = 0; Index < RelevantAreas.Num() - 1; Index++)
	{
		const ABlendArea* const& AreaA = RelevantAreas[Index].Get();
		const ABlendArea* const& AreaB = RelevantAreas[Index + 1].Get();

		if (AreaA->Priority == AreaB->Priority)
		{
			if (!SharedPriorityAreas.Contains(AreaA))
			{
				SharedPriorityAreas.Add(AreaA);
			}

			SharedPriorityAreas.Add(AreaB);

			if (Index == RelevantAreas.Num() - 2)
			{
				DistributeWeights();
			}
		}
		else
		{
			if (!SharedPriorityAreas.Contains(AreaA))
			{
				SharedPriorityAreas.Add(AreaA);
			}

			DistributeWeights();
			SharedPriorityAreas.Reset();

			if (Index == RelevantAreas.Num() - 2)
			{
				SharedPriorityAreas.Add(AreaB);
				DistributeWeights();
			}
		}
	}
	
	return EResult::OK;
}

 UBlendWeightDistributor::EResult UBlendWeightDistributor::GetAllWeights(TMap<TWeakObjectPtr<const ABlendArea>, float>& OutWeights)
{
	 if (!bIsInitialized)
	 {
		 return EResult::ERR_UNINITIALIZED;
	 }

	 OutWeights = AreaWeights;
	 return EResult::OK;
}

void UBlendWeightDistributor::LogResult(UBlendWeightDistributor::EResult Result)
{
	switch (Result)
	{
	case UBlendWeightDistributor::EResult::OK:
		UE_LOG(LogTemp, Log, TEXT("UBlendWeightDistributor::EResult: OK"))
		break;
	case UBlendWeightDistributor::EResult::ERR_UNINITIALIZED:
		UE_LOG(LogTemp, Error, TEXT("UBlendWeightDistributor::EResult: ERR_UNINITIALIZED"))
		break;
	case UBlendWeightDistributor::EResult::ERR_ALREADY_INITIALIZED:
		UE_LOG(LogTemp, Warning, TEXT("UBlendWeightDistributor::EResult: ERR_ALREADY_INITIALIZED"))
		break;
	case UBlendWeightDistributor::EResult::ERR_INVALID_AREA:
		UE_LOG(LogTemp, Error, TEXT("UBlendWeightDistributor::EResult: ERR_INVALID_AREA"))
		break;
	case UBlendWeightDistributor::EResult::ERR_UNREGISTERED_AREA:
		UE_LOG(LogTemp, Error, TEXT("UBlendWeightDistributor::EResult: ERR_INVALID_AREA"))
		break;
	default:
		break;
	}
}
