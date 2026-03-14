// Copyright 2024 An@stacioDev All rights reserved.

#include "UI/ManagerHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/GrandStrategyHUDWidget.h"
void AManagerHUD::DisplayTileSelectedWidget(const FInstancedStruct& Data)
{
	if (HudWidget)
	{
		HudWidget->SetTileSelectedWidgetData(Data);
		HudWidget->SetTileSelectedVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void AManagerHUD::SetTileSelectedWidgetVisibility(ESlateVisibility Visibility)
{
	if (HudWidget)
	{
		HudWidget->SetTileSelectedVisibility(Visibility);
	}
}

void AManagerHUD::BeginPlay()
{
	Super::BeginPlay();
	if (HudWidgetClass)
	{
		HudWidget = CreateWidget<UGrandStrategyHUDWidget>(GetOwningPlayerController(), HudWidgetClass);
		HudWidget->AddToViewport();
	}
}
