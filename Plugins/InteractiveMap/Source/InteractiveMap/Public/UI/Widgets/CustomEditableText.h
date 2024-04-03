// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomEditableText.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEditableTextCommitSignature, class UCustomEditableText*, textEdited,
											const FText&, Text, 
											ETextCommit::Type, CommitMethod);
/**
 * 
 */
class UEditableTextBox;
class URichTextBlock;
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UCustomEditableText : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetValues(const FText& text, const FText& input);

public:
	FEditableTextCommitSignature TextCommitDelegate;

protected:
	void NativeOnInitialized() override;
	void NativePreConstruct() override;
protected:
	UPROPERTY(EditAnywhere)
	FText IDText;

	UPROPERTY(meta = (BindWidget))
	URichTextBlock* ID;
	UPROPERTY(meta = (BindWidget))
	URichTextBlock* CurrentValue;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* InputValue;

	UFUNCTION()
	void TextCommited(const FText& Text, ETextCommit::Type CommitMethod);
	
};
