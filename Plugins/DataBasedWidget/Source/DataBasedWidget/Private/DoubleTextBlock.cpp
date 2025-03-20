// Copyright 2024 An@stacioDev All rights reserved.

#include "DoubleTextBlock.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "Components/RichTextBlock.h"

void UDoubleTextBlock::SetValues(const FText& NewLabelText, const FText& NewValueText) const
{
	if(Label)
		Label->SetText(NewLabelText);
	if(Text)
		Text->SetText(NewValueText);
}

void UDoubleTextBlock::InitFromData(const FName& PropertyName,const UStruct* ClassType,  const void* Data) const
{
	const FProperty* Property = UADStructUtilsFunctionLibrary::FindPropertyByDisplayName(ClassType, PropertyName);
	const FString PropertyValueText = UADStructUtilsFunctionLibrary::GetPropertyValueAsString(Property, Data);
	SetValues(FText::FromName(PropertyName), FText::FromString(PropertyValueText));
}

void UDoubleTextBlock::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}
