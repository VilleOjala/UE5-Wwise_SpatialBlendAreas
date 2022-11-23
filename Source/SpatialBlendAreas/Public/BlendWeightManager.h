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
#include "BlendWeightManager.generated.h"

UCLASS()
class SPATIALBLENDAREAS_API ABlendWeightManager : public AActor
{
	GENERATED_BODY()
	
public:	

	ABlendWeightManager();

protected:

	virtual void BeginPlay() override;

	/**
	* Retrieves the first listener from Unreal Engine's FAudioDevice as the default implementation.
	* Override this method when using an audio middleware, such as Wwise or FMOD,
	* or if some other custom blend position is needed.
	*/
	virtual void GetBlendPosition(FVector& OutPosition) const;
	void UpdateWeights(const FVector& BlendPosition);

public:	

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

private:

	UPROPERTY()
	class UBlendWeightDistributor* BlendWeightDistributor;

	UPROPERTY()
	TArray<TScriptInterface<class IBlendWeightInterface>> ScriptInterfaces;

	UPROPERTY()
	class USceneComponent* RootSceneComponent;

#if WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere)
	bool bDebugInterfaceWeights = false;

	UPROPERTY(EditAnywhere)
	bool bDebugBlendAreaWeights = false;
	
#endif

#if WITH_EDITOR

	void DebugWeights();

#endif
};
