// Copyright 2024 An@stacioDev All rights reserved.

#include "UI/Widgets/CustomButtonWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"
void UCustomButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetButtonText(ButtonText);
}

void UCustomButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UCustomButtonWidget::OnButtonClicked);
	}
}

void UCustomButtonWidget::SetButtonText(const FString& NewText)
{
	if (IsValid(ButtonTextWidget))
	{
		ButtonText = NewText;
		ButtonTextWidget->SetText(FText::FromString(NewText));
	}
}

void UCustomButtonWidget::OnButtonClicked()
{
	OnClickedDelegate.Broadcast(this);
}
