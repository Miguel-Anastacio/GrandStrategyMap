// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "CustomEditableText.h"
#include "Types/SlateEnums.h"
#include "CustomStructWidget.h"
#include "FileIO/DataManagerFunctionLibrary.h"
#include "TestAdvancedStructWidget.generated.h"

class UVerticalBox;

UCLASS(Abstract, BlueprintType)
class UTILITYMODULE_API UTestAdvStructWidget : public UGenericStructWidget	
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UCustomEditableText> CustomEditableTextClass;

	UFUNCTION(BlueprintCallable, Category = "Test")
	void InitializeFromStruct(const FTestAdvanced& structObject, const TSubclassOf<UCustomEditableText>& classText);
};

inline void UTestAdvStructWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

inline void UTestAdvStructWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

inline void UTestAdvStructWidget::InitializeFromStruct(const FTestAdvanced& structObject, const TSubclassOf<UCustomEditableText>& classText)
{
	InitializeFromStructEditable<FTestAdvanced>(structObject, classText);
}
