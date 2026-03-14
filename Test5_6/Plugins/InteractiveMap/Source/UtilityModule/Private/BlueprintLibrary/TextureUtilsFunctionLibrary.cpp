// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture.h"
#include "TextureCompiler.h"
#include "UtilityModule.h"
#include "TextureResource.h"

TArray<uint8> UAtkTextureUtilsFunctionLibrary::ReadTextureToArray(UTexture2D* Texture)
{
	// Initialize an empty TArray to hold the texture data
	TArray<uint8> DataArray;
	if(!IsTextureValid(Texture))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Read Texture to Array not Valid Texture"));
		return DataArray;
	}
#if WITH_EDITOR
	FTextureCompilingManager::Get().FinishCompilation({Texture});
#endif
	// Lock the texture for reading
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);

	// Copy the texture data into the TArray
	const uint8* Data = static_cast<const uint8*>(TextureData);
	DataArray.Append(Data, Mip.BulkData.GetBulkDataSize()); // Append data to the TArray

	// Unlock the texture
	Mip.BulkData.Unlock();
	return DataArray;
}

std::vector<uint8> UAtkTextureUtilsFunctionLibrary::ReadTextureToVector(UTexture2D* Texture)
{
    if(!IsTextureValid(Texture))
    {
        UE_LOG(LogUtilityModule, Error, TEXT("Read Texture to Buffer not Valid Texture"));
        return {};
    }
        
    #if WITH_EDITOR
        FTextureCompilingManager::Get().FinishCompilation({Texture});
    #endif
        
        FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
        
        // Lock the texture
        void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);
        const uint8* Data = static_cast<const uint8*>(TextureData);
        
        // Get size and copy data
        const int32 DataSize = Mip.BulkData.GetBulkDataSize();
        std::vector<uint8_t> buffer(Data, Data + DataSize);
        
        // Now safe to unlock
        Mip.BulkData.Unlock();
        
        return buffer;
}

FColor UAtkTextureUtilsFunctionLibrary::GetColorFromUV(uint32 Width, uint32 Height, const FVector2D& Uv,
                                                       const TArray<uint8>& DataBuffer)
{
	if(Width * Height * 4 != DataBuffer.Num())
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Buffer size does not match texture size"));
		return FColor();
	}
	
	bool bResult = false;
	const int32 Index = GetIndexFromUV(Uv, Width, Height, bResult);
	if(!bResult)
	{
		return FColor();
	}

	return GetColorFromIndex(Index, DataBuffer);
}

FColor UAtkTextureUtilsFunctionLibrary::GetColorFromUV(UTexture2D* Texture, const FVector2D& Uv, const TArray<uint8>& DataBuffer)
{
	if(!IsTextureValid(Texture))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Get Color From UV not Valid Texture"));
		return FColor();
	}

	const int32 Width = Texture->GetSizeX();
	const int32 Height = Texture->GetSizeY();
	
	if(Width * Height * 4 != DataBuffer.Num())
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Buffer size does not match texture size"));
		return FColor();
	}

	bool bResult = false;
	const int32 Index = GetIndexFromUV(Uv, Width, Height, bResult);
	if(!bResult)
	{
		return FColor();
	}

	return GetColorFromIndex(Index, DataBuffer);
}

FColor UAtkTextureUtilsFunctionLibrary::GetColorFromUV(UTexture2D* Texture, const FVector2D& Uv, const uint8* DataBuffer)
{
	if(!IsTextureValid(Texture))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Get Color From UV not Valid Texture"));
		return FColor();
	}
	
	if(!DataBuffer)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Null Buffer"));
		return FColor();
	}

	const int32 Width = Texture->GetSizeX();
	const int32 Height = Texture->GetSizeY();
	bool bResult = false;
	const int32 Index = GetIndexFromUV(Uv, Width, Height, bResult);
	if(!bResult)
	{
		return FColor();
	}
	
	return GetColorFromIndex(Index, DataBuffer);
}
FColor UAtkTextureUtilsFunctionLibrary::GetColorFromUV(uint32 Width, uint32 Height, const FVector2D& Uv,
	const uint8* DataBuffer)
{
	bool bResult = false;
	const int32 Index = GetIndexFromUV(Uv, Width, Height, bResult);
	if(!bResult)
	{
		return FColor();
	}
	return GetColorFromIndex(Index, DataBuffer);
}

