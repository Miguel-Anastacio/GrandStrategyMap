// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/DynamicTextureComponent.h"
#include "RHICommandList.h"
#include "Rendering/Texture2DResource.h"

// Sets default values for this component's properties
UDynamicTextureComponent::UDynamicTextureComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UDynamicTextureComponent::BeginPlay()
{
	//Must be called before Super otherwise Blueprint can't modify texture at BeginPlay.
	//InitializeTexture();

	Super::BeginPlay();

	// ...

}

void UDynamicTextureComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	FMemory::Free(TextureData);
	FMemory::Free(TextureRegion);
}

// Called every frame
void UDynamicTextureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*UpdateTexture();

	if (DynamicMaterial)
	{
		DynamicMaterial->SetTextureParameterValue("DynamicTexture", DynamicTexture);
	}*/
}

void UDynamicTextureComponent::FillTexture(FLinearColor Color)
{
	for (uint32 i = 0; i < TextureTotalPixels; i++)
	{
		TextureData[i * 4] = Color.B * 255;
		TextureData[i * 4 + 1] = Color.G * 255;
		TextureData[i * 4 + 2] = Color.R * 255;
		TextureData[i * 4 + 3] = Color.A * 255;
	}
}

void UDynamicTextureComponent::SetPixelColor(int32 X, int32 Y, FLinearColor Color)
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

void UDynamicTextureComponent::SetPixelValue(int32 X, int32 Y, FColor Color)
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

//void UDynamicTextureComponent::DrawRectangle(int32 StartX, int32 StartY, int32 Width, int32 Height, FLinearColor Color)
//{
//	for (int32 y = 0; y < Height; y++)
//	{
//		for (int32 x = 0; x < Width; x++) {
//			SetPixelColor(StartX + x, StartY + y, Color);
//		}
//	}
//}
//
//void UDynamicTextureComponent::DrawCircle(int32 StartX, int32 StartY, int32 Size, FLinearColor Color, bool Center /*= true*/)
//{
//	float radius = Size / 2;
//	int32 offset = FMath::Floor(radius * Center);
//
//	for (int32 y = 0; y < Size; y++)
//	{
//		for (int32 x = 0; x < Size; x++) {
//			FVector pos = FVector(x - radius, y - radius, 0);
//			if (pos.Size2D() <= radius) {
//				SetPixelColor((StartX - offset) + x, (StartY - offset) + y, Color);
//			}
//
//		}
//	}
//}

void UDynamicTextureComponent::DrawFromTexture(int32 StartX, int32 StartY, UTexture2D* Texture, FLinearColor Filter /*= FLinearColor::White*/)
{
	if (!Texture) {
		return;
	}

	int32 width = Texture->GetSizeX();
	int32 height = Texture->GetSizeY();
	uint32 texDataSize = width * height * 4;
	uint8* texData = new uint8[texDataSize];

	FTexture2DMipMap& readMip = Texture->GetPlatformData()->Mips[0];
	readMip.BulkData.GetCopy((void**)&texData);


	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++) {
			uint32 start = ((y * width) + x) * 4;

			SetPixelValue(StartX + x, StartY + y, FColor((texData[start + 2] * Filter.R), texData[start + 1] * Filter.G, texData[start] * Filter.B, texData[start + 3] * Filter.A));
		}
	}

	FMemory::Free(texData);
}

void UDynamicTextureComponent::InitializeTexture()
{
	// Get Total Pixels in Texture
	TextureTotalPixels = TextureWidth * TextureHeight;

	// Get Total Bytes of Texture - Each pixel has 4 bytes for RGBA
	TextureDataSize = TextureTotalPixels * 4;
	TextureDataSqrtSize = TextureWidth * 4;

	// Initialize Texture Data Array
	TextureData = new uint8[TextureDataSize];

	// Create Dynamic Texture Object
	DynamicTexture = UTexture2D::CreateTransient(TextureWidth, TextureHeight);
	DynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicTexture->SRGB = 0;
	DynamicTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	DynamicTexture->Filter = TextureFilter::TF_Nearest;
	DynamicTexture->AddToRoot();
	DynamicTexture->UpdateResource();

	//Create Update Region Struct Instance
	TextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureWidth, TextureHeight);

	FillTexture(FLinearColor::Green);
	UpdateTexture();
}

