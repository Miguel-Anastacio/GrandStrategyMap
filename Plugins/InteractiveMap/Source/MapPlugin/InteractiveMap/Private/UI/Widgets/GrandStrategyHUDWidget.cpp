// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/CustomButtonWidget.h"
#include "Map/ClickableMap.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"
#include "UI/Widgets/MapModeSelectorWidget.h"

void UGrandStrategyHUDWidget::SetGameMapReference()
{
	if (AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AClickableMap::StaticClass()))
	{
		GameMap = Cast<AClickableMap>(Actor);
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
}
