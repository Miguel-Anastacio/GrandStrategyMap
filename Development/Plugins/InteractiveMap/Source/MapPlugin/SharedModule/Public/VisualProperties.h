// Copyright 2024 An@stacioDev All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "Misc/EngineVersionComparison.h"
#include "SharedModule.h"
#if UE_VERSION_NEWER_THAN(5, 4, 4)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "Materials/MaterialInterface.h"
#include "VisualProperties.generated.h"

UENUM(BlueprintType)
enum class EClimate : uint8
{
	Arctic,
	Desert,
	Jungle,
	Swamp,
	Savana,
};

UCLASS(Abstract, Blueprintable)
class SHAREDMODULE_API UVisualProperty : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Visual Property")
	FColor GetColorForProperty(const FInstancedStruct& Data);
	UFUNCTION(BlueprintCallable, Category = "Visual Property")
	UMaterialInterface* GetMaterial() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Property")	
	FString Name;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Property")
	FColor DefaultColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Property")
	TSoftObjectPtr<UMaterialInterface> MaterialInstance = nullptr;
};

template<typename  T>
FColor GetColorForPropertyFromMap(const FInstancedStruct& Data, const TMap<T, FColor>& Map, const FString& PropertyName, const FColor& DefaultColor)
{
	bool bResult = false;
	const T Key = UAtkStructUtilsFunctionLibrary::GetPropertyValueFromStruct<T>(Data, PropertyName, bResult);
	if (!bResult)
	{
		return DefaultColor;
	}
	const FColor* Color = Map.Find(Key);
	if(!Color)
	{
		UE_LOG(LogMapSharedModule, Warning, TEXT("Key not in ColorMap"));
		return DefaultColor;
	}
	return *Color;
}

/**
 * Example of Visual Property that works based on a map of colors to enum values
 */
UCLASS(Blueprintable, BlueprintType)
class SHAREDMODULE_API UClimateVisualProperty : public UVisualProperty
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Property")
	TMap<EClimate, FColor> PropertyColorMap;

	virtual FColor GetColorForProperty_Implementation(const FInstancedStruct& Data) override
	{
		return 	GetColorForPropertyFromMap(Data, PropertyColorMap, Name, DefaultColor);
	}
	
};

/**
 * Base class for mapping Visual Properties from strings to colors
 */
UCLASS(Blueprintable, BlueprintType)
class SHAREDMODULE_API UStringMapVisualProperty : public UVisualProperty
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Property")
	TMap<FString, FColor> PropertyColorMap;

	virtual FColor GetColorForProperty_Implementation(const FInstancedStruct& Data) override
	{
		return 	GetColorForPropertyFromMap(Data, PropertyColorMap, Name, DefaultColor);
	}
};

/**
 * Example of Visual Property that works based on a numeric value and saturates or desaturates the color accordingly
 */
UCLASS(Blueprintable, BlueprintType)
class SHAREDMODULE_API UNumericVisualProperty : public UVisualProperty
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Numeric Visual Property")
	FColor PropertyColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Numeric Visual Property")
	int32 MinValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Numeric Visual Property")
	int32 MaxValue = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Numeric Visual Property", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinSaturation = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Numeric Visual Property", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxSaturation = 1.0f;

	virtual FColor GetColorForProperty_Implementation(const FInstancedStruct& Data) override;
};

