// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetReactionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class MANAGERUI_API UWidgetReactionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface adds events to define behaviour when widget is pushed/popped to/from a layer
 * This events are supposed to be called by creating custom LayersUI either in C++ or blueprints
 */
class MANAGERUI_API IWidgetReactionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// the reaction to the widget on being pushed, for example play animation or play a sound
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Widget Reaction")
	void OnWidgetPushed();
	virtual void OnWidgetPushed_Implementation() {};
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Widget Reaction")
	void OnWidgetPopped();
	virtual void OnWidgetPopped_Implementation() {};
};
