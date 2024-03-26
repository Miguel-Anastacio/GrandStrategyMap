// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ColorPicker.generated.h"

/**
 * 
 */
class SColorPicker;
UCLASS()
class CLICKABLEMAP_API UColorPicker : public UUserWidget
{

	GENERATED_BODY()

public:
	// Constructor
	UColorPicker(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the UMG widget is constructed
//	virtual void NativeConstruct() override;
//	// Event handler for color changes in the Slate widget
//	void OnColorChanged(FLinearColor NewColor);
//
//protected:
//	// The UMG widget that contains the Slate widget
//	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
//	class UPanelWidget* ColorPickerContainer;
//	// The Slate color picker widget
//	TSharedPtr<SColorPicker> ColorPickerWidget;



};
