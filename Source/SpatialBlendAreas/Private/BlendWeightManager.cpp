/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#include "BlendWeightManager.h"
#include "Components/SceneComponent.h"
#include "BlendArea.h"
#include "BlendWeightInterface.h"
#include "BlendWeightDistributor.h"
#include "AudioDevice.h"
#include "Kismet/KismetTextLibrary.h"

ABlendWeightManager::ABlendWeightManager()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(RootSceneComponent);
	PrimaryActorTick.bCanEverTick = true;
	SetActorHiddenInGame(true);
	SetCanBeDamaged(false);
}

void ABlendWeightManager::PostInitializeComponents()
{	
	Super::PostInitializeComponents();

	TInlineComponentArray<UActorComponent*> ActorComponents;
	GetComponents(ActorComponents);
	TSet<const ABlendArea*> AllBlendAreas;

	for (const auto& Component : ActorComponents)
	{
		IBlendWeightInterface* BlendWeightInterface = Cast<IBlendWeightInterface>(Component);

		if (BlendWeightInterface == nullptr)
		{
			continue;
		}

		ScriptInterfaces.Add(TScriptInterface<IBlendWeightInterface>(Component));

		for (const auto& BlendArea : BlendWeightInterface->GetBlendAreas())
		{
			if (IsValid(BlendArea) && !AllBlendAreas.Contains(BlendArea))
			{
				AllBlendAreas.Add(BlendArea);
			}
		}
	}

	BlendWeightDistributor = NewObject<UBlendWeightDistributor>();
	BlendWeightDistributor->Initialize(AllBlendAreas);
}

void ABlendWeightManager::BeginPlay()
{
	Super::BeginPlay();
}

void ABlendWeightManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector BlendPosition;
	GetBlendPosition(BlendPosition);
	UpdateWeights(BlendPosition);

#if WITH_EDITOR

	if (bDebugBlendAreaWeights || bDebugInterfaceWeights)
	{
		DebugWeights();
	}

#endif

}

void ABlendWeightManager::GetBlendPosition(FVector& OutPosition) const
{
	OutPosition = FVector::ZeroVector;

	if (FAudioDeviceHandle AudioDevice = this->GetWorld()->GetAudioDevice())
	{
		AudioDevice->GetListenerPosition(0, OutPosition, true);
	}
}

void ABlendWeightManager::UpdateWeights(const FVector& BlendPosition)
{
	if (!IsValid(BlendWeightDistributor))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid BlendWeightDistributor."));
		return;
	}

	// This single result variable potentially gets re-used multiple times below.
	UBlendWeightDistributor::EResult Result;

	Result = BlendWeightDistributor->UpdateWeightData(BlendPosition);

	if (Result != UBlendWeightDistributor::EResult::OK)
	{
		UBlendWeightDistributor::LogResult(Result);
		return;
	}

	for (auto& ScriptInterface : ScriptInterfaces)
	{
		if (ScriptInterface.GetInterface() == nullptr)
		{
			continue;
		}

		TSet<const ABlendArea*> BlendAreas = ScriptInterface->GetBlendAreas();
		float TotalWeight = 0.f;

		for (auto& BlendArea : BlendAreas)
		{
			if (!IsValid(BlendArea))
			{
				continue;
			}

			float Weight = 0.f;
			Result = BlendWeightDistributor->GetWeight(BlendArea, Weight);

			if (Result != UBlendWeightDistributor::EResult::OK)
			{
				UBlendWeightDistributor::LogResult(Result);
			}

			TotalWeight += Weight;
		}

		TotalWeight = FMath::Clamp(TotalWeight, 0, 1);
		ScriptInterface->SetWeight(TotalWeight);
	}
}

#if WITH_EDITOR

void ABlendWeightManager::DebugWeights()
{
	if (GEngine == nullptr)
	{
		return;
	}

	GEngine->ClearOnScreenDebugMessages();

	if (bDebugBlendAreaWeights && BlendWeightDistributor != nullptr)
	{
		TMap<TWeakObjectPtr<const ABlendArea>, float> Weights;
		
		auto Result = BlendWeightDistributor->GetAllWeights(Weights);

		if (Result == UBlendWeightDistributor::EResult::OK)
		{
			for (const auto& Item : Weights)
			{
				if (!Item.Key.IsValid())
				{
					continue;
				}

				FString Name = Item.Key->GetActorLabel();
				FString TruncatedValue = UKismetTextLibrary::Conv_FloatToText(Item.Value, ERoundingMode::HalfToEven, false, true, 1, 324, 0, 2).ToString();
				FString Message = Name + ": " + TruncatedValue;
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Magenta, Message);		
			}
		}
		else
		{
			UBlendWeightDistributor::LogResult(Result);
		}
	}

	if (bDebugInterfaceWeights)
	{
		for (const auto& ScriptInterface : ScriptInterfaces)
		{
			if (ScriptInterface.GetInterface() == nullptr)
			{
				continue;
			}
			
			UObject* Object = ScriptInterface.GetObject();

			if (Object != nullptr)
			{
				FString Name = Object->GetFName().GetPlainNameString();
				float Weight = 0.f;
				ScriptInterface->GetWeight(Weight);
				FString TruncatedValue = UKismetTextLibrary::Conv_FloatToText(Weight, ERoundingMode::HalfToEven, false, true, 1, 324, 0, 2).ToString();
				FString Message = Name + ": " + TruncatedValue;
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, Message);				
			}
		}
	}
}
#endif
