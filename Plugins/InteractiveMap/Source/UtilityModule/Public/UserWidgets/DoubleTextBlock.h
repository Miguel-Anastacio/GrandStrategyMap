// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DoubleTextBlock.generated.h"

/**
 * Two text blocks
 */
class URichTextBlock;
UCLASS(Abstract, BlueprintType)
class UTILITYMODULE_API UDoubleTextBlock : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Sets the initial values for the editable text widget. */
    UFUNCTION(BlueprintCallable)
    void SetValues(const FText& LabelText, const FText& ValueText) const;

protected:
    /** The rich text block displaying the identifier. */
    UPROPERTY(meta = (BindWidget))
    URichTextBlock* Label;

    /** The rich text block displaying the current value. */
    UPROPERTY(meta = (BindWidget))
    URichTextBlock* Text;

};
