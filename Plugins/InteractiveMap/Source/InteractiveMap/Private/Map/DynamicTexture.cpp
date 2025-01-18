// Thank you J.Anthracite for the tutorial
// https://dev.epicgames.com/community/learning/tutorials/ow9v/unreal-engine-creating-a-runtime-editable-texture-in-c
// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/DynamicTexture.h"
#include "RHICommandList.h"
#include "RenderingThread.h"
#include "Engine/Texture2D.h"

// Sets default values for this component's properties
UDynamicTexture::UDynamicTexture()
{
	
}

void UDynamicTexture::FillTexture(const FLinearColor& Color)
{
	for (uint32 i = 0; i < TextureTotalPixels; i++)
	{
		TextureData[i * 4] = Color.B * 255;
		TextureData[i * 4 + 1] = Color.G * 255;
		TextureData[i * 4 + 2] = Color.R * 255;
		TextureData[i * 4 + 3] = Color.A * 255;
	}
}

void UDynamicTexture::SetPixelColor(int32 X, int32 Y, const FLinearColor& Color)
{
	// If Pixel is outside of Texture return
	if (X < 0 || Y < 0 || X >= TextureWidth || Y >= TextureHeight) {
		return;
	}

	// Get the Start of the Pixel Data
	uint32 start = ((Y * TextureWidth) + X) * 4;

	// Set Pixel Value by Offsetting from the Start of the Pixel Data
	TextureData[start] = Color.B;
	TextureData[start + 1] = Color.G * 255;
	TextureData[start + 2] = Color.R * 255;
	TextureData[start + 3] = Color.A * 255;
}

void UDynamicTexture::SetPixelValue(int32 X, int32 Y, const FColor& Color)
{
	// If Pixel is outside of Texture return
	if (X < 0 || Y < 0 || X >= TextureWidth || Y >= TextureHeight) {
		return;
	}

	// Get the Start of the Pixel Data
	uint32 start = ((Y * TextureWidth) + X) * 4;

	// Set Pixel Value by Offsetting from the Start of the Pixel Data
	TextureData[start] = Color.B;
	TextureData[start + 1] = Color.G;
	TextureData[start + 2] = Color.R;
	TextureData[start + 3] = Color.A;
	
}

void UDynamicTexture::InitMaterial(UMaterialInstanceDynamic* MaterialInstanceDynamic, UTexture* LookupTexture, float TextureType)
{
	DynamicMaterial = MaterialInstanceDynamic;
	DynamicMaterial->SetTextureParameterValue("LookUpTexture", LookupTexture);
	DynamicMaterial->SetScalarParameterValue("TextureType", TextureType);
}

void UDynamicTexture::DrawFromTexture(int32 StartX, int32 StartY, UTexture2D* Texture, FLinearColor Filter /*= FLinearColor::White*/)
{
	if (!Texture) {
		return;
	}
	const int32 Width = Texture->GetSizeX();
	const int32 Height = Texture->GetSizeY();
	const uint32 TexDataSize = Width * Height * 4;
	uint8* TexData = new uint8[TexDataSize];

	FTexture2DMipMap& readMip = Texture->GetPlatformData()->Mips[0];
	readMip.BulkData.GetCopy((void**)&TexData);
	
	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++) {
			uint32 start = ((y * Width) + x) * 4;

			SetPixelValue(StartX + x, StartY + y, FColor((TexData[start + 2] * Filter.R), TexData[start + 1] * Filter.G, TexData[start] * Filter.B, TexData[start + 3] * Filter.A));
		}
	}

	FMemory::Free(TexData);
}

void UDynamicTexture::InitializeDynamicTexture()
{
	// Get Total Pixels in Texture
	TextureTotalPixels = TextureWidth * TextureHeight;
	// Get Total Bytes of Texture - Each pixel has 4 bytes for RGBA
	TextureDataSize = TextureTotalPixels * 4;
	TextureDataSqrtSize = TextureWidth * 4;
	// Initialize Texture Data Array
	TextureData.AddDefaulted(TextureDataSize);
	FillTexture(FColor::White);
}

void UDynamicTexture::InitializeDynamicTexture(uint32 width, uint32 height)
{
	TextureWidth = width;
	TextureHeight = height;
	InitializeDynamicTexture();
}
void UDynamicTexture::DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, uint8* dataBuffer, FLinearColor filter)
{
	if (!texture)
		return;
	const int32 Width = texture->GetSizeX();
	const int32 Height = texture->GetSizeX();
	
	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++) 
		{
			uint32 start = ((y * Width) + x) * 4;

			// SetPixelValue takes FColor, SetPixel
			SetPixelValue(startX + x, 
						startY + y, 
						FColor(dataBuffer[start] * filter.R, 
							dataBuffer[start + 1] * filter.G, 
							dataBuffer[start + 2] * filter.B, 
							dataBuffer[start + 3] * filter.A)
						);
		}
	}

}

void UDynamicTexture::DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, const TArray<float> dataBuffer, FLinearColor filter)
{
	if (!texture)
		return;
	int32 width = texture->GetSizeX();
	int32 height = texture->GetSizeY();

	int max = width * height * 4;

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			uint32 start = ((y * width) + x) * 4;

			//// SetPixelValue takes FColor, SetPixel
			SetPixelValue(startX + x, 
						startY + y, 
						FColor(dataBuffer[start] * filter.R, 
							dataBuffer[start + 1] * filter.G, 
							dataBuffer[start + 2] * filter.B, 
							dataBuffer[start + 3] * filter.A)
						);
		}
	}
}
