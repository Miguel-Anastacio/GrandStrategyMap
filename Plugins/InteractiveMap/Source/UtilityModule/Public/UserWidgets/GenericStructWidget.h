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
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable)
	void InitFromStruct(const FInstancedStruct& InstancedStruct);

	UPROPERTY(EditAnywhere)
	int Columns = 1;
	UPROPERTY(EditAnywhere)
	int Rows = 0;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Struct Panel Display")
	class UGridPanel* MainPanel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> DefaultWidgetType;
	UPROPERTY(EditAnywhere)
	TMap<FName, TSubclassOf<UUserWidget>> WidgetTypesMap;

	UPROPERTY(EditAnywhere)
	UScriptStruct* StructType;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, UUserWidget*> WidgetFields;

protected:
	// To be implemented in Blueprint or C++
	// UFUNCTION(BlueprintNativeEvent, Category = "Struct Viewer")
	// void CreateEditableFieldWidget(const FName& FieldName, const FString& FieldValue, UClass* classPtr);
	// UFUNCTION(BlueprintNativeEvent, Category = "Struct Viewer")
	// void CreateFieldWidget(const FName& FieldName, const FString& FieldValue);

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Custom Struct Display")
	void CreatePanelSlots();
};
