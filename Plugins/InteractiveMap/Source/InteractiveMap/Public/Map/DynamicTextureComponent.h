// Thank you J.Anthracite for the tutorial
// https://dev.epicgames.com/community/learning/tutorials/ow9v/unreal-engine-creating-a-runtime-editable-texture-in-c
// Copyright 2024 An@stacioDev All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RHIResources.h"
#include "Rendering/Texture2DResource.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DynamicTextureComponent.generated.h"
/**
 * Component for dynamically updating a texture.
 */
struct FUpdateTextureRegion2D;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTIVEMAP_API UDynamicTextureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDynamicTextureComponent();
public:

	/**
	 * Fill the entire texture with a specified color.
	 * @param Color The color to fill the texture with.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void FillTexture(FLinearColor Color);

	/**
	 * Set the color of a pixel in the texture.
	 *
	 * @param X The x-coordinate of the pixel.
	 * @param Y The y-coordinate of the pixel.
	 * @param Color The color to set the pixel to.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void SetPixelColor(int32 X, int32 Y, FLinearColor Color);
	void SetPixelValue(int32 X, int32 Y, FColor Color);

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

	/**
	 * Update the texture object from texture data.
	 * @param bFreeData Whether to free the data after updating.
	 */
	void UpdateTexture(bool bFreeData = false);

	/**
	* Get the dynamic texture.
	* @return The dynamic texture.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dynamic Texture")
	UTexture2D* GetTexture() const;

	void InitializeTexture(uint32 width, uint32 height);
	void DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, uint8* dataBuffer, FLinearColor filter = FLinearColor::White);
	void DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, const TArray<float> dataBuffer, FLinearColor filter = FLinearColor::White);

	FORCEINLINE TArray<uint8>* GetTextureData() { return &TextureData; };

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	int32 TextureWidth = 512;

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	int32 TextureHeight = 512;

	UPROPERTY(BlueprintReadWrite, Category = "Texture")
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	FName DynamicMaterialParamName = "DynamicTexture";

protected:
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	// Initialize the Dynamic Texture
	void InitializeTexture();

private:
	// Array that contains the Texture Data
	TArray<uint8> TextureData;

	// Total Bytes of Texture Data
	uint32 TextureDataSize;

	// Texture Data Sqrt Size
	uint32 TextureDataSqrtSize;

	// Total Count of Pixels in Texture
	uint32 TextureTotalPixels;

	// Texture Object
	UPROPERTY()
	UTexture2D* DynamicTexture;

	// Update Region Struct
	FUpdateTextureRegion2D* TextureRegion;


};
