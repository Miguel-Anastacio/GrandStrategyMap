// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/CustomEditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/RichTextBlock.h"
UE_DISABLE_OPTIMIZATION
void UCustomEditableText::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	InputValue->OnTextCommitted.AddDynamic(this, &UCustomEditableText::TextCommited);
}

void UCustomEditableText::NativePreConstruct()
{
	Super::NativePreConstruct();
	ID->SetText(IDText);
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
UE_ENABLE_OPTIMIZATION