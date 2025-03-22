// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/PropertyWithIcon.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/PropertyUtilityFunctionLibrary.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"

void UWPropGenPropertyWithIcon::SetValues(const FText& LabelText, const FText& ValueText) const
{
	if(Label)
		Label->SetText(LabelText);
	if(Text)
		Text->SetText(ValueText);
}

void UWPropGenPropertyWithIcon::SetIcon(UTexture2D* NewIcon) const
{
	if(Icon && NewIcon)
	{
		Icon->SetBrushFromTexture(NewIcon);
	}
}

void UWPropGenPropertyWithIcon::InitFromData(const FName& PropertyName,const UStruct* ClassType,  const void* Data) const
{
	const FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName(ClassType, PropertyName);
	const FString ValueText = UAtkStructUtilsFunctionLibrary::GetPropertyValueAsString(Property, Data);
	SetValues(FText::FromName(PropertyName), FText::FromString(ValueText));

	SetIcon(UAtkPropertyUtilityFunctionLibrary::GetPropertyAs<UTexture2D>(Property, UTexture2D::StaticClass(), Data));
}

void UWPropGenPropertyWithIcon::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}
