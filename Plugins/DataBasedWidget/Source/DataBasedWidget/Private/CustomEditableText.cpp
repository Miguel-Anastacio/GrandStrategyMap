// Copyright 2024 An@stacioDev All rights reserved.

#include "CustomEditableText.h"
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

void UWPropGenCustomEditableText::SetIDText(const FText& text) const
{
	ID->SetText(text);
}

void UWPropGenCustomEditableText::InitFromData(const FName& PropertyName, const UStruct* ClassType, const void* Data) const
{
	SetIDText(FText::FromName(PropertyName));
	const FProperty* Property = ClassType->FindPropertyByName(PropertyName);
	const FString Text = UADStructUtilsFunctionLibrary::GetPropertyValueAsString(Property, Data);
	SetValues(FText::FromString(Text), FText::FromString(Text));
}
