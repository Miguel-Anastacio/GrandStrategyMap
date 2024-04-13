// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Map/MapUtils.h"
#include "CountryEditorWidget.generated.h"
//
class UCustomEditableText;
class URamaColorPicker;
/**
 * Widget for editing country data.
 */
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UCountryEditorWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Sets the country data for this widget. */
    UFUNCTION(BlueprintCallable, Category = "Country Editor")
    void SetCountryData(const FCountryData& data, FName id);

    /** Sets the reference to the interactive map. */
    UFUNCTION(BlueprintCallable, Category = "Country Editor")
    void SetInteractiveMapReference(class AClickableMap* map);
protected:
    virtual void NativeOnInitialized() override;

    /** Updates the data when text is edited. */
    UFUNCTION(Category = "Country Editor")
    void UpdateData(UCustomEditableText* editedText, const FText& Text, ETextCommit::Type CommitMethod);

    /** Updates the color of the country. */
    UFUNCTION(Category = "Country Editor")
    void UpdateColor(const FLinearColor& color);

protected:
    /** The custom editable text for editing the country name. */
    UPROPERTY(meta = (BindWidget))
    UCustomEditableText* NameCustomInput;

    /** The color picker for choosing the country color. */
    UPROPERTY(meta = (BindWidget))
    class URamaColorPicker* ColorPicker;

    /** The tag of the country being edited. */
    FName CountryTag;

    /** The data of the country being edited. */
    FCountryData CountryData;

    /** Reference to the interactive map. */
    class AClickableMap* GameMapReference;

};
