// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/CustomButtonWidget.h"
#include "Map/ClickableMap.h"
#include "Kismet/GameplayStatics.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "InteractiveMap.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"
#include "UI/Widgets/MapModeSelectorWidget.h"

void UGrandStrategyHUDWidget::SetGameMapReference()
{
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AClickableMap::StaticClass());
	if (actor)
	{
		GameMap = Cast<AClickableMap>(actor);
		if(UMapModeSelectorWidget && GameMap)
		{
			UMapModeSelectorWidget->SetInteractiveMapReference(GameMap);
		}
	}
}

void UGrandStrategyHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetGameMapReference();

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
}

void UGrandStrategyHUDWidget::SaveDataToJson(UCustomButtonWidget* button)
{
	if (button == SaveDataButton)
	{
		// TMap<int, FInstancedStruct>* provinceMapData = GameMap->GetProvinceDataMap();
		// if (!provinceMapData)
		// 	return;
		//
		// bool result = false;
		// FString outMessageInfo;
		// FileName = FileName.EndsWith(FString(".json")) ? FileName : FileName.Append(TEXT(".json"));
		//
		// const FString dirPath = FPaths::ProjectDir() + DirectoryPath;
		//
		// // UDataManagerFunctionLibrary::WriteMapToJsonFile(dirPath + FileName, *provinceMapData, result, outMessageInfo);
		// if (!result)
		// {
		// 	GEngine->AddOnScreenDebugMessage(4, 1.0f, FColor::Magenta, outMessageInfo);
		// }
		//
		// return;
	}

	if (button == SaveCountryButton)
	{
		// bool result;
		// FString outMessageInfo;
		// FileName = FileName.EndsWith(FString(".json")) ? FileName : FileName.Append(TEXT(".json"));
		//
		// const FString dirPath = FPaths::ProjectDir() + DirectoryPath;
		//
		// if (!result)
		// {
		// 	GEngine->AddOnScreenDebugMessage(4, 1.0f, FColor::Magenta, outMessageInfo);
		// }
		//
		// return;
	}


}
