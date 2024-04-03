// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/ButtonWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"
void UButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (IsValid(ButtonText))
	{
		ButtonText->SetText(Text);
	}
}

void UButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UButtonWidget::OnButtonClicked);
	}
}

void UButtonWidget::OnButtonClicked()
{
	OnClickedDelegate.Broadcast(this);
}
