// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "Types/SlateEnums.h"
////#include "../../Map/MapEnums.h"
#include "Map/MapUtils.h"
#include "EditCountriesContainerWidget.generated.h"
//
class UCustomEditableText;
class URamaColorPicker;
class UScrollBox;
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UEditCountriesContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddCountriesToBox(const TMap<FName, FCountryData>& countryData);
	UFUNCTION(BlueprintCallable)
	void SetInteractiveMapReference(class AInteractiveMap* map);
////
//protected:
//	void NativeOnInitialized() override;
//	UFUNCTION()
//	void UpdateData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod);
//	UFUNCTION()
//	void UpdateColor(const FLinearColor& color);
////
protected:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* CountriesHolder;

	UPROPERTY(EditAnywhere, Category = "Country Editor")
	TSubclassOf<class UCountryEditorWidget> CountryEditorClass;
	class AInteractiveMap* GameMapReference;

};
