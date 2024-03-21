// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ButtonWidget.h"
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
