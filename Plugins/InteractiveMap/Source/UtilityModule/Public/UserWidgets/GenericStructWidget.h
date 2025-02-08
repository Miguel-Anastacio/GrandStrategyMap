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
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable,  Category = "Generic Struct Widget")
	void InitFromStruct(const FInstancedStruct& InstancedStruct);

	UPROPERTY(EditAnywhere, Category = "Generic Struct Widget")
	int Columns = 1;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Struct Panel Display")
	class UGridPanel* MainPanel;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generic Struct Widget")
	class UStructWidgetMapDataAsset* DataAssetWidgetMap;
#endif
	
	UPROPERTY(BlueprintReadOnly, Category = "Generic Struct Widget")
	TMap<FName, UUserWidget*> WidgetFields;
	UPROPERTY(BlueprintReadOnly, Category = "Generic Struct Widget")
	UScriptStruct* StructType;

protected:
#if WITH_EDITOR
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Generic Struct Widget")
	void CreatePanelSlots();
#endif
	
};
