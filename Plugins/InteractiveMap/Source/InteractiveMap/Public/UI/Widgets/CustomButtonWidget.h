// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "CustomButtonWidget.generated.h"

/**
 * Delegate signature for custom button clicked events.
 * Parameters:
 *    - Button: The custom button widget that was clicked.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCustomButtonClickedSignature, class UCustomButtonWidget*, Button);

/**
 * Custom Button Widget - allows customization of button appearance and behavior.
 */
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UCustomButtonWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Returns the button widget associated with this custom button. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Button")
    class UButton* GetButton() const { return Button; };

    virtual void NativePreConstruct() override;
    virtual void NativeOnInitialized() override;

    /** The text displayed on the button. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
    FText Text;

    /** Delegate for custom button clicked events. */
    UPROPERTY(BlueprintAssignable)
    FCustomButtonClickedSignature OnClickedDelegate;

protected:
    /** Handles the button click event. */
    UFUNCTION()
    void OnButtonClicked();

    /** The button widget. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> Button;

    /** The rich text block displaying the button text. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class URichTextBlock> ButtonText;
	
};
