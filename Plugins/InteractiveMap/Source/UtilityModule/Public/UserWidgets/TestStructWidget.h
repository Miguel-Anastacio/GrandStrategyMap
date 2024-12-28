// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "CustomEditableText.h"
#include "Types/SlateEnums.h"
#include "GenericStructWidget.h"
#include "FileIO/DataStructs.h"
#include "TestStructWidget.generated.h"

class UVerticalBox;

UCLASS(Abstract, BlueprintType)
class UTILITYMODULE_API UTestBasicStructWidget : public UGenericStructWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UCustomEditableText> CustomEditableTextClass;

	UFUNCTION(BlueprintCallable, Category = "Test")
	void InitializeFromStruct(const FTestBasic& structObject, const TSubclassOf<UCustomEditableText>& classText);
};

inline void UTestBasicStructWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

inline void UTestBasicStructWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

inline void UTestBasicStructWidget::InitializeFromStruct(const FTestBasic& structObject, const TSubclassOf<UCustomEditableText>& classText)
{
	InitializeFromStructEditable<FTestBasic>(structObject, classText);
}
