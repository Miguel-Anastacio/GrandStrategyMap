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
	virtual void PostLoad() override;

	UFUNCTION(BlueprintCallable)
	void InitFromStruct(const FInstancedStruct& InstancedStruct);

	UPROPERTY(EditAnywhere)
	int Columns = 1;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Struct Panel Display")
	class UGridPanel* MainPanel;

	// Widget used by defaulr
	UPROPERTY(EditAnywhere, Category="Generic Struct Widgets")
	TSubclassOf<UUserWidget> DefaultWidgetType;
	// Specify which widget to use for each Property
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generic Struct Widgets")
	TMap<FName, TSubclassOf<UUserWidget>> WidgetTypesMap;

	UPROPERTY(EditAnywhere, Category="Generic Struct Widgets")
	UScriptStruct* StructType;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, UUserWidget*> WidgetFields;

protected:
	void InitializeWidgetTypesMap();
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Generic Struct Widgets")
	void CreatePanelSlots();
};
