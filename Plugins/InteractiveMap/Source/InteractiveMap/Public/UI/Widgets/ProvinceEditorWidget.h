// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
//#include "../../Map/MapEnums.h"
#include "InstancedStruct.h"
#include "Map/MapUtils.h"
#include "ProvinceEditorWidget.generated.h"

class UCustomButtonWidget;
class UEditableText;
class URichTextBlock; 
class UCustomEditableText;
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UProvinceEditorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Sets the data for a specific province identified by its ID.
	 *
	 * @param data The data to set for the province.
	 * @param provinceID The unique identifier of the province.
	 */
	UFUNCTION(BlueprintCallable, Category = "Province Editor")
	void SetProvinceData(const FInstancedStruct& Data, int ProvinceID);

	/**
	 * Sets the reference to the interactive map actor.
	 *
	 * @param map Pointer to the interactive map actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Province Editor")
	void SetInteractiveMapReference(class AClickableMap* Map);

protected:
	/**
	 * Overrides the initialization method to perform custom initialization tasks.
	 */
	virtual void NativeOnInitialized() override;

	/**
	 * Handles updates to province data when an editable text is modified.
	 *
	 * @param editedText The editable text widget being modified.
	 * @param Text The updated text.
	 * @param CommitMethod The type of text commit event.
	 */
	UFUNCTION(Category = "Province Editor")
	void UpdateProvinceData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod);
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Province Editor")
	class UGenericStructWidget* ProvinceDataWidget;
	// The unique identifier of the currently selected province.
	int ProvinceSelectedID; 
	// Data structure containing information about the currently selected province.
	FInstancedStruct ProvinceSelectedData; 
	// Pointer to the interactive map actor used as a reference for this component.
	UPROPERTY()
	class AClickableMap* GameMapReference; 

};
