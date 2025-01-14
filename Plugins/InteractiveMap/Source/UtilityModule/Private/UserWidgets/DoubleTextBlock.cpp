// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/DoubleTextBlock.h"
#include "Components/RichTextBlock.h"
void UDoubleTextBlock::SetValues(const FText& LabelText, const FText& ValueText) const
{
	Label->SetText(LabelText);
	Text->SetText(ValueText);
}

