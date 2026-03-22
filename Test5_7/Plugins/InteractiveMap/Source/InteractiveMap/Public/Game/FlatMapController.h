// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BirdEyeController.h"
#include "FlatMapController.generated.h"

/**
 * Map controller for flat maps, a base controller + a component to handle movements when mouse is on viewport edges
 */
UCLASS()
class INTERACTIVEMAP_API AFlatMapController : public ABirdEyeController
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    AFlatMapController();
protected:
    
    UPROPERTY(EditAnywhere, Category="Component")
    TObjectPtr<class UEdgeScrollComponent> EdgeScrollComponent;

};
