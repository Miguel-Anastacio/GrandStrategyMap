// Copyright 2024 An@stacioDev All rights reserved.

#include "CustomEditableText.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "Components/EditableTextBox.h"
#include "Components/RichTextBlock.h"
void UCustomEditableText::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	InputValue->OnTextCommitted.AddDynamic(this, &UCustomEditableText::TextCommited);
}

void UCustomEditableText::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UCustomEditableText::TextCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	TextCommitDelegate.Broadcast(this, Text, CommitMethod);
}

void UCustomEditableText::SetValues(const FText& Current, const FText& Input) const
{
	CurrentValue->SetText(Current);
	InputValue->SetText(Input);
}

void UCustomEditableText::SetIDText(const FText& text) const
{
	ID->SetText(text);
}

void UCustomEditableText::PerformAction_Implementation(const FName& PropertyName,
	const FInstancedStruct& InstancedStruct) const
{
	IGenericUserWidgetInterface::PerformAction_Implementation(PropertyName, InstancedStruct);
	SetIDText(FText::FromName(PropertyName));
	bool bResult = false;
	const FString Text = UADStructUtilsFunctionLibrary::GetPropertyValueAsStringFromStruct(InstancedStruct, PropertyName.ToString(), bResult);
	SetValues(FText::FromString(Text), FText::FromString(Text));
}
