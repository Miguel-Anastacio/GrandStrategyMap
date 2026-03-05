// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#include "UObject/Interface.h"
#if UE_VERSION_NEWER_THAN(5, 4, 4)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "WidgetDataInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UWidgetDataInterface : public UInterface
{
	GENERATED_BODY()
};

class IWidgetDataInterface
{
	GENERATED_BODY()

public:
	// Blueprint-callable AND overridable in BP
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetData(const FInstancedStruct& Data);
};
