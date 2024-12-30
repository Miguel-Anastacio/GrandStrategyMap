// Copyright 2024 An@stacioDev All rights reserved.

#include "UserWidgets/GenericStructWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Field/FieldSystemNoiseAlgo.h"
#include "UserWidgets/CustomEditableText.h"

void UGenericStructWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UGenericStructWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UGenericStructWidget::CreateEditableFieldWidget(const FName& FieldName, const FString& FieldValue,UClass* ClassPtr)
{
	if(UCustomEditableText* NewWidget = WidgetTree->ConstructWidget<UCustomEditableText>(ClassPtr))
	{
		NewWidget->SetValues(FText::FromString(FieldValue), FText::FromString(FieldValue));
		NewWidget->SetIDText(FText::FromName(FieldName));
		StructHolder->AddChildToVerticalBox(NewWidget);
		Fields.Add(NewWidget);
	}
}

void UGenericStructWidget::CreateFieldWidget(const FName& FieldName, const FString& FieldValue)
{
	
}
