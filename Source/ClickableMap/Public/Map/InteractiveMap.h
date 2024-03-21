// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapUtils.h"
#include "MapEnums.h"
#include "InteractiveMap.generated.h"

class UTextureRenderTarget2D;
class UDynamicTextureComponent;
UCLASS()
class CLICKABLEMAP_API AInteractiveMap : public AActor
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

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetProvinceID(const FVector& color) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetProvinceData(FName name, FProvinceData& out_data);

	UFUNCTION(BlueprintCallable)
	void SetMapMode(MapMode mode);

	FProvinceData* GetProvinceData(FName name);

	UFUNCTION(BlueprintCallable)
	void SetBorderVisibility(bool status);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	void SetPixelColor(int index, TArray<float>& pixelArray, uint8 R, uint8 G, uint8 B, uint8 A);
	void SetPixelColor(int index, TArray<float>& pixelArray, const FColor& color);
	bool GetCountryColor(const FVector& color, FColor& out_countryColor);
	FColor GetCountryColor(const FProvinceData* data);
	FColor GetReligionColor(const FProvinceData* data);
	FColor GetCultureColor(const FProvinceData* data);

	void SaveMapTextureData();
	void CreateLookUpTable();

	void ReadProvinceDataTable();
	void ReadCountryDataTable();

	void CreatePoliticalMapTexture();
	void CreateMapTexture(UDynamicTextureComponent* textureCompoment, const TArray<float>& pixelArray, UTexture2D* texture);
protected:
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MapSelectMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MapBorderMesh;
	UPROPERTY(EditAnywhere, Category = "Map", BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> GameplayMapMesh;

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

	// NOT IN USE
	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* TestPoliticalMapRenderTarget;

	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* MapDataTable;
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* ProvinceDataTable;
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* CountryDataTable;

	UPROPERTY(EditAnywhere, Category = "Map")
	TObjectPtr<UTexture2D> MapLookUpTexture;


	UPROPERTY()
	TObjectPtr<UTexture2D> PoliticalMapTexture;
	UPROPERTY()
	TObjectPtr<UTexture2D> ReligiousMapTexture;
	UPROPERTY()
	TObjectPtr<UTexture2D> CultureMapTexture;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTextureRenderTarget2D> MapRenderTarget;

	UPROPERTY(BlueprintReadOnly)
	TMap<FVector, FName> LookUpTable;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FProvinceData> ProvinceDataMap;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FCountryData> CountryData;
	
	TArray<FColor> MapColorCodeTextureData;

	TArray<float> PixelColorPoliticalTexture;
	TArray<float> PixelColorReligiousTexture;
	TArray<float> PixelColorCultureMapTexture;

};