TArray<uint32> UAtkTextureUtilsFunctionLibrary::PackUint8ToUint32(const TArray<uint8>& DataArray)
{
	TArray<uint32> PackedArray;
	// Ensure the input array has at least 4 bytes to process
	const int32 NumElements = DataArray.Num();
	const int32 NumPackedElements = NumElements / 4; // Number of uint32 values we can pack
	if (NumElements % 4 != 0)
	{
		return PackedArray;
	}
		
	PackedArray.Reserve(NumPackedElements);
	for (int32 i = 0; i < NumPackedElements; ++i)
	{
		// Grab the next 4 bytes from the uint8 array
		uint32 PackedValue = 0;

		// Combine 4 uint8s into a single uint32
		PackedValue |= static_cast<uint32>(DataArray[i * 4 + 0] << 0);   // First byte at the lowest byte position	B
		PackedValue |= static_cast<uint32>(DataArray[i * 4 + 1] << 8);   // Second byte shifted by 8 bits			G
		PackedValue |= static_cast<uint32>(DataArray[i * 4 + 2] << 16);  // Third byte shifted by 16 bits			R
		PackedValue |= static_cast<uint32>(DataArray[i * 4 + 3] << 24);  // Fourth byte shifted by 24 bits			A
		// Add the packed value to the uint32 array
		PackedArray.Emplace(PackedValue);
	}
	return PackedArray;
}

TArray<uint8> UAtkTextureUtilsFunctionLibrary::UnPackUint32ToUint8(const TArray<uint32>& DataArray)
{
	TArray<uint8> UnPackedArray;
	// Ensure the input array has at least 4 bytes to process
	const int32 NumElements = DataArray.Num();
	const int32 NumUnPackedElements = NumElements * 4; // Number of uint32 values we can pack
	if (NumUnPackedElements % 4 != 0)
	{
		return UnPackedArray;
	}
		
	UnPackedArray.Reserve(NumUnPackedElements);
	for (int32 i = 0; i < NumElements; ++i)
	{
		// Grab the next 4 bytes from the uint8 array
		const uint32 PackedValue = DataArray[i];
		UnPackedArray.Emplace(static_cast<uint8>(PackedValue >> 0));
		UnPackedArray.Emplace(static_cast<uint8>(PackedValue >> 8));
		UnPackedArray.Emplace(static_cast<uint8>(PackedValue >> 16));
		UnPackedArray.Emplace(static_cast<uint8>(255));
	}
	return UnPackedArray;
};

FColor UAtkTextureUtilsFunctionLibrary::GetColorFromIndex(uint32 Index, const TArray<uint8>& DataBuffer)
{
	// Data read from texture is in BGRA format
	return FColor(DataBuffer[Index + 2],
		DataBuffer[Index + 1],
		DataBuffer[Index],
		DataBuffer[Index + 3]);
}

FColor UAtkTextureUtilsFunctionLibrary::GetColorFromIndex(uint32 Index, const uint8* DataBuffer)
{
	// Data read from texture is in BGRA format
	return FColor(DataBuffer[Index + 2],
		DataBuffer[Index + 1],
		DataBuffer[Index],
		DataBuffer[Index + 3]);
}

int32 UAtkTextureUtilsFunctionLibrary::GetIndexFromUV(const FVector2D& Uv, uint32 Width, uint32 Height,
	bool& bOutResult)
{
	const uint32 y = Uv.Y * Height;
	const uint32 x = Uv.X * Width;

	uint32 Index = (y * Width + x) * 4;

	Index = Index % 4 + Index;

	if (Index > Width * Height * 4 - 4)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Invalid index from uvs"));
		bOutResult = false;
		return -1;
	}

	bOutResult = true;
	return Index;
}

bool UAtkTextureUtilsFunctionLibrary::IsTextureValid(const UTexture2D* Texture)
{
	if(!Texture)
	{
		return false;
	}
	if(Texture->CompressionSettings != TC_EditorIcon)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("%s has wrong compression settings, please use UserInterface"), *Texture->GetName());
		return false;
	}
	// in editor check mip map settings
#if WITH_EDITOR
	if(Texture->MipGenSettings != TMGS_NoMipmaps)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("%s has wrong Mipmap settings, please use NoMipmaps"), *Texture->GetName());
		return false;
	}
#endif
	return true;
}
