// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"

#include "TextureCompiler.h"
#include "UtilityModule.h"
TArray<uint8> UTextureUtilsFunctionLibrary::ReadTextureToArray(UTexture2D* Texture)
{
	// Initialize an empty TArray to hold the texture data
	TArray<uint8> DataArray;
	if(ValidateTexture(Texture))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Read Texture to Array not Valid Texture"));
		return DataArray;
	} 

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

const uint8* UTextureUtilsFunctionLibrary::ReadTextureToBuffer(UTexture2D* Texture) 
{
	if(ValidateTexture(Texture))
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Read Texture to Buffer not Valid Texture"));
		return nullptr;
	}
	FTextureCompilingManager::Get().FinishCompilation({Texture});
	
	// Lock the texture for reading
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);
	const uint8* Data = static_cast<const uint8*>(TextureData);
	// Unlock the texture
	Mip.BulkData.Unlock();

	return  Data;
}

FColor UTextureUtilsFunctionLibrary::GetColorFromUV(uint32 Width, uint32 Height, const FVector2D& Uv,
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

FColor UTextureUtilsFunctionLibrary::GetColorFromUV(UTexture2D* Texture, const FVector2D& Uv, const TArray<uint8>& DataBuffer)
{
	if(ValidateTexture(Texture))
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

FColor UTextureUtilsFunctionLibrary::GetColorFromUV(UTexture2D* Texture, const FVector2D& Uv, const uint8* DataBuffer)
{
	if(ValidateTexture(Texture))
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
FColor UTextureUtilsFunctionLibrary::GetColorFromUV(uint32 Width, uint32 Height, const FVector2D& Uv,
	const uint8* DataBuffer)
{
	bool bResult = false;
	int32 Index = GetIndexFromUV(Uv, Width, Height, bResult);
	if(!bResult)
	{
		return FColor();
	}
	return GetColorFromIndex(Index, DataBuffer);
}

FColor UTextureUtilsFunctionLibrary::GetColorFromIndex(uint32 Index, const TArray<uint8>& DataBuffer)
{
	// Data read from texture is in GBRA format
	return FColor(DataBuffer[Index+1],
		DataBuffer[Index + 2],
		DataBuffer[Index],
		DataBuffer[Index + 3]);
}

FColor UTextureUtilsFunctionLibrary::GetColorFromIndex(uint32 Index, const uint8* DataBuffer)
{
	// Data read from texture is in GBRA format
	return FColor(DataBuffer[Index+1],
		DataBuffer[Index + 2],
		DataBuffer[Index],
		DataBuffer[Index + 3]);
}

int32 UTextureUtilsFunctionLibrary::GetIndexFromUV(const FVector2D& Uv, uint32 Width, uint32 Height,
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

	return Index;
}

bool UTextureUtilsFunctionLibrary::ValidateTexture(const UTexture2D* Texture)
{
	if(!Texture)
		return false;
	if(Texture->CompressionSettings != TC_EditorIcon)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("%s has wrong compression settings, please use UserInterface"), *Texture->GetName());
		return false;
	}
	// TODO - Validate Mip Map settings

	return true;
}
