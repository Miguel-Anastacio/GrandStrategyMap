// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "GenericWidget/DataDrivenUserWidgetInterface.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "DoubleTextBlock.generated.h"

/**
 * Two text blocks
 */
class URichTextBlock;
UCLASS(Abstract, BlueprintType, DisplayName=DoubleTextBlock)
class DATABASEDWIDGET_API UWPropGenDoubleTextBlock : public UUserWidget, public IPropGenDataDrivenUserWidgetInterface, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    /** Sets the initial values for the editable text widget. */
    UFUNCTION(BlueprintCallable, Category = "Double Text Block")
    void SetValues(const FText& NewLabelText, const FText& NewValueText) const;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
protected:
    // IPropGenDataDrivenUserWidgetInterface
    virtual void InitFromData(const FName& PropertyName, const UStruct* ClassType, const void* Data) const override;
    // IPropGenDataDrivenUserWidgetInterface
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
