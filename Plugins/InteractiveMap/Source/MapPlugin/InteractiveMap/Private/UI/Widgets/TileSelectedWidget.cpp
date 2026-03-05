// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/TileSelectedWidget.h"
#include "InteractiveMap.h"
#include "GenericWidget/GenericStructWidget.h"

void UTileSelectedWidget::SetData_Implementation(const FInstancedStruct& Data)
{
	if(Data.GetScriptStruct() == ProvinceDataWidget->GetDataClass())
	{
		OceanTileDataWidget->SetVisibility(ESlateVisibility::Collapsed);
		ProvinceDataWidget->InitFromStruct(Data);
		ProvinceDataWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else if(Data.GetScriptStruct() == OceanTileDataWidget->GetDataClass())
	{
		ProvinceDataWidget->SetVisibility(ESlateVisibility::Collapsed);
		OceanTileDataWidget->InitFromStruct(Data);
		OceanTileDataWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Province Data struct of Map does not match any Struct Specified in Widgets %s"), *ProvinceDataWidget->GetName())
	}
}


