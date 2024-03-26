// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapModeEditor.generated.h"

/**
 * 
 */
class UCustomEditableText;
UCLASS()
class CLICKABLEMAP_API UMapModeEditor : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCustomEditableText> NameEdit;

	

};
