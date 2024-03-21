// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SlateWrapperTypes.h"

#include "LayerUI.generated.h"

// How many widgets can be visible at the same time
// SINGLE - one
// MULTIPLE - several
UENUM(BlueprintType)
enum class LayerType : uint8
{
	SINGLE UMETA(DisplayName = "Single"),
	MULTIPLE   UMETA(DisplayName = "Multiple"),
};

class UUserWidget;

/**
 * UObject that keeps a stack of UserWidgets
 * Create layers that inherit from this to override the reactions when a widget is added or removed to a layer
 * It is also possible to define the impact that pushing/popping to a layer has on the other layers 
 *
 */
UCLASS(Blueprintable, BlueprintType)
class MANAGERUI_API ULayerUI : public UObject
{
	GENERATED_BODY()

public:
	//ULayerUI();
	// You should not call these functions outside of a layer object or the layer manager
	void PushToStack(UUserWidget* widget);
	UUserWidget* PopFromStack();

	UUserWidget* PeakStack() const; 
	bool IsLayerEmpty() const;
	//////////////////////////////////////


	UFUNCTION(BlueprintCallable, Category = Layer)
	void SetVisibilityOfLayer(ESlateVisibility visibility);

	UFUNCTION(BlueprintCallable, Category = Layer)
	ESlateVisibility GetVisibilityOfLayer() const;

	UFUNCTION(BlueprintCallable, Category = Layer)
	void ClearStack();

	// this defines the impact that pushing to this layer has on the other layers
	UFUNCTION(BlueprintNativeEvent, Category = "Layer Reaction")
	void OnWidgetPushedOthers(const TArray<ULayerUI*>& otherLayers);
	virtual void OnWidgetPushedOthers_Implementation(const TArray<ULayerUI*>& otherLayers);

	// this defines the impact that pushing to this layer has on the other layers
	UFUNCTION(BlueprintNativeEvent, Category = Layer)
	void OnWidgetPoppedOthers(const TArray<ULayerUI*>& otherLayers);
	virtual void OnWidgetPoppedOthers_Implementation(const TArray<ULayerUI*>& otherLayers);

	// this defines the impact that clearing this layer has on its widgets 
	UFUNCTION(BlueprintNativeEvent, Category = Layer)
	void OnLayerCleared();
	// by default it just changes the widgets visibility to hidden state
	virtual void OnLayerCleared_Implementation();


public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Layer);
	LayerType Type = LayerType::SINGLE;

	// The visibility to set the widget when it is pushed to the layer
	UPROPERTY(BlueprintReadWrite, EditAnywhere,  meta = (ExposeOnSpawn = true), Category = Layer);
	ESlateVisibility VisibleState = ESlateVisibility::SelfHitTestInvisible;

	// The visibility to set the widget when it is popped from the layer
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Layer);
	ESlateVisibility HiddenState = ESlateVisibility::Collapsed;

protected:
	UPROPERTY(VisibleAnywhere, Category = Layer)
	TArray<class UUserWidget*> WidgetStack;

	// called when pushing a widget by default uses the layerType to hide/not hide the other widgets
	UFUNCTION(BlueprintNativeEvent, Category = Layer)
	void OnWidgetPushed(class UUserWidget* widget);
	virtual void OnWidgetPushed_Implementation(class UUserWidget* widget);

	// called when popping a widget by default - nothing
	UFUNCTION(BlueprintNativeEvent, Category = Layer)
	void OnWidgetPopped(class UUserWidget* widget);
	virtual void OnWidgetPopped_Implementation(class UUserWidget* widget);

};
