// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/CustomEditableText.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "Components/EditableTextBox.h"
#include "Components/RichTextBlock.h"
void UWPropGenCustomEditableText::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	InputValue->OnTextCommitted.AddDynamic(this, &UWPropGenCustomEditableText::TextCommited);
}

void UWPropGenCustomEditableText::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UWPropGenCustomEditableText::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}

void UWPropGenCustomEditableText::TextCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	TextCommitDelegate.Broadcast(this, Text, CommitMethod);
}

void UWPropGenCustomEditableText::SetValues(const FText& Current, const FText& Input) const
{
	CurrentValue->SetText(Current);
	InputValue->SetText(Input);
}

void UWPropGenCustomEditableText::SetIDText(const FText& Text) const
{
	ID->SetText(Text);
}

void UWPropGenCustomEditableText::InitFromData(const FName& PropertyName, const UStruct* ClassType, const void* Data) const
{
	SetIDText(FText::FromName(PropertyName));
	const FProperty* Property = UAtkStructUtilsFunctionLibrary::FindPropertyByDisplayName(ClassType, PropertyName);
	const FString Text = UAtkStructUtilsFunctionLibrary::GetPropertyValueAsString(Property, Data);
	SetValues(FText::FromString(Text), FText::FromString(Text));
}
