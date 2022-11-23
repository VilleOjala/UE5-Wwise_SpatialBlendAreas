/*****************************************************************************************************
Spatial Blend Areas
Copyright 2022 Ville Ojala
Apache License, Version 2.0

The plugin contains dependencies to Unreal Engine by Epic Games Inc. and AUDIOKINETIC Wwise Technology
by Audiokinetic Inc. (the latter applies only to the 'WwiseIntegration' -module), the use of which is
subjected to the respective product licensing terms.
******************************************************************************************************/

#include "WwiseBlendWeightManager.h"
#include "AkAudioDevice.h"
#include "AkComponent.h"

AWwiseBlendWeightManager::AWwiseBlendWeightManager()
	: bUseSpatialAudioListener(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWwiseBlendWeightManager::BeginPlay()
{
	Super::BeginPlay();
}

void AWwiseBlendWeightManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (NoneState != nullptr)
	{
		FAkAudioDevice* AudioDevice = FAkAudioDevice::Get();

		if (AudioDevice != nullptr)
		{
			AudioDevice->SetState(NoneState);
		}
	}
}

void AWwiseBlendWeightManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!(Timer >= RoomContainmentCheckInterval))
	{
		Timer += DeltaTime;
		return;
	}

	Timer = 0.f;
	FAkAudioDevice* AudioDevice = FAkAudioDevice::Get();
	
	if (AudioDevice != nullptr && InsideRoomState != nullptr && NoneState != nullptr)
	{
		if (!AudioDevice->UsingSpatialAudioRooms(this->GetWorld()) && bInsideRoom)
		{
			AudioDevice->SetState(NoneState);
			bInsideRoom = false;
		}
		else
		{
			FVector Position;
			GetBlendPosition(Position);
			TArray<UAkRoomComponent*> Rooms = AudioDevice->FindRoomComponentsAtLocation(Position, this->GetWorld());

			if (Rooms.Num() == 0 && bInsideRoom)
			{
				AudioDevice->SetState(NoneState);
				bInsideRoom = false;
			}
			else if (Rooms.Num() > 0 && !bInsideRoom)
			{
				AudioDevice->SetState(InsideRoomState);
				bInsideRoom = true;
			}
		}
	}
}

void AWwiseBlendWeightManager::GetBlendPosition(FVector& OutPosition) const
{
	OutPosition = FVector::ZeroVector;
	FAkAudioDevice* AudioDevice = FAkAudioDevice::Get();

	if (AudioDevice != nullptr)
	{
		if (bUseSpatialAudioListener)
		{
			UAkComponent* SpatialAudioListener = AudioDevice->GetSpatialAudioListener();

			if (SpatialAudioListener != nullptr)
			{
				OutPosition = SpatialAudioListener->GetPosition();
			}
		}
		else
		{
			UAkComponentSet Listeners = AudioDevice->GetDefaultListeners();

			for (const auto& Listener : Listeners)
			{
				OutPosition = Listener->GetPosition();

				// As a default implementation, assume that only one listener is being used and early out.
				return;
			}
		}
	}
}
