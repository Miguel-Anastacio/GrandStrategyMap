#pragma once
#include "CoreMinimal.h"
#include "source/map/components/LookupMap.h"
#include "MapGeneratorWrapper.generated.h"

USTRUCT(NotBlueprintType)
struct SHAREDMODULE_API FTileIdData 
{
	GENERATED_BODY()
	FTileIdData()  : ColorString(TEXT("")), Name(-1) {};
	FTileIdData(const char* color, int32 id) :  ColorString(color), Name(id) {};

	UPROPERTY()
	FString ColorString = "";
	UPROPERTY()
	int32 Name = -1;

	// Serialize to JSON
	void SerializeToJson(const TSharedPtr<FJsonObject>& OutJsonObject) const
	{
		OutJsonObject->SetStringField(TEXT("Name"), FString::FromInt(Name));
		OutJsonObject->SetStringField(TEXT("Color"), ColorString);
	}
};

