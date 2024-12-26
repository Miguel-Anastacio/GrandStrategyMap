// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "StructWrapper.h"
#include "StructWrapperInterface.h"
#include "BasicStructWrapper.generated.h"

class SJsonTreeDisplay;
UCLASS(Blueprintable)
class MAPEDITOR_API UBasicStructWrapper : public UStructWrapper
{
	GENERATED_BODY()

public:
	UBasicStructWrapper(const FObjectInitializer& ObjectInitializer);
	
	virtual void OpenFileOfType(const TArray<FString>& FileNames, TSharedPtr<STreeJsonDisplay>& Tree) override;
};
