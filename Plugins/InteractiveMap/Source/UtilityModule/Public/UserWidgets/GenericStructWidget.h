// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
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
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitFromStruct(const FInstancedStruct& InstancedStruct);

	virtual void InitFromStruct_Implementation(const FInstancedStruct& InstancedStruct);
	
	template<typename T>
	void InitializeFromStructEditable(const T& structInstance, UClass* classPtr)
	{
		// Iterate over the struct's properties
		for (TFieldIterator<FProperty> It(T::StaticStruct()); It; ++It)
		{
			FProperty* Property = *It;
			FName PropertyName = Property->GetFName();
			bool bResult = false;
			FString PropertyValue = UADStructUtilsFunctionLibrary::GetPropertyValue<FString>(Property, &structInstance, bResult);
			if(bResult)
			{
				// CreateEditableFieldWidget(PropertyName, PropertyValue, classPtr);
			}
		}
	}
	int Columns = 1;
	// int Rows = 0;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Struct Panel Display")
	class UGridPanel* MainPanel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetType;

	UPROPERTY(EditAnywhere)
	UScriptStruct* StructType;
	
	UPROPERTY()
	TMap<FName, TWeakObjectPtr<UUserWidget>> WidgetFields;
	
protected:
	// To be implemented in Blueprint or C++
	// UFUNCTION(BlueprintNativeEvent, Category = "Struct Viewer")
	// void CreateEditableFieldWidget(const FName& FieldName, const FString& FieldValue, UClass* classPtr);
	// UFUNCTION(BlueprintNativeEvent, Category = "Struct Viewer")
	// void CreateFieldWidget(const FName& FieldName, const FString& FieldValue);

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Custom Struct Display")
	void CreatePanelSlots();
};
