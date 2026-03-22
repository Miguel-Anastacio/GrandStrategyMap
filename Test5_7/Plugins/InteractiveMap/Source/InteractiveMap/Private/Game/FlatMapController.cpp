// Copyright 2024 An@stacioDev All rights reserved.


#include "Game/FlatMapController.h"
#include "Game/EdgeScrollComponent.h"

AFlatMapController::AFlatMapController()
{
	EdgeScrollComponent = CreateDefaultSubobject<UEdgeScrollComponent>(TEXT("EdgeScrollingComponent"));
}
