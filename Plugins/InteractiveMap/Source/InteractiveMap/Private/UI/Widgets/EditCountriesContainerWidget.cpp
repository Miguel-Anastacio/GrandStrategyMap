// Copyright 2024 An@stacioDev All rights reserved.
#include "UI/Widgets/EditCountriesContainerWidget.h"
#include "UI/Widgets/CountryEditorWidget.h"
#include "Map/InteractiveMap.h"
#include "Components/ScrollBox.h"


void UEditCountriesContainerWidget::SetInteractiveMapReference(AInteractiveMap* map)
{
	GameMapReference = map;
	AddCountriesToBox(*map->GetCountryDataMap());
}
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

