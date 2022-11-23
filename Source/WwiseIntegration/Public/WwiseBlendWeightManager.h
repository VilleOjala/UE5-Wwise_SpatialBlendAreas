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
#include "BlendWeightManager.h"
#include "WwiseBlendWeightManager.generated.h"

UCLASS()
class WWISEINTEGRATION_API AWwiseBlendWeightManager : public ABlendWeightManager
{
	GENERATED_BODY()
	
public:	

	AWwiseBlendWeightManager();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetBlendPosition(FVector& OutPosition) const override;

public:	

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	bool bUseSpatialAudioListener;

private:

	UPROPERTY(EditAnywhere)
	UAkStateValue* InsideRoomState;

	UPROPERTY(EditAnywhere)
	UAkStateValue* NoneState;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float RoomContainmentCheckInterval = 0.f;

	float Timer = 0.f;
	bool bInsideRoom = true;
};
