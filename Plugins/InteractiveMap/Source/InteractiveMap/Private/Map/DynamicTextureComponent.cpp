// Thank you J.Anthracite for the tutorial
// https://dev.epicgames.com/community/learning/tutorials/ow9v/unreal-engine-creating-a-runtime-editable-texture-in-c
// Copyright 2024 An@stacioDev All rights reserved.

#include "Map/DynamicTextureComponent.h"
#include "RHICommandList.h"
#include "Components/ActorComponent.h"
#include "RenderingThread.h"
//#include "Rendering/Texture2DResource.h"
#include "Engine/Texture2D.h"
#include "InteractiveMap.h"

// Sets default values for this component's properties
UDynamicTextureComponent::UDynamicTextureComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// mapGenerator = new MapGenerator::Map(1024, 1024);
	// ...
}

void UDynamicTextureComponent::SetDynamicTexture(UDynamicTexture* DynTexture)
{
	this->DynamicTextureData = DynTexture;
	// InitializeTexture(DynTexture->TextureWidth, DynTexture->TextureHeight);
	// UpdateTexture();
}

UMaterialInstanceDynamic* UDynamicTextureComponent::GetMaterialInstance() const
{
	if(DynamicTextureData)
	{
		return DynamicTextureData->DynamicMaterial;
	}
	return nullptr;
}

void UDynamicTextureComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	FMemory::Free(TextureRegion);
}

void UDynamicTextureComponent::FillTexture(const FLinearColor& Color) const
{
	if(DynamicTextureData)
	{
		DynamicTextureData->FillTexture(Color);
	}
}

void UDynamicTextureComponent::SetPixelColor(int32 X, int32 Y, const FLinearColor& Color) const
{
	if(DynamicTextureData)
	{
		DynamicTextureData->SetPixelColor(X, Y, Color);
	}
}

void UDynamicTextureComponent::SetPixelValue(int32 X, int32 Y, const FColor& Color) const
{
	if(DynamicTextureData)
	{
		DynamicTextureData->SetPixelValue(X, Y, Color);
	}
}


void UDynamicTextureComponent::DrawFromTexture(int32 StartX, int32 StartY, UTexture2D* Texture, FLinearColor Filter /*= FLinearColor::White*/)
{
	if(DynamicTextureData)
	{
		DynamicTextureData->DrawFromTexture(StartX, StartY, Texture, Filter);
	}
}

void UDynamicTextureComponent::InitializeTexture()
{
	if(!DynamicTextureData)
	{
		//log
		return;
	}
	// Create Dynamic Texture Object
	const uint32 TextureWidth = DynamicTextureData->TextureWidth;
	const uint32 TextureHeight = DynamicTextureData->TextureHeight;
	DynamicTexture = UTexture2D::CreateTransient(TextureWidth, TextureHeight);
	DynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicTexture->SRGB = 0;
#if WITH_EDITORONLY_DATA
	DynamicTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	DynamicTexture->Filter = TextureFilter::TF_Nearest;
	DynamicTexture->AddToRoot();
	DynamicTexture->UpdateResource();

	//Create Update Region Struct Instance
	TextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureWidth, TextureHeight);

	// FillTexture(FLinearColor::Red);
	UpdateTexture();
}

void UDynamicTextureComponent::UpdateTexture(bool bFreeData)
{
	if(!DynamicTextureData)
	{
		UE_LOG(LogInteractiveMap, Warning, TEXT("Dynamic Texture tried to Update Texture but Data is Null"));
		return;
	}
	
	if (DynamicTexture == nullptr)
	{
		UE_LOG(LogInteractiveMap, Warning, TEXT("Dynamic Texture tried to Update Texture but it hasn't been initialized!"));
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
	RegionData->SrcPitch = DynamicTextureData->GetTextureDataSqrtSize();
	RegionData->SrcBpp = 4;
	RegionData->SrcData = DynamicTextureData->GetTextureData()->GetData();

	ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)
	(
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
		}
	);
}

void UDynamicTextureComponent::InitializeTexture(uint32 width, uint32 height)
{
	// Create Dynamic Texture Object
	const uint32 TextureWidth = width;
	const uint32 TextureHeight = height;
	DynamicTexture = UTexture2D::CreateTransient(TextureWidth, TextureHeight);
	DynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicTexture->SRGB = 0;
#if WITH_EDITORONLY_DATA
	DynamicTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	DynamicTexture->Filter = TextureFilter::TF_Nearest;
	DynamicTexture->AddToRoot();
	DynamicTexture->UpdateResource();

	//Create Update Region Struct Instance
	TextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureWidth, TextureHeight);

	// FillTexture(FLinearColor::Red);
	UpdateTexture();
}
void UDynamicTextureComponent::DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, uint8* dataBuffer, FLinearColor filter)
{
	if(DynamicTextureData)
	{
		DynamicTextureData->DrawFromDataBuffer(startX, startY, texture, dataBuffer, filter);
	}
}

void UDynamicTextureComponent::DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, const TArray<float> dataBuffer, FLinearColor filter)
{
	if(DynamicTextureData)
	{
		DynamicTextureData->DrawFromDataBuffer(startX, startY, texture, dataBuffer, filter);
	}
}
UTexture2D* UDynamicTextureComponent::GetTexture() const
{
	return DynamicTexture;
}
