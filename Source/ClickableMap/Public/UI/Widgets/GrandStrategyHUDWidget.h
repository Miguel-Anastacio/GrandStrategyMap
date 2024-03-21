// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ManagerUI/WidgetReactionInterface.h"
#include "GrandStrategyHUDWidget.generated.h"

UCLASS(Abstract, BlueprintType)
class CLICKABLEMAP_API UGrandStrategyHUDWidget : public UUserWidget, public IWidgetReactionInterface
{
	GENERATED_BODY()
public:
	

	
};
