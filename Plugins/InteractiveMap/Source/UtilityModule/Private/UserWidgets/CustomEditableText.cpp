// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/CustomEditableText.h"
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
	// ID->SetText(IDText);
}

void UCustomEditableText::TextCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	TextCommitDelegate.Broadcast(this, Text, CommitMethod);
}

void UCustomEditableText::SetValues(const FText& current, const FText& input)
{
	CurrentValue->SetText(current);
	InputValue->SetText(input);
}

void UCustomEditableText::SetIDText(const FText& text)
{
	ID->SetText(text);
}
