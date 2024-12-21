// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/ProvinceEditorWidget.h"
#include "UserWidgets/CustomEditableText.h"
#include "Map/ClickableMap.h"
#include "InteractiveMap.h"

void UProvinceEditorWidget::SetProvinceData(const FProvinceData& data, FName provinceID)
{
	NameCustomInput->SetValues(FText::FromString(data.ProvinceName), FText::FromString(FString("______")));
	PopulationCustomInput->SetValues(FText::AsNumber(data.Population), FText::FromString(FString("______")));

	const FCountryData* owner = GameMapReference->GetDataFromID(data.Owner, *GameMapReference->GetCountryDataMap());
	const FColoredData* culture = GameMapReference->GetDataFromID(data.Culture, *GameMapReference->GetVisualPropertiesDataMap());
	const FColoredData* religion = GameMapReference->GetDataFromID(data.Religion, *GameMapReference->GetVisualPropertiesDataMap());



	if (!owner)
	{
		OwnerCustomInput->SetValues(FText::FromString("Invalid"), FText::FromString(FString("______")));

		UE_LOG(LogInteractiveMap, Error, TEXT("In Province Editor Widget"));
		UE_LOG(LogInteractiveMap, Error, TEXT("Province Owner is not valid, either the CountryData table is not set or the country tag is not present in the data set"));
	}
	else
	{
		FString ownerName = owner->CountryName;
		OwnerCustomInput->SetValues(FText::FromString(ownerName), FText::FromString(FString("______")));
	}

	if (!culture)
	{
		CultureCustomInput->SetValues(FText::FromString("Invalid"), FText::FromString(FString("______")));
		UE_LOG(LogInteractiveMap, Error, TEXT("In Province Editor Widget"));
		UE_LOG(LogInteractiveMap, Error, TEXT("Province Culture is not valid, either the VisualProperties data table is not set or the religion tag is not present in the data set"));
	}
	else
	{
		FString cultureName = culture->DataName;
		CultureCustomInput->SetValues(FText::FromString(cultureName), FText::FromString(FString("______")));
	}
	if (!religion)
	{
		ReligionCustomInput->SetValues(FText::FromString("Invalid"), FText::FromString(FString("______")));
		UE_LOG(LogInteractiveMap, Error, TEXT("In Province Editor Widget"));
		UE_LOG(LogInteractiveMap, Error, TEXT("Province Religion is not valid, either the VisualProperties table is not set or the culture tag is not present in the data set"));
	}
	else
	{
		FString religionName = religion->DataName;
		ReligionCustomInput->SetValues(FText::FromString(religionName), FText::FromString(FString("______")));
	}


	ProvinceSelectedData = data;
	ProvinceSelectedID = provinceID;
}

void UProvinceEditorWidget::SetInteractiveMapReference(AClickableMap* map)
{
	GameMapReference = map;
}

void UProvinceEditorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
		
	OwnerCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);
	NameCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);
	ReligionCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);
	CultureCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);
	PopulationCustomInput->TextCommitDelegate.AddDynamic(this, &UProvinceEditorWidget::UpdateProvinceData);

}

void UProvinceEditorWidget::UpdateProvinceData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod)
{

	if (editedText == OwnerCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString dummy = Text.ToString();
				ProvinceSelectedData.Owner = FName(*dummy);
				if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{
					const FCountryData* owner = GameMapReference->GetDataFromID(ProvinceSelectedData.Owner, *GameMapReference->GetCountryDataMap());
					editedText->SetValues(FText::FromString(owner->CountryName), FText::FromString(FString("______")));
				}

			}
		}
		return;
	}

	if (editedText == NameCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString value = Text.ToString();
				ProvinceSelectedData.ProvinceName = value;
				if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{
					editedText->SetValues(Text, FText::FromString(FString("______")));
				}

			}
		}
		return;
	}

	if (editedText == ReligionCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString value = Text.ToString();
				ProvinceSelectedData.Religion = FName(*value);
				if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{
					const FColoredData* religion = GameMapReference->GetDataFromID(ProvinceSelectedData.Religion, *GameMapReference->GetVisualPropertiesDataMap());
					editedText->SetValues(FText::FromString(religion->DataName), FText::FromString(FString("______")));
				}
			

			}
		}
		return;
	}

	if (editedText == CultureCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString value = Text.ToString();
				ProvinceSelectedData.Culture = FName(*value);
				if(GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{

					const FColoredData* culture = GameMapReference->GetDataFromID(ProvinceSelectedData.Culture, *GameMapReference->GetVisualPropertiesDataMap());
					editedText->SetValues(FText::FromString(culture->DataName), FText::FromString(FString("______")));
				}
			}
		}
		return;
	}

	if (editedText == PopulationCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (!Text.IsNumeric())
			{
				UE_LOG(LogInteractiveMap, Warning, TEXT("Invalid value in population, please insert a number"));
				editedText->SetValues(Text, FText::FromString(FString("______")));
			}
			if (GameMapReference)
			{
				FString value = Text.ToString();
				ProvinceSelectedData.Population = FCString::Atoi(*value);
				if (GameMapReference->UpdateProvinceData(ProvinceSelectedData, ProvinceSelectedID))
				{
					editedText->SetValues(Text, FText::FromString(FString("______")));
				}
			}
		}
		return;
	}

}


