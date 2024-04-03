// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapUtils.h"
#include "MapEnums.h"
#include "InteractiveMap.generated.h"

class UTextureRenderTarget2D;
class UDynamicTextureComponent;

USTRUCT()
struct FLookUpTextureData
{
	GENERATED_BODY()

	TArray<float> PixelData;
	// useles for now and a waste of memory
	// keep around for now, might be useful eventually
	TMap<int32, FName> PixedIndexID;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMapDataChangedSignature, MapMode, mode, FName, provinceID);
UCLASS()
class INTERACTIVEMAP_API AInteractiveMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractiveMap();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTextureRenderTarget2D* GetMapRenderTarget() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TMap<FVector, FName> GetLookUpTable() const;

	//UFUNCTION(BlueprintCallable, BlueprintPure)
	TMap<FName, FProvinceData>* GetProvinceDataMap();
	TMap<FName, FCountryData>* GetCountryDataMap();
	TMap<FName, FColoredData>* GetVisualPropertiesDataMap();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetProvinceID(const FVector& color) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetProvinceData(FName name, FProvinceData& out_data) const;
	FProvinceData* GetProvinceData(FName name);

	UFUNCTION(BlueprintCallable)
	FColor GetColorFromLookUpTexture(FVector2D uv) const;

	//UFUNCTION(BlueprintCallable, BlueprintPure)
	//F GetProvinceID(const FVector& color) const;

	UFUNCTION(BlueprintNativeEvent)
	void SetMapMode(MapMode mode);
	
	void SetMapMode_Implementation(MapMode mode);

	// Get Actual name of thing from tag/id
	template<class T>
	const T* GetDataFromID(FName tag, const TMap<FName, T>& mapToSearch)
	{
		const T* data = mapToSearch.Find(tag);

		return data;
	}

	// Update Data
	UFUNCTION(BlueprintCallable)
	bool UpdateProvinceData(const FProvinceData& data, FName id);
	UFUNCTION(BlueprintCallable)
	bool UpdateCountryData(const FCountryData& data, FName id);
	bool UpdateCountryColor(const FLinearColor& color, FName id);


	// Update Visual
	UFUNCTION(BlueprintCallable)
	void UpdateProvinceHovered(const FColor& color);
	UFUNCTION(BlueprintCallable)
	void SetBorderVisibility(bool status);

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FMapDataChangedSignature MapDataChangedDelegate;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	void SetPixelColor(int index, TArray<float>& pixelArray, uint8 R, uint8 G, uint8 B, uint8 A);
	void SetPixelColor(int index, TArray<float>& pixelArray, const FColor& color);

	bool GetCountryColor(const FVector& color, FColor& out_countryColor) const;
	FColor GetCountryColor(const FProvinceData* data) const;

	FColor GetReligionColor(const FProvinceData* data) const;
	FColor GetCultureColor(const FProvinceData* data) const;

	FColor GetColorFromUV(UTexture2D* texture, FVector2D uv) const;

	void SaveMapTextureData();

	// DATA
	void CreateLookUpTable();
	void SetCountryProvinces();



	// Visual Data
	void CreateMapTexture(UDynamicTextureComponent* textureCompoment, UTexture2D* texture);

	UFUNCTION(BlueprintCallable)
	void UpdateMapTexturePerProvince(MapMode mode, FName provinceID, const FColor& newColor);

	//void UpdatePixelArray(TArray<float>& pixelArray, const FColor& oldColor, const FColor& newColor, const UTexture2D* texture, const TArray<FName>& provinceIDs);
	void UpdatePixelArray(TArray<uint8>& pixelArray, const FColor& oldColor, const FColor& newColor, const UTexture2D* texture, const TArray<FName>& provinceIDs);
	//void UpdatePixelArrayB(TArray<float>& pixelArray,const UTexture2D* texture, const TArray<FName>& provinceIDs, const FColor& oldColor);


protected:
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MapRoot;

	// VISUAL
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UMapVisualComponent> MapVisualComponent;

	// -------------------------VISUAL DATA--------------------------------------
	UPROPERTY(EditAnywhere, Category = "Map")
	TObjectPtr<UTexture2D> MapLookUpTexture;

	UPROPERTY(EditAnywhere, Category = "Texture", BlueprintReadOnly)
	TObjectPtr<UDynamicTextureComponent> PoliticalMapTextureComponent;	
	UPROPERTY(EditAnywhere, Category = "Texture", BlueprintReadOnly)
	TObjectPtr<UDynamicTextureComponent> ReligiousMapTextureComponent;
	UPROPERTY(EditAnywhere, Category = "Texture", BlueprintReadOnly)
	TObjectPtr<UDynamicTextureComponent> CultureMapTextureComponent;
	// Material that show the current gameplay map
	// Political religious or terrain
	UPROPERTY(EditAnywhere)
	UMaterialInterface* GameplayMapMaterial;


	// Hold pixel data of the lookup texture
	FLookUpTextureData MapColorCodeTextureData;

	//// Hold pixel data of the political map texture
	//TArray<float> PixelColorPoliticalTexture;
	//// Hold pixel data of the religious map texture
	//TArray<float> PixelColorReligiousTexture;
	//// Hold pixel data of the culture map texture
	//TArray<float> PixelColorCultureMapTexture;

	// Border Material 
	UPROPERTY(EditAnywhere)
	UMaterialInterface* BorderMaterial;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* HQXFilterMaterial;
	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* BorderMaterialRenderTarget;
	// -----------------------------------------------------------------------------

	//------------------------------- Data -----------------------------------------
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* MapDataTable;
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* ProvinceDataTable;
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* CountryDataTable;
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* VisualPropertiesDataTable;
	UPROPERTY(BlueprintReadOnly)
	TMap<FVector, FName> LookUpTable;
	// Data Populated by the data tables
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FProvinceData> ProvinceDataMap;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FCountryData> CountryData;

	// Populated when reading the ProvinceDataTable
	// Not in USE
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FColor> Religions;
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FColor> Cultures;

	// Province properties that have a map color associated
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FColoredData> VisualPropertiesDataMap;

	//------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadWrite)
	MapMode CurrentMapMode = MapMode::POLITICAL;

	// ----------------------------------------- Useless ---------------------------
	UPROPERTY()
	UTexture2D* PoliticalMapTexture;
	UPROPERTY()
	UTexture2D* ReligiousMapTexture;
	UPROPERTY()
	UTexture2D* CultureMapTexture;

	// IN USE but Obsolete
	// removed at final cleanup
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextureRenderTarget2D> MapRenderTarget;



};
