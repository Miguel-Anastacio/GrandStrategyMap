// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ManagerHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/GrandStrategyHUDWidget.h"
void AManagerHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (HudWidgetClass)
	{
		HudWidget = CreateWidget<UGrandStrategyHUDWidget>(GetOwningPlayerController(), HudWidgetClass);
		HudWidget->AddToViewport();

	}
}
