// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/ProvinceEditorWidget.h"
#include "Map/ClickableMap.h"
#include "InteractiveMap.h"
#include "GenericWidget/GenericStructWidget.h"

void UProvinceEditorWidget::SetProvinceData(const FInstancedStruct& Data, int ProvinceID)
{
	ProvinceSelectedData = Data;
	ProvinceSelectedID = ProvinceID;
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

void UProvinceEditorWidget::SetInteractiveMapReference(AClickableMap* map)
{
	GameMapReference = map;
}

void UProvinceEditorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}




