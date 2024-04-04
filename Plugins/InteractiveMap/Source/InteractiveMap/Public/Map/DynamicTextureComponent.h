// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RHIResources.h"
#include "Rendering/Texture2DResource.h"
#include "DynamicTextureComponent.generated.h"

struct FUpdateTextureRegion2D;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTIVEMAP_API UDynamicTextureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDynamicTextureComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	int32 TextureWidth = 512;

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	int32 TextureHeight = 512;

	UPROPERTY(BlueprintReadWrite, Category = "Texture")
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Texture")
	FName DynamicMaterialParamName = "DynamicTexture";

	/// Fill Entire Texture with a specified color.
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void FillTexture(FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void SetPixelColor(int32 X, int32 Y, FLinearColor Color);
	void SetPixelValue(int32 X, int32 Y, FColor Color);

	//UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	//void DrawRectangle(int32 StartX, int32 StartY, int32 Width, int32 Height, FLinearColor Color);

	//UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	//void DrawCircle(int32 StartX, int32 StartY, int32 Size, FLinearColor Color, bool Center = true);

	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void DrawFromTexture(int32 StartX, int32 StartY, UTexture2D* Texture, FLinearColor Filter = FLinearColor::White);

	//Update Texture Object from Texture Data
	void UpdateTexture(bool bFreeData = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dynamic Texture")
	UTexture2D* GetTexture() const;

	//UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	//void InitializeTexture(uint32 width, uint32 height, UTexture* texture);
	void InitializeTexture(uint32 width, uint32 height);
	void DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, uint8* dataBuffer, FLinearColor filter = FLinearColor::White);
	void DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, const TArray<float> dataBuffer, FLinearColor filter = FLinearColor::White);
	//void DrawFromDataBuffer(int32 startX, int32 startY, UTexture2D* texture, const TArray<float> dataBuffer, FLinearColor filter = FLinearColor::White);

	FORCEINLINE TArray<uint8>* GetTextureData() { return &TextureData; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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

	// Initialize the Dynamic Texture
	void InitializeTexture();


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
private:




};
