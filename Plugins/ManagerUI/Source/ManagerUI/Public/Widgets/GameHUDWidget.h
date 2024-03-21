// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUDWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class MANAGERUI_API UGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	void NativeOnInitialized() override;
	// Here you would add widgets (text, images or other custom widgets) that you would 
	// want to change through code 
	// this is just an example
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock>  PlayerName;
};
