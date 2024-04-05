// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/ProvinceEditorWidget.h"
#include "UI/Widgets/CustomEditableText.h"
#include "Components/EditableText.h"
#include "Components/RichTextBlock.h"
#include "Map/ClickableMap.h"
#include "InteractiveMap.h"

void UProvinceEditorWidget::SetProvinceData(const FProvinceData& data, FName provinceID)
{

	const FCountryData* owner = GameMapReference->GetDataFromID(data.Owner, *GameMapReference->GetCountryDataMap());
	const FColoredData* culture = GameMapReference->GetDataFromID(data.Culture, *GameMapReference->GetVisualPropertiesDataMap());
	const FColoredData* religion = GameMapReference->GetDataFromID(data.Religion, *GameMapReference->GetVisualPropertiesDataMap());
	FString ownerName = owner->CountryName;
	FString cultureName = culture->DataName;
	FString religionName = religion->DataName;

	NameCustomInput->SetValues(FText::FromString(data.ProvinceName), FText::FromString(FString("______")));
	OwnerCustomInput->SetValues(FText::FromString(ownerName), FText::FromString(FString("______")));
	ReligionCustomInput->SetValues(FText::FromString(religionName), FText::FromString(FString("______")));
	CultureCustomInput->SetValues(FText::FromString(cultureName), FText::FromString(FString("______")));
	PopulationCustomInput->SetValues(FText::AsNumber(data.Population), FText::FromString(FString("______")));

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
					editedText->SetValues(Text, FText::FromString(FString("______")));
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
					editedText->SetValues(Text, FText::FromString(FString("______")));
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
					editedText->SetValues(Text, FText::FromString(FString("______")));
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


