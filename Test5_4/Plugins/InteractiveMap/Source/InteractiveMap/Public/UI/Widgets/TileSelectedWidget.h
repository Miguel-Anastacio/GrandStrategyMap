// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "WidgetDataInterface.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 4, 4)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "TileSelectedWidget.generated.h"

UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UTileSelectedWidget : public UUserWidget, public IWidgetDataInterface
{
	GENERATED_BODY()

public:
	virtual void SetData_Implementation(const FInstancedStruct& Data) override;

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Tile Selected")
	class UWPropGenGeneric *ProvinceDataWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Tile Selected")
	class UWPropGenGeneric *OceanTileDataWidget;
	
};
