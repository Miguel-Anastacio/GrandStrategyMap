// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/ProvinceEditorWidget.h"
#include "UserWidgets/CustomEditableText.h"
#include "Map/ClickableMap.h"
#include "InteractiveMap.h"
#include "UserWidgets/GenericStructWidget.h"

void UProvinceEditorWidget::SetProvinceData(const FInstancedStruct& Data, int ProvinceID)
{
	ProvinceSelectedData = Data;
	ProvinceSelectedID = ProvinceID;
	if(!ProvinceDataWidget)
	{
		return;
	}
	if(ProvinceDataWidget->StructType != Data.GetScriptStruct())
	{
		UE_LOG(LogInteractiveMap, Error, TEXT("Province Data struct of Map does not match Struct Specified in Widget %s"), *ProvinceDataWidget->GetName())
		return;
	}
	ProvinceDataWidget->InitFromStruct(Data);
}

void UProvinceEditorWidget::SetInteractiveMapReference(AClickableMap* map)
{
	GameMapReference = map;
}

void UProvinceEditorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UProvinceEditorWidget::UpdateProvinceData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod)
{
	
}


