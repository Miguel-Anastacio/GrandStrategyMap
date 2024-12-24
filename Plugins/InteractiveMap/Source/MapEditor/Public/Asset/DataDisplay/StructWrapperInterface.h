// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StructWrapperInterface.generated.h"

class STreeJsonDisplay;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStructWrapperInterface : public UInterface
{
	GENERATED_BODY()
};

class MAPEDITOR_API IStructWrapperInterface
{
	GENERATED_BODY()

public:
	// Pure virtual function to be implemented by any class that implements this interface
	virtual void OpenFileOfType(const TArray<FString>& FileNames, TSharedPtr<STreeJsonDisplay>& Tree) = 0;
};
