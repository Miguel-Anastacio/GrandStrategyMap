// Thank you J.Anthracite for the tutorial
// https://dev.epicgames.com/community/learning/tutorials/ow9v/unreal-engine-creating-a-runtime-editable-texture-in-c
// Copyright 2024 An@stacioDev All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "RHIResources.h"
#include "Rendering/Texture2DResource.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DynamicTexture.generated.h"

/**
 * Component for dynamically updating a texture.
 */
USTRUCT()
struct FArrayOfPixels
{
	GENERATED_BODY()
	TArray<FIntVector2> PixelPositions;

	explicit FArrayOfPixels(const FIntVector2& NewPosition)
	{
		PixelPositions.Emplace(NewPosition);
	};
	FArrayOfPixels(){};
};

struct FUpdateTextureRegion2D;
UCLASS(ClassGroup = (Custom))
class INTERACTIVEMAP_API UDynamicTexture : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDynamicTexture();
public:

	void SetTextureData(const TArray<uint8>& NewData);
	void SetTextureData(const TArray<uint32>& NewData);
	/**
	 * Fill the entire texture with a specified color.
	 * @param Color The color to fill the texture with.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void FillTexture(const FLinearColor& Color);

	/**
	 * Set the color of a pixel in the texture.
	 *
	 * @param X The x-coordinate of the pixel.
	 * @param Y The y-coordinate of the pixel.
	 * @param Color The color to set the pixel to.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void SetPixelColor(int32 X, int32 Y, const FLinearColor& Color);
	void SetPixelValue(int32 X, int32 Y, const FColor& Color);

	void InitMaterial(UMaterialInstanceDynamic* MaterialInstanceDynamic, UTexture* LookupTexture, float TextureType);

	/**
	 * Draw a texture onto the dynamic texture.
	 *
	 * @param StartX The x-coordinate to start drawing from.
	 * @param StartY The y-coordinate to start drawing from.
	 * @param Texture The texture to draw onto the dynamic texture.
	 * @param Filter The filter to apply when drawing the texture.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void DrawFromTexture(int32 StartX, int32 StartY, UTexture2D* Texture, FLinearColor Filter = FLinearColor::White);

	void InitializeDynamicTexture(uint32 width, uint32 height);
	void DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, uint8* dataBuffer, FLinearColor filter = FLinearColor::White);
	void DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, const TArray<float> dataBuffer, FLinearColor filter = FLinearColor::White);

	FORCEINLINE TArray<uint8>* GetTextureData() { return &TextureData; };
	FORCEINLINE TArray<uint8> GetTextureDataCopy() { return TextureData; };
	FORCEINLINE uint32 GetTextureDataSqrtSize() const { return TextureDataSqrtSize; };

	int32 TextureWidth = 512;
	int32 TextureHeight = 512;

	UPROPERTY(BlueprintReadWrite, Category = "Texture")
	UMaterialInstanceDynamic* DynamicMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	FName DynamicMaterialParamName = "DynamicTexture";

private:
	// Initialize the Dynamic Texture
	void InitializeDynamicTexture();

private:
	// Array that contains the Texture Data
	TArray<uint8> TextureData;

	// Total Bytes of Texture Data
	uint32 TextureDataSize;

	// Texture Data Sqrt Size
	uint32 TextureDataSqrtSize;

	// Total Count of Pixels in Texture
	uint32 TextureTotalPixels;
};
