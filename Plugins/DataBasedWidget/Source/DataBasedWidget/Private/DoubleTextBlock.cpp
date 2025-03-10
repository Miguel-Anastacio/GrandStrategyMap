// Copyright 2024 An@stacioDev All rights reserved.

#include "DoubleTextBlock.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "Components/RichTextBlock.h"

void UDoubleTextBlock::SetValues(const FText& LabelText, const FText& ValueText) const
{
	if(Label)
		Label->SetText(LabelText);
	if(Text)
		Text->SetText(ValueText);
}

void UDoubleTextBlock::InitFromData(const FName& PropertyName,const UStruct* ClassType,  const void* Data) const
{
	const FProperty* Property = ClassType->FindPropertyByName(PropertyName);
	const FString ValueText = UADStructUtilsFunctionLibrary::GetPropertyValueAsString(Property, Data);
	SetValues(FText::FromName(PropertyName), FText::FromString(ValueText));
}
