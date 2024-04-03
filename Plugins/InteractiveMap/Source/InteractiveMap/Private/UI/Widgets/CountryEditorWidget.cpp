#include "UI/Widgets/CountryEditorWidget.h"
#include "UI/Widgets/CustomEditableText.h"
#include "Map/InteractiveMap.h"
#include "UI/Utilities/RamaColorPicker.h"
UE_DISABLE_OPTIMIZATION


void UCountryEditorWidget::SetCountryData(const FCountryData& data, FName id)
{
	CountryData = data;
	CountryTag = id;
	NameCustomInput->SetValues(FText::FromString(data.CountryName), FText::FromString(FString("______")));
	ColorPicker->JoyColor = data.Color;
}

void UCountryEditorWidget::SetInteractiveMapReference(AInteractiveMap* map)
{
	GameMapReference = map;
}



void UCountryEditorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	NameCustomInput->TextCommitDelegate.AddDynamic(this, &UCountryEditorWidget::UpdateData);
	ColorPicker->OnColorChanged.AddDynamic(this, &UCountryEditorWidget::UpdateColor);
}

void UCountryEditorWidget::UpdateColor(const FLinearColor& color)
{
	if (GameMapReference)
	{
		GameMapReference->UpdateCountryColor(color, CountryTag);
	}
}

UE_ENABLE_OPTIMIZATION
void UCountryEditorWidget::UpdateData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod)
{
	if (editedText == NameCustomInput)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (GameMapReference)
			{
				FString dummy = Text.ToString();
				CountryData.CountryName = dummy;
				if (GameMapReference->UpdateCountryData(CountryData, CountryTag))
				{
					editedText->SetValues(Text, FText::FromString(FString("______")));
				}
			}
		}
		return;
	}
}
