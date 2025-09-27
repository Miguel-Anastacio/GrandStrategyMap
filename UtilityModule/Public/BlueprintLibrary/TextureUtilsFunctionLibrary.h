// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TextureUtilsFunctionLibrary.generated.h"
class UTexture2D;
UCLASS()
class UTILITYMODULE_API UAtkTextureUtilsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static const uint8* ReadTextureToBuffer(UTexture2D* Texture);
	static TArray<uint8> ReadTextureToArray(UTexture2D* Texture);

	static FColor GetColorFromUV(UTexture2D* Texture,const FVector2D& Uv, const TArray<uint8>& DataBuffer);
	static FColor GetColorFromUV(UTexture2D* Texture,const FVector2D& Uv, const uint8* DataBuffer);
	static FColor GetColorFromUV(uint32 Width, uint32 Height,const FVector2D& Uv, const TArray<uint8>& DataBuffer);
	static FColor GetColorFromUV(uint32 Width, uint32 Height,const FVector2D& Uv, const uint8* DataBuffer);
	
	static TArray<uint32> PackUint8ToUint32(const TArray<uint8>& DataArray);
	static TArray<uint8> UnPackUint32ToUint8(const TArray<uint32>& DataArray);
	static FColor GetColorFromIndex(uint32 Index, const TArray<uint8>& DataBuffer);
	static FColor GetColorFromIndex(uint32 Index, const uint8* DataBuffer);
	
private:
	static int32 GetIndexFromUV(const FVector2D& Uv, uint32 Width, uint32 Height, bool& bOutResult);

	static bool IsTextureValid(const UTexture2D* Texture);

   
};

