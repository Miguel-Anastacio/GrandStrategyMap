// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "MapObjectUserUploadAction.generated.h"

/**
 * 
 */
UCLASS()
class MAPEDITOR_API UMapObjectUserUploadAction : public UAssetActionUtility
{
	GENERATED_BODY()
	UMapObjectUserUploadAction();

public:	
	UFUNCTION(CallInEditor, Category = "Upload Map Files ")
	virtual void UploadFiles(
		UPARAM(meta = (AllowedClasses = "ScriptStruct"))
			UScriptStruct* StructLandTiles = nullptr,
		UPARAM(meta = (AllowedClasses = "ScriptStruct"))
			UScriptStruct* StructOceanTiles = nullptr,
		UPARAM(meta = (AllowedClasses = "Texture2D"))
			UTexture2D* LookupTexture = nullptr) const;
	
private:
	static void UploadLookupTexture(UTexture2D* Texture);
	static void UploadLookupTable();
	static void UploadMapDataFile();
	static void CreateMaterialInstance(UTexture2D* Texture);
	
};
