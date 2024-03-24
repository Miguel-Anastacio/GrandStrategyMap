// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
//#include "../../Map/MapEnums.h"
#include "Map/MapUtils.h"
#include "ProvinceEditorWidget.generated.h"

class UButtonWidget;
class UEditableText;
class URichTextBlock; 
class UCustomEditableText;
UCLASS(Abstract, BlueprintType)
class CLICKABLEMAP_API UProvinceEditorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetProvinceData(const FProvinceData& data, FName provinceID);
	UFUNCTION(BlueprintCallable)
	void SetInteractiveMapReference(class AInteractiveMap* map);

protected:
	void NativeOnInitialized() override;
	//void NativeOnInitialized() override;
//	UFUNCTION()
//	void SetMapMode(UButtonWidget* button);
//
protected:
	UPROPERTY(meta = (BindWidget))
	UCustomEditableText* NameCustomInput;
	UPROPERTY(meta = (BindWidget))
	UCustomEditableText* OwnerCustomInput;
	UPROPERTY(meta = (BindWidget))
	UCustomEditableText* ReligionCustomInput;
	UPROPERTY(meta = (BindWidget))
	UCustomEditableText* CultureCustomInput;
	UPROPERTY(meta = (BindWidget))
	UCustomEditableText* PopulationCustomInput;
	//UPROPERTY(meta = (BindWidget))
	//URichTextBlock* ProvinceName;
	//UPROPERTY(meta = (BindWidget))
	//UEditableText* NameInput;

	//UPROPERTY(meta = (BindWidget))
	//URichTextBlock* OwnerName;
	//UPROPERTY(meta = (BindWidget))
	//UEditableText* OwnerInput;

	//UFUNCTION()
	//void UpdateProvinceName(const FText& Text, ETextCommit::Type CommitMethod);
	//UFUNCTION()
	//void UpdateProvinceOwner(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateProvinceData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod);
	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* NameText;
	//UPROPERTY(meta = (BindWidget))
	//UEditableText* NameInput;

	FName ProvinceSelectedID;
	FProvinceData ProvinceSelectedData;
	class AInteractiveMap* GameMapReference;

};
