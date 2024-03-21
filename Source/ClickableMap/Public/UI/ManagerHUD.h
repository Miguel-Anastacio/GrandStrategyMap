// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ManagerHUD.generated.h"

/**
 * 
 */
class UGrandStrategyHUDWidget;
UCLASS()
class CLICKABLEMAP_API AManagerHUD : public AHUD
{
	GENERATED_BODY()
	
public:


protected:
	void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UGrandStrategyHUDWidget> HudWidgetClass;

	UPROPERTY()
	TObjectPtr<UGrandStrategyHUDWidget> HudWidget;
	
};
