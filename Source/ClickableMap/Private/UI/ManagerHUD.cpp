// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ManagerHUD.h"
#include "Blueprint/UserWidget.h"
void AManagerHUD::BeginPlay()
{
	Super::BeginPlay();
	
	//HudWidget = CreateWidget<UGrandStrategyHUDWidget>(GetOwningPlayerController(), HudWidgetClass);
	//HudWidget->AddToViewport();
}
