// Copyright 2024 An@stacioDev All rights reserved.

#include "UI/ManagerHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/ProvinceEditorWidget.h"
#include "InteractiveMap.h"
void AManagerHUD::DisplayProvinceEditorWidget(const FInstancedStruct& ProvinceData, int Id)
{
	if (!ProvinceEditorWidget)
	{
		if (!ProvincedEditorWidgetClass)
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Province Editor Widget class not set"));
			return;
		}
		ProvinceEditorWidget = CreateWidget<UProvinceEditorWidget>(GetOwningPlayerController(), ProvincedEditorWidgetClass);
		ProvinceEditorWidget->SetInteractiveMapReference(GameMapReference);
		ProvinceEditorWidget->AddToViewport();
	}

	ProvinceEditorWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ProvinceEditorWidget->SetProvinceData(ProvinceData, Id);
}

void AManagerHUD::SetProvinceEditorVisibility(ESlateVisibility Visibility)
{
	if (ProvinceEditorWidget)
	{
		ProvinceEditorWidget->SetVisibility(Visibility);
	}
}

void AManagerHUD::SetInteractiveMapReference(AClickableMap* Map)
{
	GameMapReference = Map;
	if (ProvinceEditorWidget)
	{
		ProvinceEditorWidget->SetInteractiveMapReference(Map);
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
