// Copyright 2024 An@stacioDev All rights reserved.

#include "AssetActions/MapObjectUserUploadAction.h"
#include "EditorUtilityLibrary.h"
#include "MapDataValidation.h"
#include "MapEditor.h"
#include "MapObject.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "BlueprintLibrary/FilePickerFunctionLibrary.h"
#include "Materials/MaterialInstanceConstant.h"

#define LOCTEXT_NAMESPACE "CustomAssetActions"

#undef LOCTEXT_NAMESPACE
UMapObjectUserUploadAction::UMapObjectUserUploadAction()
{
	SupportedClasses.Add(UMapObject::StaticClass());
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Script/Engine.Material'/InteractiveMap/GSMap/Editor/M_PoliticalMapEditor.M_PoliticalMapEditor'"));
	if(MaterialFinder.Succeeded())
	{
		Material = MaterialFinder.Object;
	}
}

void UMapObjectUserUploadAction::UploadFiles(UScriptStruct* StructLandTiles, UScriptStruct* StructOceanTiles, UTexture2D* LookupTexture) const
{
	// Validate user params
	if (StructLandTiles == nullptr && StructOceanTiles == nullptr)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("No Structs selected"));
		return;
	}
	if (!LookupTexture)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("No Lookup Texture selected"));
		return;
	}
	
	// Load lookup table from file  and validate
	TArray<FString> FilePathsLookup;
	UAtkFilePickerFunctionLibrary::OpenFileDialogJson("Select Lookup Table Json", FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()), FilePathsLookup);
	if(FilePathsLookup.IsEmpty())
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("No file selected"));
		return;
	}
	const FString FilePathLookup = FPaths::CreateStandardFilename(FilePathsLookup[0]);
	bool bOutResult = false;
	const TArray<FLookupEntry> LookupEntries = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson<FLookupEntry>(FilePathLookup, bOutResult);
	if (!bOutResult)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to load lookup map from %s"), *FilePathLookup);
		return;
	}
	
	// Load map data from file and validate
	TArray<FString> FilePathsMapData;
	UAtkFilePickerFunctionLibrary::OpenFileDialogJson("Select Map Data Json", FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()), FilePathsMapData);
	if(FilePathsMapData.IsEmpty())
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("No file selected"));
		return;
	}
	const FString FilePath = FPaths::CreateStandardFilename(FilePathsMapData[0]);
	TArray<const UScriptStruct*> StructTypes {StructLandTiles, StructOceanTiles};
	const TArray<FInstancedStruct> NewMapData = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson(FilePath, StructTypes);
	if (!MapDataValidation::ValidateMapData(NewMapData.Num(), NewMapData, StructTypes, FilePath))
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to load map data from %s"), *FilePath);
		 return;
	}
	
	// All inputs are valid apply changes to selected assets
	const TArray<UObject *> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (UObject *Asset : SelectedAssets)
	{
		UMapObject* MapObject = Cast<UMapObject>(Asset);
		if (!MapObject)
			continue;
		
		MapObject->InitLandStructType(StructLandTiles);
		MapObject->InitOceanStructType(StructOceanTiles);
		
		MapObject->SetLookupFilePath(FilePathLookup, LookupEntries);
		
		// Load map data
		MapObject->SetFilePathMapData(FilePath);
		MapObject->SetMapData(NewMapData);
		
		MapObject->SetLookupTexture(LookupTexture);
		MapObject->ReconstructMapGenSavedFromData();
		
		if (bCreateMaterialForPreview)
		{
			UMaterialInstanceConstant* MaterialInstance = CreateMaterialInstance(LookupTexture, MapObject);
			MapObject->SetMaterialOverride(MaterialInstance);
		}
		
		MapObject->Modify();
	}
	
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
}

UMaterialInstanceConstant* UMapObjectUserUploadAction::CreateMaterialInstance(UTexture2D* Texture, UMapObject* MapObject) const
{
	const FString PackagePath = FPackageName::GetLongPackagePath(MapObject->GetPathName()) + "/";
	
	UObject* Asset = UAtkAssetCreatorFunctionLibrary::CreateAssetInPackageWithUniqueName(PackagePath, UMaterialInstanceConstant::StaticClass(), "MI_LookupMaterial");
	UMaterialInstanceConstant* MaterialInstance= Cast<UMaterialInstanceConstant>(Asset);
	MaterialInstance->SetParentEditorOnly(Material);
	MaterialInstance->SetTextureParameterValueEditorOnly(FName("DynamicTexture"), Texture);
	MaterialInstance->SetTextureParameterValueEditorOnly(FName("LookupTexture"), Texture);
	MaterialInstance->PostEditChange();
	return MaterialInstance;
}
