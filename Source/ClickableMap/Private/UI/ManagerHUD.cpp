// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ManagerHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/ProvinceEditorWidget.h"
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

		ProvinceEditorWidget->AddToViewport();
	}

	ProvinceEditorWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ProvinceEditorWidget->SetProvinceData(provinceData, id);
}
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
