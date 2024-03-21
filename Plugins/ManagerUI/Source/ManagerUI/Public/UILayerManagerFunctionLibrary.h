// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UILayerManagerFunctionLibrary.generated.h"

/**
 *  Utility fucntion library to simplify interacting with the LayerManagerHUD from outside the HUD class
 */
UCLASS()
class MANAGERUI_API UUILayerManagerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Usually used when adding widgets to layer from outside of the HUD class
	UFUNCTION(BlueprintCallable, Category = "UI Utils")
	static UUserWidget* AddWidgetToLayer(TSubclassOf<class UUserWidget> widgetClass, const FString& layerName, APlayerController* controller);

	UFUNCTION(BlueprintCallable, Category = "UI Utils")
	static void ToggleWidgetVisibility(class UUserWidget* widget, const FString& layerName, APlayerController* controller);
	//static UUserWidget* ToggleWi(TSubclassOf<class UUserWidget> widgetClass, const FString& layerName, APlayerController* controller);
};
