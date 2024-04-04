// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Map/MapUtils.h"
#include "EditCountriesContainerWidget.generated.h"
class UCustomEditableText;
class URamaColorPicker;
class UScrollBox;
/**
 * This widget container is responsible for managing country editor widgets.
 * It allows adding countries to the scroll box and sets the reference to the interactive map.
 */
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UEditCountriesContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Adds countries to the scroll box. */
	UFUNCTION(BlueprintCallable, Category = "CountryContainerEditorWidget")
	void AddCountriesToBox(const TMap<FName, FCountryData>& countryData);

	/** Sets the reference to the interactive map. */
	UFUNCTION(BlueprintCallable, Category = "CountryContainerEditorWidget")
	void SetInteractiveMapReference(class AInteractiveMap* map);

protected:
	/** Holds the scroll box containing country editor widgets. */
	UPROPERTY(meta = (BindWidget))
	UScrollBox* CountriesHolder;

	/** The class of the country editor widget. */
	UPROPERTY(EditAnywhere, Category = "Country Editor")
	TSubclassOf<class UCountryEditorWidget> CountryEditorClass;

	/** Reference to the interactive map. */
	class AInteractiveMap* GameMapReference;

};
