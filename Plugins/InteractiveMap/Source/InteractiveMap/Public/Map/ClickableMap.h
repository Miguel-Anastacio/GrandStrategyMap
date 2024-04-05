// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapUtils.h"
#include "MapEnums.h"
#include "ClickableMap.generated.h"

class UTextureRenderTarget2D;
class UDynamicTextureComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMapDataChangedSignature, MapMode, mode, FName, provinceID);
UCLASS()
class INTERACTIVEMAP_API AClickableMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClickableMap();

	UFUNCTION(BlueprintCallable, Category = "Map")
	virtual void InitializeMap();

	virtual void Tick(float DeltaTime) override;
	//------------------------------- Data -----------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	TMap<FVector, FName> GetLookUpTable() const;

	TMap<FName, FProvinceData>* GetProvinceDataMap();
	TMap<FName, FCountryData>* GetCountryDataMap();
	TMap<FName, FColoredData>* GetVisualPropertiesDataMap();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	FName GetProvinceID(const FVector& color, bool& out_result) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	void GetProvinceData(FName name, FProvinceData& out_data) const;
	FProvinceData* GetProvinceData(FName name);


	UFUNCTION(BlueprintCallable, Category = "Map Data")
	bool UpdateProvinceData(const FProvinceData& data, FName id);
	UFUNCTION(BlueprintCallable, Category = "Map Data")
	bool UpdateCountryData(const FCountryData& data, FName id);

	// Get data from tag/id
	template<class T>
	const T* GetDataFromID(FName tag, const TMap<FName, T>& mapToSearch)
	{
		const T* data = mapToSearch.Find(tag);
		return data;
	}

	//------------------------------- Visual Data -----------------------------------

	UFUNCTION(BlueprintCallable, Category = "Map Visual Data")
	FColor GetColorFromLookUpTexture(FVector2D uv) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Map Mode")
	void SetMapMode(MapMode mode);
	virtual void SetMapMode_Implementation(MapMode mode);

	// Update Visual Data
	UFUNCTION(BlueprintCallable, Category = "Map Visual Data")
	bool UpdateCountryColor(const FLinearColor& color, FName id);

	//------------------------------- Visual ----------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Map Visual")
	void UpdateProvinceHovered(const FColor& color);
	UFUNCTION(BlueprintCallable, Category = "Map Visual")
	void SetBorderVisibility(bool status);

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
	FMapDataChangedSignature MapDataChangedDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	FColor GetColorFromUV(UTexture2D* texture, FVector2D uv) const;
	
	/** Reads the look up texture and populates the dynamic textures data with the right pixel colors based on the ProvinceDataMap (MapDataComponent)*/
	void SaveMapTextureData();
	void CreateMapTexture(UDynamicTextureComponent* textureCompoment);

	/**
	* Update map texture per province.
	*
	* @param mode The map mode.
	* @param provinceID The ID of the province.
	* @param newColor The new color.
	*/
	UFUNCTION(BlueprintCallable, Category = "Map Visual")
	void UpdateMapTexturePerProvince(MapMode mode, FName provinceID, const FColor& newColor);
	/**
	* Update the array that holds the pixel data of a texture.
	*
	* @param pixelArray The array of pixels to update.
	* @param oldColor The old color to replace.
	* @param newColor The new color to set.
	* @param texture The texture that the array corresponds to, only passed to get its dimensions
	* @param provinceIDs The array of province IDs that should change color
	*/
	void UpdatePixelArray(TArray<uint8>& pixelArray, const FColor& oldColor, const FColor& newColor, const UTexture2D* texture, const TArray<FName>& provinceIDs);
	void SetPixelColor(int index, TArray<float>& pixelArray, uint8 R, uint8 G, uint8 B, uint8 A);
	void SetPixelColor(int index, TArray<float>& pixelArray, const FColor& color);
	void SetPixelColorInt(int index, TArray<uint8>& pixelArray, const FColor& color);


protected:
	// The root of the map.
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MapRoot;

	// The visual representation of the map 
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UMapVisualComponent> MapVisualComponent;

	// -------------------------VISUAL DATA--------------------------------------
	// The lookup texture.
	UPROPERTY(EditAnywhere, Category = "Map")
	TObjectPtr<UTexture2D> MapLookUpTexture;

	// The political map texture component.
	UPROPERTY(EditAnywhere, Category = "Texture", BlueprintReadOnly)
	TObjectPtr<UDynamicTextureComponent> PoliticalMapTextureComponent;

	// The religious map texture component.
	UPROPERTY(EditAnywhere, Category = "Texture", BlueprintReadOnly)
	TObjectPtr<UDynamicTextureComponent> ReligiousMapTextureComponent;

	// The culture map texture component.
	UPROPERTY(EditAnywhere, Category = "Texture", BlueprintReadOnly)
	TObjectPtr<UDynamicTextureComponent> CultureMapTextureComponent;

	// Material that shows the current gameplay map.
	// Political, religious, or terrain.
	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* GameplayMapMaterial;

	// Hold pixel data of the lookup texture.
	TArray<uint8> MapColorCodeTextureData;

	// Border Material 
	UPROPERTY(EditAnywhere, Category = "Border|Materials")
	UMaterialInterface* BorderMaterial;
	UPROPERTY(EditAnywhere, Category = "Border|Materials")
	UMaterialInterface* HQXFilterMaterial;
	UPROPERTY(EditAnywhere, Category = "Border")
	UTextureRenderTarget2D* BorderMaterialRenderTarget;
	UPROPERTY(EditAnywhere, Category = "Border")
	bool bUseBorderMesh = false;
	//------------------------------- Data -----------------------------------------
	// The map data component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<class UMapDataComponent> MapDataComponent;

	// The current map mode.
	UPROPERTY(BlueprintReadWrite, Category = "Map Mode")
	MapMode CurrentMapMode = MapMode::POLITICAL;



};
