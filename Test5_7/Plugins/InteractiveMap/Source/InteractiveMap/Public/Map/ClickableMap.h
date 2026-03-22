// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 4, 4)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "GameFramework/Actor.h"
#include "VisualProperties.h"
#include "ClickableMap.generated.h"

class UDynamicTexture;
class UMapObject;
class UTextureRenderTarget2D;
class UDynamicTextureComponent;

USTRUCT(BlueprintType)
struct FTilePair
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	int ID = -1;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	FInstancedStruct Data;
};

USTRUCT()
struct FPositions
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FVector2D> PosArray;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapTileChangedSignature, const TArray<int> &, IDs);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapInitializedSignature, AClickableMap *, map);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMapModeChangedSignature, FName, oldMode, FName, newMode);
UCLASS(Abstract, NotBlueprintable)
class INTERACTIVEMAP_API AClickableMap : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AClickableMap(const FObjectInitializer &ObjectInitializer);

	UFUNCTION(BlueprintNativeEvent, Category = "Map")
	void InitializeMap();
	virtual void InitializeMap_Implementation();

	UFUNCTION(BlueprintCallable, Category= "Map")
	UMapObject* GetMapObject() const;
	
	//------------------------------- Data -----------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	TMap<FColor, int> GetLookUpTable() const;
	
	TMap<int, FInstancedStruct> *GetProvinceDataMap() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	int GetProvinceID(const FColor &Color, bool &bOutResult) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map Data")
	void GetProvinceData(const int ID, FInstancedStruct &OutData) const;

	FInstancedStruct *GetProvinceData(int ID) const;

	//------------------------------- Visual Data -----------------------------------

	UFUNCTION(BlueprintCallable, Category = "Map Visual Data")
	FColor GetColorFromLookUpTexture(const FVector2D &Uv) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Map Mode")
	void SetMapMode(const FName &Mode);
	virtual void SetMapMode_Implementation(const FName &Mode);

	//------------------------------- Visual ----------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Map Visual")
	virtual void UpdateProvinceHovered(const FColor &color);

	// UFUNCTION(CallInEditor, Category = "Map")
	void FillPixelMap();

public:
	// Delegate triggered when map initialization is finished
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
	FMapInitializedSignature MapInitializationDelegate;
	// Delegate triggered when map mode changes, provides the previous mode and the current mode
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
	FMapModeChangedSignature MapModeChangedDelegate;
	// Delegate triggered when one or more map tile data changes, provides the list of tiles that were changed
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Map")
	FMapTileChangedSignature MapTileChangedDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "Map Data")
	void SetMapTiles(const TArray<FTilePair> &NewData);

	UFUNCTION()
	void UpdateDynamicTextures(const TArray<int> &IDs);

	void LoadMapAsset(UMapObject *MapObject);

	void CreateMapModes();

	UFUNCTION(BlueprintCallable, Category = "Map Data")
	void UpdateTileData(const FInstancedStruct &Data, int ID) const;

	void CreateDynamicTextures(const TArray<TObjectPtr<UVisualProperty>>& VisualPropertyTypes);
	void FillDynamicTextures(const TArray<uint8> &LookupTextureData);

	void MarkPixelsToEdit(TArray<uint8> &PixelBuffer, const TArray<int> &IDs, uint8 MarkerValue) const;

	UFUNCTION(BlueprintCallable, Category = "Map Lookup")
	UTexture2D* GetLookupTexture() const;
	UFUNCTION(BlueprintCallable, Category = "Map Lookup")
	TArray<uint8> GetLookupTextureData() const;
protected:
	// The root of the map.
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MapRoot;

	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UMapObject> MapAsset;

	// The visual representation of the map
	UPROPERTY(Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UMapVisualComponent> MapVisualComponent;
	

	// -------------------------VISUAL DATA--------------------------------------
	UPROPERTY(EditAnywhere, Category = "Map|LookupTexture")
	FName StartMapMode = "Terrain";

	// The political map texture component.
	UPROPERTY(EditAnywhere, Category = "Texture", BlueprintReadOnly)
	TObjectPtr<UDynamicTextureComponent> DynamicTextureComponent;

	//------------------------------- Data -----------------------------------------
	// The map data component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<class UMapDataComponent> MapDataComponent;

	// The current map mode.
	UPROPERTY(BlueprintReadOnly, Category = "Map Mode")
	FName CurrentMapMode = "Country";

	UPROPERTY()
	TMap<FName, UDynamicTexture *> MapModeDynamicTextures;

	UPROPERTY()
	TMap<FColor, FPositions> PixelMap;
};
