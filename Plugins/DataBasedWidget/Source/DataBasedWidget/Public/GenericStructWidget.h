// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "Types/SlateEnums.h"
#include "GenericStructWidget.generated.h"

class UWidgetMapDataAsset;
class UCustomEditableText;
class UVerticalBox;
UCLASS(Abstract, BlueprintType)
class DATABASEDWIDGET_API UGenericStructWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	const UStruct* GetDataClass() const;
	
#if WITH_EDITOR
	void CreateGenericWidget(UWidgetMapDataAsset* DataAssetWidgetMap);
#endif

	// wrapper to init from Instanced Struct exposed to BP
	UFUNCTION(BlueprintCallable,  Category = "Generic Struct Widget")
	virtual void InitFromStruct(const FInstancedStruct& InstancedStruct);
	
	// wrapper to init from UObject exposed to BP
	UFUNCTION(BlueprintCallable,  Category = "Generic Struct Widget")
	virtual void InitFromObject(const UObject* Object);

	UPROPERTY(EditAnywhere, Category = "Generic Struct Widget")
	int Columns = 1;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Struct Panel Display")
	class UGridPanel* MainPanel;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generic Struct Widget")
	class UWidgetMapDataAsset* DataAssetWidgetMap;
#endif
	
	UPROPERTY(BlueprintReadOnly, Category = "Generic Struct Widget")
	TMap<FName, UUserWidget*> WidgetFields;

	// Should Widget Hold a reference at runtime to the data that is displaying?
	
protected:
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Generic Struct Widget")
	void CreatePanelSlots();
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Generic Struct Widget")
	void CreateMainPanel();
#endif

	void UpdateGridPosition(uint8& ColumnIndex, uint8& RowIndex) const;
	void InitFromData(const UStruct* ClassType, const void* Data);
private:	
	void InitializeWidgetFields();
	
};
