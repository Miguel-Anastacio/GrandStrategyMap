// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "GameFramework/Actor.h"
#include "MapUtils.h"
#include "MapEnums.h"
#include "ClickableMap.generated.h"

struct FArrayOfVisualProperties;
class UDynamicTexture;
class UMapObject;
class UTextureRenderTarget2D;
class UDynamicTextureComponent;

USTRUCT()
struct FPositions
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FVector2D> PosArray;
	// UPROPERTY()
	// FColor PixelColor;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapDataChangedSignature, MapMode, mode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMapTileChangeSignature, int, Id, const FInstancedStruct&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapInitializedSignature, AClickableMap*, map);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMapModeChangedSignature, MapMode, oldMode, MapMode, newMode);
UCLASS(Abstract, NotBlueprintable)
class INTERACTIVEMAP_API AClickableMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClickableMap(const FObjectInitializer& ObjectInitializer);
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Map")
	void InitializeMap();
	virtual void InitializeMap_Implementation();

	virtual void Tick(float DeltaTime) override;

	//------------------------------- Data -----------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	TMap<FColor, int> GetLookUpTable() const;

	TMap<int, FInstancedStruct>* GetProvinceDataMap() const;
	TMap<FName, FCountryData>* GetCountryDataMap(); 
	TMap<FName, FColoredData>* GetVisualPropertiesDataMap();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	int GetProvinceID(const FColor& Color, bool& bOutResult) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	void GetProvinceData(int ID, FInstancedStruct& OutData) const;
	
	FInstancedStruct* GetProvinceData(int ID);


	UFUNCTION(BlueprintCallable, Category = "Map Data")
	bool UpdateProvinceData(const FInstancedStruct& Data, int ID);
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
	void SetMapMode(const FName& Mode);
	virtual void SetMapMode_Implementation(const FName& Mode);

	void SetMeshMaterial(const FName& Mode, UStaticMeshComponent* Mesh);

	// Update Visual Data
	UFUNCTION(BlueprintCallable, Category = "Map Visual Data")
	bool UpdateCountryColor(const FLinearColor& color, FName id);

	UFUNCTION(BlueprintCallable, Category = "Map Border")
	void UpdateBorder(UMaterialInstanceDynamic* material, UTextureRenderTarget2D* renderTarget);
	/**
	* Set the a dynamic texture as the lookuptexture of the border material
	*
	* @param borderMat dynamic instance of the Border Material
	* @param textureComponent the dynamic texture component that owns the texture to use as lookup
	*		 by default pass null and use the MapLookUpTexture
	*/
	UFUNCTION(BlueprintCallable, Category = "Map Border")
	void SetBorderLookUpTexture(UMaterialInstanceDynamic* bordeMat, UDynamicTextureComponent* textureComponent = nullptr);

	//------------------------------- Visual ----------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Map Visual")
	virtual void UpdateProvinceHovered(const FColor& color);
	UFUNCTION(BlueprintCallable, Category = "Map Visual")
	void SetBorderVisibility(bool status);

	UFUNCTION(CallInEditor, Category="Map")
	void FillPixelMap();

public:
	// Delegate triggered when map data changes impacts the visual representation of the map
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
	FMapDataChangedSignature MapDataChangedDelegate;

	// Delegate triggered when map initialization is finished
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
	FMapInitializedSignature MapInitializationDelegate;
	// Delegate triggered when map initialization is finished
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
	FMapModeChangedSignature MapModeChangedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
	FMapTileChangeSignature MapTileChangeDelegate;
	
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnMapTileChanged(int ID, const FInstancedStruct& Data);

	void LoadMapAsset(UMapObject* MapObject);
	
	/** Reads the look up texture and populates the dynamic textures data with the right pixel colors based on the ProvinceDataMap (MapDataComponent)*/
	void SaveMapTextureData();
	void CreateMapTexture(UDynamicTextureComponent* textureCompoment);


	UFUNCTION(BlueprintCallable, Category = "Map Visual Data")
	void RefreshDynamicTextureDataBuffer(UDynamicTextureComponent* textureCompoment, MapMode mode);
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
	* Update map texture completely
	* reads the look up texture data buffer
	* and updates the data buffer of one of the texture dynamic components
	* @param mode The map mode.
	*/
	UFUNCTION(BlueprintCallable, Category = "Map Visual")
	void UpdateMapTexture(MapMode mode);
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
	void SetPixelColorInt(int index, TArray<uint8>& pixelArray, const FColor& color);

	UFUNCTION(CallInEditor, BlueprintCallable)
	void CreateMapModes();
// #if WITH_EDITOR
	
	void CreateDynamicTextures(const TArray<FVisualPropertyType>& VisualPropertyTypes);
	void FillDynamicTextures(const TMap<FName, FArrayOfVisualProperties>& VisualProperties, const TArray<uint8>& LookupTextureData);
// #endif
protected:
	// The root of the map.
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MapRoot;
	
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UMapObject> MapAsset;
	
	// The visual representation of the map 
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UMapVisualComponent> MapVisualComponent;

	// Terrain
	UPROPERTY(EditAnywhere, Category = "Map|Terrain", BlueprintReadOnly)
	TObjectPtr<class UTexture2D> TerrainTexture;

	UPROPERTY(EditAnywhere, Category = "Map|Terrain")
	UMaterialInterface* TerrainMapMaterial;

	UPROPERTY(BlueprintReadWrite, Category = "Map|Terrain|Material")
	UMaterialInstanceDynamic* TerrainDynamicMaterial;

	// -------------------------VISUAL DATA--------------------------------------
	// The lookup texture.
	UPROPERTY(EditAnywhere, Category = "Map|LookupTexture")
	TObjectPtr<UTexture2D> MapLookUpTexture;

	// The political map texture component.
	UPROPERTY(EditAnywhere, Category = "Texture", BlueprintReadOnly)
	TObjectPtr<UDynamicTextureComponent> DynamicTextureComponent;

	// Material that shows the current gameplay map.
	// Political, religious, or terrain.
	UPROPERTY(EditAnywhere, Category = "Map|GameplayMap")
	UMaterialInterface* GameplayMapMaterial;

	// Hold pixel data of the lookup texture.
	TArray<uint8> MapColorCodeTextureData;

	// Border Material 
	UPROPERTY(EditAnywhere, Category = "Map|Border")
	UMaterialInterface* BorderMaterial;
	//UPROPERTY(EditAnywhere, Category = "Border|Materials")
	//UMaterialInterface* HQXFilterMaterial;

	UPROPERTY(BlueprintReadWrite, Category = "Map|Border|Material")
	UMaterialInstanceDynamic* BorderDynamicMaterial;
	UPROPERTY(BlueprintReadWrite, Category = "Border")
	UTextureRenderTarget2D* BorderMaterialRenderTarget;
	UPROPERTY(EditAnywhere, Category = "Map|Border")
	bool bUseBorderMesh = false;

	//------------------------------- Data -----------------------------------------
	// The map data component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<class UMapDataComponent> MapDataComponent;

	// The current map mode.
	UPROPERTY(BlueprintReadWrite, Category = "Map Mode")
	FName CurrentMapMode = "Country";

	// TODO: Create Map Modes and Dynamic Texture Components based on the visual properties available
	// TMap<FName, UDynamicTextureComponent> DynamicTextureComponents;
	// void CreateDynamicTextureComponents();
	UPROPERTY()
	TMap<FName, UDynamicTexture*> MapModeDynamicTextures;
	
	UPROPERTY()
	TMap<FColor, FPositions> PixelMap;
	


};
