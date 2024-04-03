// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "Types/SlateEnums.h"
////#include "../../Map/MapEnums.h"
#include "Map/MapUtils.h"
#include "CountryEditorWidget.generated.h"
//
class UCustomEditableText;
class URamaColorPicker;
UCLASS(Abstract, BlueprintType)
class CLICKABLEMAP_API UCountryEditorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetCountryData(const FCountryData& data, FName id);
	UFUNCTION(BlueprintCallable)
	void SetInteractiveMapReference(class AInteractiveMap* map);
//
protected:
	void NativeOnInitialized() override;
	UFUNCTION()
	void UpdateData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateColor(const FLinearColor& color);
//
//protected:
	UPROPERTY(meta = (BindWidget))
	UCustomEditableText* NameCustomInput;
	UPROPERTY(meta = (BindWidget))
	URamaColorPicker* ColorPicker;
	
	FName CountryTag;
	FCountryData CountryData;
	class AInteractiveMap* GameMapReference;
//	UPROPERTY(meta = (BindWidget))
//	UCustomEditableText* ReligionCustomInput;
//	UPROPERTY(meta = (BindWidget))
//	UCustomEditableText* CultureCustomInput;
//	UPROPERTY(meta = (BindWidget))
//	UCustomEditableText* PopulationCustomInput;
//
//
//	FName ProvinceSelectedID;
//	FProvinceData ProvinceSelectedData;
//	class AInteractiveMap* GameMapReference;

};
