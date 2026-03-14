// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/DoubleTextBlock.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "Components/RichTextBlock.h"

void UWPropGenDoubleTextBlock::SetValues(const FText& NewLabelText, const FText& NewValueText) const
{
	if(Label)
		Label->SetText(NewLabelText);
	if(Text)
		Text->SetText(NewValueText);
}

void UWPropGenDoubleTextBlock::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWPropGenDoubleTextBlock::InitFromData(const FName& PropertyName,const UStruct* ClassType,  const void* Data) const
{
	FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName(ClassType, PropertyName);
	const FString PropertyValueText = UAtkStructUtilsFunctionLibrary::GetPropertyValueAsString(Property, Data);
	SetValues(FText::FromName(PropertyName), FText::FromString(PropertyValueText));
}

void UWPropGenDoubleTextBlock::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}
