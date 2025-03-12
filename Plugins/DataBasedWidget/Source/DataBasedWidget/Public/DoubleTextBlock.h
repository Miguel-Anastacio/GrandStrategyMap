// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "GenericUserWidgetInterface.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "DoubleTextBlock.generated.h"

/**
 * Two text blocks
 */
class URichTextBlock;
UCLASS(Abstract, BlueprintType)
class DATABASEDWIDGET_API UDoubleTextBlock : public UUserWidget, public IGenericUserWidgetInterface, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    /** Sets the initial values for the editable text widget. */
    UFUNCTION(BlueprintCallable, Category = "Double Text Block")
    void SetValues(const FText& LabelText, const FText& ValueText) const;

protected:
    // IGenericUserWidgetInterface
    virtual void InitFromData(const FName& PropertyName, const UStruct* ClassType, const void* Data) const override;
    // IGenericUserWidgetInterface

    // IUserObjectListEntry
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    // IUserObjectListEntry
    
    /** The rich text block displaying the identifier. */
    UPROPERTY(meta = (BindWidget))
    URichTextBlock* Label;

    /** The rich text block displaying the current value. */
    UPROPERTY(meta = (BindWidget))
    URichTextBlock* Text;

};
