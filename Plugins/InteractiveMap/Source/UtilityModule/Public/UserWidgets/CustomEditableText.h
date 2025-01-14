// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomEditableText.generated.h"

/**
 * Delegate signature for editable text commit events.
 * Parameters:
 *    - textEdited: The editable text widget that was edited.
 *    - Text: The new text value after editing.
 *    - CommitMethod: The method by which the editing was committed.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEditableTextCommitSignature, class UCustomEditableText*, textEdited,
											                                 const FText&, Text, 
											                                 ETextCommit::Type, CommitMethod);
/**
 * A custom editable text widget that supports text editing events.
 */
class UEditableTextBox;
class URichTextBlock;
UCLASS(Abstract, BlueprintType)
class UTILITYMODULE_API UCustomEditableText : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Sets the initial values for the editable text widget. */
    void SetValues(const FText& text, const FText& input) const;
    void SetIDText(const FText& text) const;

public:
    /** Delegate for editable text commit events. */
    UPROPERTY(BlueprintAssignable, Category = "CustomEditableText")
    FEditableTextCommitSignature TextCommitDelegate;

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;

protected:
    /** The text displayed as the identifier. */
    UPROPERTY(EditAnywhere, Category = "CustomEditableText")
    FText IDText;

    /** The rich text block displaying the identifier. */
    UPROPERTY(meta = (BindWidget))
    URichTextBlock* ID;

    /** The rich text block displaying the current value. */
    UPROPERTY(meta = (BindWidget))
    URichTextBlock* CurrentValue;

    /** The editable text box for input. */
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* InputValue;

    /** Handles text committed event. */
    UFUNCTION()
    void TextCommited(const FText& Text, ETextCommit::Type CommitMethod);
	
};
