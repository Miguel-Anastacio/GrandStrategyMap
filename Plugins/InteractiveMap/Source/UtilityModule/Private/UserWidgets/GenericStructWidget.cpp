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

FString UGenericStructWidget::GetPropertyValueAsString(FProperty* Property, const void* StructObject, bool& OutResult)
{
	OutResult = true;
	if (!Property || !StructObject)
	{
		OutResult = false;
		return TEXT("Invalid Property or Instance");
	}

	const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructObject);

	if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
	{
		return FString::FromInt(IntProperty->GetPropertyValue(ValuePtr));
	}
	else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
	{
		return FString::SanitizeFloat(FloatProperty->GetPropertyValue(ValuePtr));
	}
	else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		return BoolProperty->GetPropertyValue(ValuePtr) ? TEXT("True") : TEXT("False");
	}
	else if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
	{
		return StrProperty->GetPropertyValue(ValuePtr);
	}
	else if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
	{
		return NameProperty->GetPropertyValue(ValuePtr).ToString();
	}
	else if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		return TextProperty->GetPropertyValue(ValuePtr).ToString();
	}
	else
	{
		OutResult = false;
		return TEXT("Invalid Property or Instance");
	}
}

void UGenericStructWidget::CreateEditableFieldWidget(const FName& FieldName, const FString& FieldValue,UClass* classPtr)
{
	UCustomEditableText* newWidget = WidgetTree->ConstructWidget<UCustomEditableText>(classPtr);
	if(newWidget)
	{
		newWidget->SetValues(FText::FromString(FieldValue), FText::FromString(FieldValue));
		newWidget->SetIDText(FText::FromName(FieldName));
		StructHolder->AddChildToVerticalBox(newWidget);
		Fields.Add(newWidget);
	}
}

void UGenericStructWidget::CreateFieldWidget(const FName& FieldName, const FString& FieldValue)
{
	
}
