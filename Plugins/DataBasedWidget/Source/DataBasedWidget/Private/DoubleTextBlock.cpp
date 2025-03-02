// Copyright 2024 An@stacioDev All rights reserved.

#include "DoubleTextBlock.h"
#include "Components/RichTextBlock.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
void UDoubleTextBlock::SetValues(const FText& LabelText, const FText& ValueText) const
{
	Label->SetText(LabelText);
	Text->SetText(ValueText);
}

void UDoubleTextBlock::PerformAction_Implementation(const FName& PropertyName, const FInstancedStruct& InstancedStruct) const
{
	bool bResult = false;
	const FString ValueText = UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(InstancedStruct, PropertyName.ToString(), bResult);
	SetValues(FText::FromName(PropertyName), FText::FromString(ValueText));
}

