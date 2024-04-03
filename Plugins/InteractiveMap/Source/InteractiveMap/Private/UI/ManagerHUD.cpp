// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ManagerHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/ProvinceEditorWidget.h"
#include "UI/Widgets/EditCountriesContainerWidget.h"
void AManagerHUD::DisplayProvinceEditorWidget(const FProvinceData& provinceData, FName id)
{
	if (!ProvinceEditorWidget)
	{
		if (!ProvincedEditorWidgetClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Province Editor Widget class not set"));
			return;
		}
		ProvinceEditorWidget = CreateWidget<UProvinceEditorWidget>(GetOwningPlayerController(), ProvincedEditorWidgetClass);
		ProvinceEditorWidget->SetInteractiveMapReference(GameMapReference);
		ProvinceEditorWidget->AddToViewport();
	}

	ProvinceEditorWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ProvinceEditorWidget->SetProvinceData(provinceData, id);
}
UE_DISABLE_OPTIMIZATION
void AManagerHUD::DisplayCountryContainerEditorWidget()
{
	if (!CountryContainerEditorWidget)
	{
		if (!CountryContainerEditorWidgetClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Country Editor Widget class not set"));
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
void AManagerHUD::SetInteractiveMapReference(AInteractiveMap* map)
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
