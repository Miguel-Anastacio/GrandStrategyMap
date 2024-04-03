#include "UI/Widgets/EditCountriesContainerWidget.h"
#include "UI/Widgets/CountryEditorWidget.h"
#include "Map/InteractiveMap.h"
#include "Components/ScrollBox.h"
//UE_DISABLE_OPTIMIZATION



//
UE_DISABLE_OPTIMIZATION
void UEditCountriesContainerWidget::SetInteractiveMapReference(AInteractiveMap* map)
{
	GameMapReference = map;
	AddCountriesToBox(*map->GetCountryDataMap());
}

//
////UE_ENABLE_OPTIMIZATION
//
//void UEditCountriesContainerWidget::NativeOnInitialized()
//{
//	Super::NativeOnInitialized();
//	NameCustomInput->TextCommitDelegate.AddDynamic(this, &UCountryEditorWidget::UpdateData);
//	ColorPicker->OnColorChanged.AddDynamic(this, &UCountryEditorWidget::UpdateColor);
//}
//
//void UEditCountriesContainerWidget::UpdateColor(const FLinearColor& color)
//{
//	if (GameMapReference)
//	{
//		GameMapReference->UpdateCountryColor(color, CountryTag);
//	}
//}
//
//void UEditCountriesContainerWidget::UpdateData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod)
//{
//
//	if (editedText == NameCustomInput)
//	{
//		if (CommitMethod == ETextCommit::OnEnter)
//		{
//			if (GameMapReference)
//			{
//				CountryData.CountryName = Text.ToString();
//				GameMapReference->UpdateCountryData(CountryData, CountryTag);
//			}
//		}
//		return;
//	}
//}
void UEditCountriesContainerWidget::AddCountriesToBox(const TMap<FName, FCountryData>& countryData)
{
	for (auto& country : countryData)
	{
		UCountryEditorWidget* widget = CreateWidget<UCountryEditorWidget>(GetOwningPlayer(), CountryEditorClass);
		widget->AddToViewport();
		widget->SetCountryData(country.Value, country.Key);
		widget->SetInteractiveMapReference(GameMapReference);
		CountriesHolder->AddChild(widget);
	}
}

UE_ENABLE_OPTIMIZATION