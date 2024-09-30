// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/CustomButtonWidget.h"
#include "Components/Button.h"
#include "Map/ClickableMap.h"
#include "Kismet/GameplayStatics.h"
#include "DataManager/DataManagerFunctionLibrary.h"
#include "InteractiveMap.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"

void UGrandStrategyHUDWidget::SetGameMapReference()
{
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AClickableMap::StaticClass());
	if (actor)
	{
		GameMap = Cast<AClickableMap>(actor);
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

	if (SaveDataButton)
	{
		SaveDataButton->OnClickedDelegate.AddDynamic(this, &UGrandStrategyHUDWidget::SaveDataToJson);
	}
	if (SaveCountryButton)
	{
		SaveCountryButton->OnClickedDelegate.AddDynamic(this, &UGrandStrategyHUDWidget::SaveDataToJson);
	}
}

void UGrandStrategyHUDWidget::SetMapMode(UCustomButtonWidget* button)
{
	if (!GameMap)
	{
		UE_LOG(LogInteractiveMap, Warning, TEXT("Map Variable not set in hud widget"));
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

void UGrandStrategyHUDWidget::SaveDataToJson(UCustomButtonWidget* button)
{
	if (button == SaveDataButton)
	{
		TMap<FName, FProvinceData>* provinceMapData = GameMap->GetProvinceDataMap();
		if (!provinceMapData)
			return;

		bool result;
		FString outMessageInfo;
		FileName = FileName.EndsWith(FString(".json")) ? FileName : FileName.Append(TEXT(".json"));

		const FString dirPath = FPaths::ProjectDir() + DirectoryPath;

		UDataManagerFunctioLibrary::WriteMapToJsonFile(dirPath + FileName, *provinceMapData, result, outMessageInfo);
		if (!result)
		{
			GEngine->AddOnScreenDebugMessage(4, 1.0f, FColor::Magenta, outMessageInfo);
		}

		return;
	}

	if (button == SaveCountryButton)
	{
		TMap<FName, FCountryData>* countryDataMap = GameMap->GetCountryDataMap();
		if (!countryDataMap)
			return;

		bool result;
		FString outMessageInfo;
		FileName = FileName.EndsWith(FString(".json")) ? FileName : FileName.Append(TEXT(".json"));

		const FString dirPath = FPaths::ProjectDir() + DirectoryPath;

		UDataManagerFunctioLibrary::WriteMapToJsonFile(dirPath + FileNameCountry, *countryDataMap, result, outMessageInfo);
		if (!result)
		{
			GEngine->AddOnScreenDebugMessage(4, 1.0f, FColor::Magenta, outMessageInfo);
		}

		return;
	}


}
