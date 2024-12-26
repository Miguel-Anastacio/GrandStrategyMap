// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
// #include "StructUtils/InstancedStruct.h"
#include "StructWrapper.generated.h"
class STreeJsonDisplay;
UCLASS(NotBlueprintable)
class MAPEDITOR_API UStructWrapper : public UObject
{
    GENERATED_BODY()
	
public:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	// FInstancedStruct InstancedStruct;
	
	virtual void OpenFileOfType( const TArray<FString>& FileNames, TSharedPtr<STreeJsonDisplay>& Tree)
	{
		
	};
};
