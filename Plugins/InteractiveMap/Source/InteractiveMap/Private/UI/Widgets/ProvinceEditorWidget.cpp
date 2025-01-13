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
	//
	// if (editedText == OwnerCustomInput)
	// {
	// 	if (CommitMethod == ETextCommit::OnEnter)
	// 	{
	// 		if (GameMapReference)
	// 		{
	// 			FString dummy = Text.ToString();
	// 			// ProvinceSelectedData.Owner = FName(*dummy);
	// 			// if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
	// 			// {
	// 			// 	const FCountryData* owner = GameMapReference->GetDataFromID(ProvinceSelectedData.Owner, *GameMapReference->GetCountryDataMap());
	// 			// 	editedText->SetValues(FText::FromString(owner->CountryName), FText::FromString(FString("______")));
	// 			// }
	//
	// 		}
	// 	}
	// 	return;
	// }
	//
	// if (editedText == NameCustomInput)
	// {
	// 	if (CommitMethod == ETextCommit::OnEnter)
	// 	{
	// 		if (GameMapReference)
	// 		{
	// 			FString value = Text.ToString();
	// 			ProvinceSelectedData.ProvinceName = value;
	// 			// if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
	// 			// {
	// 			// 	editedText->SetValues(Text, FText::FromString(FString("______")));
	// 			// }
	//
	// 		}
	// 	}
	// 	return;
	// }
	//
	// if (editedText == ReligionCustomInput)
	// {
	// 	if (CommitMethod == ETextCommit::OnEnter)
	// 	{
	// 		if (GameMapReference)
	// 		{
	// 			FString value = Text.ToString();
	// 			ProvinceSelectedData.Religion = FName(*value);
	// 			// if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
	// 			// {
	// 			// 	const FColoredData* religion = GameMapReference->GetDataFromID(ProvinceSelectedData.Religion, *GameMapReference->GetVisualPropertiesDataMap());
	// 			// 	editedText->SetValues(FText::FromString(religion->DataName), FText::FromString(FString("______")));
	// 			// }
	// 		
	//
	// 		}
	// 	}
	// 	return;
	// }
	//
	// if (editedText == CultureCustomInput)
	// {
	// 	if (CommitMethod == ETextCommit::OnEnter)
	// 	{
	// 		if (GameMapReference)
	// 		{
	// 			FString value = Text.ToString();
	// 			ProvinceSelectedData.Culture = FName(*value);
	// 			// if(GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
	// 			// {
	// 			//
	// 			// 	const FColoredData* culture = GameMapReference->GetDataFromID(ProvinceSelectedData.Culture, *GameMapReference->GetVisualPropertiesDataMap());
	// 			// 	editedText->SetValues(FText::FromString(culture->DataName), FText::FromString(FString("______")));
	// 			// }
	// 		}
	// 	}
	// 	return;
	// }
	//
	// if (editedText == PopulationCustomInput)
	// {
	// 	if (CommitMethod == ETextCommit::OnEnter)
	// 	{
	// 		if (!Text.IsNumeric())
	// 		{
	// 			UE_LOG(LogInteractiveMap, Warning, TEXT("Invalid value in population, please insert a number"));
	// 			editedText->SetValues(Text, FText::FromString(FString("______")));
	// 		}
	// 		if (GameMapReference)
	// 		{
	// 			FString value = Text.ToString();
	// 			ProvinceSelectedData.Population = FCString::Atoi(*value);
	// 			// if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
	// 			// {
	// 			// 	editedText->SetValues(Text, FText::FromString(FString("______")));
	// 			// }
	// 		}
	// 	}
	// 	return;
	// }

}