//void UDynamicTextureComponent::InitializeTexture(UTexture2D* texture)
//{
//	// Get Total Pixels in Texture
//	TextureTotalPixels = TextureWidth * TextureHeight;
//
//	// Get Total Bytes of Texture - Each pixel has 4 bytes for RGBA
//	TextureDataSize = TextureTotalPixels * 4;
//	TextureDataSqrtSize = TextureWidth * 4;
//
//	// Initialize Texture Data Array
//	TextureData = new uint8[TextureDataSize];
//
//	// Create Dynamic Texture Object
//	texture = UTexture2D::CreateTransient(TextureWidth, TextureHeight);
//	texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
//	texture->SRGB = 0;
//	texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
//	texture->Filter = TextureFilter::TF_Nearest;
//	texture->AddToRoot();
//	texture->UpdateResource();
//
//	//Create Update Region Struct Instance
//	TextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureWidth, TextureHeight);
//
//	FillTexture(FLinearColor::Green);
//	UpdateTexture();
////}

void UDynamicTextureComponent::UpdateTexture(bool bFreeData)
{
	if (DynamicTexture == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dynamic Texture tried to Update Texture but it hasn't been initialized!"));
		return;
	}

	struct FUpdateTextureRegionsData
	{
		FTexture2DResource* Texture2DResource;
		FRHITexture2D* TextureRHI;
		int32 MipIndex;
		uint32 NumRegions;
		FUpdateTextureRegion2D* Regions;
		uint32 SrcPitch;
		uint32 SrcBpp;
		uint8* SrcData;
	};

	FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

	UTexture2D* Texture = DynamicTexture;

	RegionData->Texture2DResource = (FTexture2DResource*)Texture->GetResource();
	RegionData->TextureRHI = RegionData->Texture2DResource->GetTexture2DRHI();
	RegionData->MipIndex = 0;
	RegionData->NumRegions = 1;
	RegionData->Regions = TextureRegion;
	RegionData->SrcPitch = TextureDataSqrtSize;
	RegionData->SrcBpp = 4;
	RegionData->SrcData = TextureData;

	ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
		[RegionData, bFreeData, Texture](FRHICommandListImmediate& RHICmdList)
		{
			for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
			{
				int32 CurrentFirstMip = Texture->FirstResourceMemMip;
				if (RegionData->TextureRHI && RegionData->MipIndex >= CurrentFirstMip)
				{
					RHIUpdateTexture2D(
						RegionData->TextureRHI,
						RegionData->MipIndex - CurrentFirstMip,
						RegionData->Regions[RegionIndex],
						RegionData->SrcPitch,
						RegionData->SrcData
						+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
						+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
					);
				}
			}
			if (bFreeData) {
				FMemory::Free(RegionData->Regions);
				FMemory::Free(RegionData->SrcData);
			}
			delete RegionData;
		});
}

//
//void UDynamicTextureComponent::InitializeTexture(uint32 width, uint32 height, UTexture* texture)
//{
//
//}

void UDynamicTextureComponent::InitializeTexture(uint32 width, uint32 height)
{
	TextureWidth = width;
	TextureHeight = height;
	InitializeTexture();
}
void UDynamicTextureComponent::DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, uint8* dataBuffer, FLinearColor filter)
{
	if (!texture)
		return;
	int32 width = texture->GetSizeX();
	int32 height = texture->GetSizeX();


	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++) 
		{
			uint32 start = ((y * width) + x) * 4;

			//// SetPixelValue takes FColor, SetPixel
			//SetPixelValue(startX + x, 
			//			startY + y, 
			//			FColor(dataBuffer[start] * filter.R, 
			//				dataBuffer[start + 1] * filter.G, 
			//				dataBuffer[start + 2] * filter.B, 
			//				dataBuffer[start + 3] * filter.A)
			//			);
		}
	}

}

void UDynamicTextureComponent::DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, const TArray<float> dataBuffer, FLinearColor filter)
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
UE_ENABLE_OPTIMIZATION

UTexture2D* UDynamicTextureComponent::GetTexture() const
{
	return DynamicTexture;
}
