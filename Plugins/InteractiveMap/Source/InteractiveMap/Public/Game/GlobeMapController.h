// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/BirdEyeController.h"
#include "GlobeMapController.generated.h"

/**
 * 
 */
struct FInputActionInstance;
UCLASS()
class INTERACTIVEMAP_API AGlobeMapController : public ABirdEyeController
{
	GENERATED_BODY()


protected:
	// To add mapping context
	void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION()
	void StartRotate();
	UFUNCTION()
	void RotateMap(const FInputActionInstance& instance);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* GlobeMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StartRotateAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateMapAction;

	UPROPERTY(EditAnywhere, Category = "MapRotation")
	float MapRotSpeed = 20.0f;

	//UPROPERTY(Transient)
	//class AInteractiveMap* Map;
};
