// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetReactionInterface.h"
#include "ButtonWidget.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUISelectionInputSignature);
/**
 * Custom Button - create custom looks for it by creating BP widgets that inherit from this one
 */
UCLASS(Abstract, BlueprintType)
class MANAGERUI_API UButtonWidget : public UUserWidget, public IWidgetReactionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Button") 
	class UButton* GetButton() const { return Button; };

	virtual void NativePreConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FText Text;

	//// Delegate to use when we wish to trigger the button through an input action
	//// Mostly used to trigger 
	//UPROPERTY(BlueprintCallable, BlueprintAssignable)
	//FOnUISelectionInputSignature UIInputActionDelegate;

protected:
	//UFUNCTION()
	//	void OnInputActionUI();
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URichTextBlock> ButtonText;
	
};
