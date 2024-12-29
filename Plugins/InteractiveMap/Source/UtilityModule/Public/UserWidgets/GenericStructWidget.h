// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FileIO/DataManagerFunctionLibrary.h"
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
	
	template<typename T>
	void InitializeFromStructEditable(const T& structInstance, UClass* classPtr)
	{
		// Iterate over the struct's properties
		for (TFieldIterator<FProperty> It(T::StaticStruct()); It; ++It)
		{
			FProperty* Property = *It;
			FName PropertyName = Property->GetFName();
			bool bResult = false;
			FString PropertyValue = UDataManagerFunctionLibrary::GetPropertyValueAsString(Property, &structInstance, bResult);
			if(bResult)
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
