#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/ButtonWidget.h"
#include "Components/Button.h"
#include "Map/InteractiveMap.h"
#include "Kismet/GameplayStatics.h"
// Fill out your copyright notice in the Description page of Project Settings.

void UGrandStrategyHUDWidget::SetGameMapReference()
{
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AInteractiveMap::StaticClass());
	if (actor)
	{
		GameMap = Cast<AInteractiveMap>(actor);
	}
}

void UGrandStrategyHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetGameMapReference();
	if (PoliticalMapButton)
	{
		PoliticalMapButton->OnClickedDelegate.AddDynamic(this, &UGrandStrategyHUDWidget::SetMapMode);
	}
	if (ReligiousMapButton)
	{
		ReligiousMapButton->OnClickedDelegate.AddDynamic(this, &UGrandStrategyHUDWidget::SetMapMode);
	}
	if (CultureMapButton)
	{
		CultureMapButton->OnClickedDelegate.AddDynamic(this, &UGrandStrategyHUDWidget::SetMapMode);
	}
	if (TerrainMapButton)
	{
		TerrainMapButton->OnClickedDelegate.AddDynamic(this, &UGrandStrategyHUDWidget::SetMapMode);
	}
}

void UGrandStrategyHUDWidget::SetMapMode(UButtonWidget* button)
{
	if (!GameMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("Map Variable not set in hud widget"));
		return;
	}
	if (button == PoliticalMapButton)
	{
		GameMap->SetMapMode(MapMode::POLITICAL);
		return;
	}
	if (button == ReligiousMapButton)
	{
		GameMap->SetMapMode(MapMode::RELIGIOUS);
		return;
	}
	if (button == CultureMapButton)
	{
		GameMap->SetMapMode(MapMode::CULTURAL);
		return;
	}
	if (button == TerrainMapButton)
	{
		GameMap->SetMapMode(MapMode::TERRAIN);
		return;
	}
}
