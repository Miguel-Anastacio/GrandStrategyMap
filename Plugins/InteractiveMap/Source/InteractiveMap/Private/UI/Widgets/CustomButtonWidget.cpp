// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/CustomButtonWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"
void UCustomButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (IsValid(ButtonText))
	{
		ButtonText->SetText(Text);
	}
}

void UCustomButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UCustomButtonWidget::OnButtonClicked);
	}
}

void UCustomButtonWidget::OnButtonClicked()
{
	OnClickedDelegate.Broadcast(this);
}
