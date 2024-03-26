// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Utilities/ColorPicker.h"
#include "Widgets/SBoxPanel.h"
#include "Components/PanelWidget.h"
//#include "Widgets/Colors/SColorPicker.h"

UColorPicker::UColorPicker(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//void UColorPicker::NativeConstruct()
//{
//	Super::NativeConstruct();
//
//    // Create the Slate color picker widget
//    //ColorPickerWidget = SNew(SColorPicker)
//    //    .OnValueChanged(this, &UColorPicker::OnColorChanged);
//
//    //// Add the Slate widget to the UMG container widget
//    //if (ColorPickerContainer)
//    //{
//    //    //TSharedRef<SWidget> ColorPickerWidgetRef = ColorPickerWidget.ToSharedRef();
//    //    //ColorPickerContainer->ClearChildren();
//    //    //ColorPickerContainer->
//    //    //    .AutoHeight()
//    //    //    .AutoWidth()
//    //    //    [
//    //    //        ColorPickerWidgetRef
//    //    //    ];
//    //}
//}
////
////void UColorPicker::OnColorChanged(FLinearColor NewColor)
////{
////}
