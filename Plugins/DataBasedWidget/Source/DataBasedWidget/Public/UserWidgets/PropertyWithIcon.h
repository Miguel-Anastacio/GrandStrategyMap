// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "GenericUserWidgetInterface.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "PropertyWithIcon.generated.h"

/**
 * Two text blocks and a image
 */
class URichTextBlock;
UCLASS(Abstract, BlueprintType)
class DATABASEDWIDGET_API UWPropGenPropertyWithIcon : public UUserWidget, public IPropGenDataDrivenUserWidgetInterface, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "PropertyWithIcon")
    void SetValues(const FText& LabelText, const FText& ValueText) const;
    UFUNCTION(BlueprintCallable, Category = "PropertyWithIcon")
    void SetIcon(UTexture2D* NewIcon) const;

protected:
    // IPropGenDataDrivenUserWidgetInterface
    virtual void InitFromData(const FName& PropertyName, const UStruct* ClassType, const void* Data) const override;
    // IPropGenDataDrivenUserWidgetInterface

    // IUserObjectListEntry
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    // IUserObjectListEntry
    
    /** The rich text block displaying the identifier. */
    UPROPERTY(meta = (BindWidgetOptional))
    URichTextBlock* Label;

    /** The rich text block displaying some other value. */
    UPROPERTY(meta = (BindWidgetOptional))
    URichTextBlock* Text;
    
    /** The icon displaying the current value. */
    UPROPERTY(meta = (BindWidget))
    class UImage* Icon = nullptr;
};
