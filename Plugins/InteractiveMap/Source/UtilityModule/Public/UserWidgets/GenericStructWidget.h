// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "GenericStructWidget.generated.h"

class UCustomEditableText;
class UVerticalBox;
UCLASS(Abstract, BlueprintType)
class UTILITYMODULE_API UGenericStructWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	
	template<typename T, typename V>
	T GetPropertyValue(FProperty* Property, const V* object)
	{
		// return object->GetPropertyValue<T>(Property);
		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(object);
		T Result = TPropertyTypeFundamentals<T>::GetPropertyValue(ValuePtr);
		return Result;
	}

	static FString GetPropertyValueAsString(FProperty* Property, const void* structObject, bool& OutResult);
	
	template<typename T>
	void InitializeFromStructEditable(const T& structInstance, UClass* classPtr)
	{
		// Iterate over the struct's properties
		for (TFieldIterator<FProperty> It(T::StaticStruct()); It; ++It)
		{
			FProperty* Property = *It;
			FName PropertyName = Property->GetFName();
			bool result = false;
			FString PropertyValue = GetPropertyValueAsString(Property, &structInstance, result);
			if(result)
			{
				CreateEditableFieldWidget(PropertyName, PropertyValue, classPtr);
			}
		}
	}

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Custom Struct Display")
	UVerticalBox* StructHolder;

	TArray<UCustomEditableText*> Fields;
	
protected:
	// To be implemented in Blueprint or C++
	// UFUNCTION(BlueprintNativeEvent, Category = "Struct Viewer")
	void CreateEditableFieldWidget(const FName& FieldName, const FString& FieldValue, UClass* classPtr);
	// UFUNCTION(BlueprintNativeEvent, Category = "Struct Viewer")
	void CreateFieldWidget(const FName& FieldName, const FString& FieldValue);
};
