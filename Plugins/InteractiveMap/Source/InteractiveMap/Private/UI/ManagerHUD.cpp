// Copyright 2024 An@stacioDev All rights reserved.

#include "UI/ManagerHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/ProvinceEditorWidget.h"
#include "UI/Widgets/EditCountriesContainerWidget.h"
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
UE_DISABLE_OPTIMIZATION
void AManagerHUD::DisplayCountryContainerEditorWidget()
{
	if (!CountryContainerEditorWidget)
	{
		if (!CountryContainerEditorWidgetClass)
		{
			UE_LOG(LogInteractiveMap, Error, TEXT("Country Editor Widget class not set"));
			return;
		}

		CountryContainerEditorWidget = CreateWidget<UEditCountriesContainerWidget>(GetOwningPlayerController(), CountryContainerEditorWidgetClass);
		CountryContainerEditorWidget->AddToViewport();

		CountryContainerEditorWidget->SetInteractiveMapReference(GameMapReference);
	}
}
void AManagerHUD::SetProvinceEditorVisibility(ESlateVisibility visibility)
{
	if (ProvinceEditorWidget)
	{
		ProvinceEditorWidget->SetVisibility(visibility);
	}
}
void AManagerHUD::ToggleCountryEditorVisibility()
{
	if (!CountryContainerEditorWidget)
	{
		DisplayCountryContainerEditorWidget();
		return;
	}

	bool test = CountryContainerEditorWidget->IsVisible();

	if (CountryContainerEditorWidget->IsVisible())
	{
		CountryContainerEditorWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		CountryContainerEditorWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

}
UE_ENABLE_OPTIMIZATION
//bool AManagerHUD::IsProvinceDataValidRef() const
//{
//	if (ProvinceDataMapRef)
//		return true;
//	return false;
//}
void AManagerHUD::SetInteractiveMapReference(AClickableMap* map)
{
	GameMapReference = map;
	if (ProvinceEditorWidget)
	{
		ProvinceEditorWidget->SetInteractiveMapReference(map);
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
