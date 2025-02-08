// Copyright 2024 An@stacioDev All rights reserved.

#include "UI/Widgets/CustomButtonWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"
void UCustomButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UCustomButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UCustomButtonWidget::OnButtonClicked);
	}
}

void UCustomButtonWidget::SetButtonText(const FText& NewText) const
{
	if (IsValid(ButtonText))
	{
		ButtonText->SetText(NewText);
	}
}

void UCustomButtonWidget::OnButtonClicked()
{
	OnClickedDelegate.Broadcast(this);
}
