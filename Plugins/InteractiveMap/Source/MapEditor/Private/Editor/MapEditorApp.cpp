// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorApp.h" 

#include "FileHelpers.h"
#include "Editor/NameDefines.h"
#include "MapObject.h"
#include "Asset/SMapObjectViewport.h"
#include "BlueprintLibrary/MiscFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "Editor/MapEditorPreset.h"
#include "Editor/SMapTextureViewer.h"
#include "MapDataMode/MapEditorDataAppMode.h"
#include "MapEditorMode/MapEditorGenAppMode.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MapEditor.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "MapGeneratorWrapper.h"
#include "Asset/SCustomInstancedStructList.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"

FMapEditorApp::~FMapEditorApp()
{
	GEditor->UnregisterForUndo(this);
}

void FMapEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(tabManager);
	LoadPreviewTexturesFromMapMapObject(WorkingAsset);
}

void FMapEditorApp::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost,
	UObject* InObject)
{
	GEditor->RegisterForUndo(this);
	WorkingAsset = Cast<UMapObject>(InObject);
	MapGenPreset = NewObject<UMapEditorPreset>();

	AddApplicationMode(MapEditorGenModeName, MakeShareable(new FMapEditorGenAppMode(SharedThis(this))));
	AddApplicationMode(MapDataEditorModeName, MakeShareable(new FMapEditorDataAppMode(SharedThis(this))));
	
	TArray<UObject*> objectsToEdit{InObject};
	InitAssetEditor(
		Mode
		,InitToolkitHost
		,GetToolkitFName()
		,FTabManager::FLayout::NullLayout
		,true
		,true
		,objectsToEdit
		);
	AddToolbarExtender(); 
	SetCurrentMode(MapEditorGenModeName);
}

void FMapEditorApp::PostUndo(bool bSuccess)
{
	FEditorUndoClient::PostUndo(bSuccess);
	if(bSuccess)
	{
		LoadPreviewTexturesFromMapMapObject(WorkingAsset);
		TempMapGenerator = WorkingAsset->GetMapGen();
		MapGenPreset->MapEditorDetails = WorkingAsset->GetLastParamsUsed();
		RestoreTexturePreview();
	}
}

void FMapEditorApp::OnTexturePreviewClicked(FName ID) const
{
	if(GetCurrentMode() == MapEditorGenModeName)
	{
		TWeakObjectPtr<UTexture2D> Texture2D = nullptr;
		if(ID == FName("Lookup"))
		{
			Texture2D = GetLookupTexture();
		}
		else if(ID == FName("LookupLand"))
		{
			Texture2D = GetLookupLandTexture();
		}
		else if(ID == FName("LookupOcean"))
		{
			Texture2D = GetLookupOceanTexture();
		}
		else if(ID == FName("HeightMap"))
		{
			Texture2D = GetRootTexture();
		}
		else if(ID == FName("Borders"))
		{
			Texture2D = GetBorderTexture();
		}
		else if(ID == FName("Visited"))
		{
			Texture2D = GetVisitedTilesTexture();
		}
		
		if(Texture2D.IsValid())
		{
			MapViewport->UpdatePreviewActorMaterial(MapGenPreset->Material, Texture2D.Get());
		}
	}
}

void FMapEditorApp::SaveAsset_Execute()
{
	if(!WorkingAsset->IsMapSaved())
		SaveGeneratedMap();
	
	FWorkflowCentricApplication::SaveAsset_Execute();
}

void FMapEditorApp::GenerateMap()
{
	TempMapGenerator = MakeShareable(new MapGenerator::Map(1024, 1024));
	
	UTexture2D* Texture = MapGenPreset->MapEditorDetails.OriginTexture;
	const uint8* Data = UAtkTextureUtilsFunctionLibrary::ReadTextureToBuffer(Texture);
	if(!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read Height Map texture"));
		return;
	}
	 
	// TODO - improve this, adjust MapGeneratorLib
	const uint32 Height = Texture->GetSizeY();
	const uint32 Width = Texture->GetSizeX();
	const std::vector<uint8_t> vector = std::vector(&Data[0], &Data[Width * Height * 4]);
	UAtkMiscFunctionLibrary::ExecuteSlowTaskWithProgressBar(
		[&, vector, this](TFunction<void(float, std::string_view)> ProgressCallback)
		{
			//Call GenerateMap and pass the progress callback
			const MapGenerator::MapModeGen GenType = MapGenerator::MapModeGen::FromHeightMap;
			TempMapGenerator->GenerateMap(vector, Width, Height, MapGenPreset->GetLookupMapData(), GenType, ProgressCallback);

			// Update the preview textures
			PreviewLookupTexture = CreateLookupTexture(TempMapGenerator->GetLookupTileMap());
			PreviewLookupTextureLand = CreateTexture(TempMapGenerator->GetLookupTileMap().GetLandTileMap(), Width, Height);
			PreviewLookupTextureOcean = CreateTexture(TempMapGenerator->GetLookupTileMap().GetOceanTileMap(), Width, Height);
			PreviewBorderTexture = CreateTexture(TempMapGenerator->GetLookupTileMap().GetBordersTileMap(), Width, Height);
			PreviewVisitedTilesTexture = CreateTexture(TempMapGenerator->GetLookupTileMap().GetVisitedTileMap(), Width, Height);
			PreviewRootTexture = Texture;
			
			if(UMapObject* MapObject = GetWorkingAsset())
			{
				// Causes slowdown in map gen -> maybe consider custom revert method through UI and not hook into Post Undo
				const FScopedTransaction Transaction(NSLOCTEXT("MapEditor", "UndoEditMapGen", "Map Generated"));
				MapObject->Modify();
				MapObject->SetMapSaved(false);
				MapObject->MarkPackageDirty();
			}

			MapViewport->UpdatePreviewActorMaterial(MapGenPreset->Material, PreviewLookupTexture);
			
			RestoreTexturePreview();
		});
}

void FMapEditorApp::RestoreTexturePreview() const
{
	if(!WorkingAsset)
		return;
	if(MapTexturePreview)
	{
		MapTexturePreview->SetTextures(TArray
							{
								TPair<FName, UTexture2D*>(FName("Lookup"), GetLookupTexture().Get()),
								TPair<FName, UTexture2D*>(FName("LookupLand"), GetLookupLandTexture().Get()),
								TPair<FName, UTexture2D*>(FName("LookupOcean"), GetLookupOceanTexture().Get()),
								TPair<FName, UTexture2D*>(FName("Borders"), GetBorderTexture().Get()),
								TPair<FName, UTexture2D*>(FName("Visited"), GetVisitedTilesTexture().Get()),
								TPair<FName, UTexture2D*>(FName("HeightMap"), GetRootTexture().Get())
							});
	}

	if(WorkingAsset->IsMapSaved())	
		MapViewport->UpdatePreviewActorMaterial(MapGenPreset->Material, GetLookupTexture().Get());
}

void FMapEditorApp::RestoreMapGenPreset() const
{
	MapGenPreset->MapEditorDetails = WorkingAsset->GetLastParamsUsed();
	if(WorkingAsset->StructType)
		MapGenPreset->TileDataStructType = WorkingAsset->StructType;
	if(WorkingAsset->OceanStructType)
		MapGenPreset->OceanTileDataType = WorkingAsset->OceanStructType;
}

void FMapEditorApp::SaveGeneratedMap()
{
	FString AssetPath = WorkingAsset->GetPathName();
	FString FullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + AssetPath.Replace(TEXT("/Game/"), TEXT("")) + TEXT(".uasset"));
	FString DirPath = FPaths::GetPath(FullPath);
	FPaths::NormalizeDirectoryName(DirPath);
	if(!FPaths::IsUnderDirectory(DirPath, FPaths::ProjectContentDir()))
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Folder to save Map Object must be inside project content"));
		return;
	}
	
	const FString ProjectContentsDir = FPaths::CreateStandardFilename(FPaths::ProjectContentDir());
	FString CompleteDirPath =  FPaths::CreateStandardFilename(DirPath);
	
	if(!DirPath.RemoveFromStart(ProjectContentsDir))
	{
		return;
	}
	
	const FString PackagePath = "/Game/" + DirPath + "/";
	UTexture2D* TextureAsset = CreateLookupTextureAsset(PackagePath);
	if(!TextureAsset)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to create texture Asset"));
		return;
	}
	
	UMaterialInstanceConstant* Material = CreateMaterialInstanceAsset(TextureAsset, PackagePath);
	if(!Material)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to create material Asset"));
		// return;
	}

	CompleteDirPath = CompleteDirPath + "/" + TextureAsset->GetName();
	const FString LookupFilePath = CompleteDirPath + + "_lookup.json";
	OutputLookupJson(LookupFilePath);
	
	const FString StubMapDataFilePath = CompleteDirPath + "_MapDataStub.json";
	OutputStubMapDataJson(StubMapDataFilePath);
	// Outputs the params and the results of the Map Lookup Created
	const FString LookupGenResultsFilePath = CompleteDirPath + "_MapGenParamsResults.json";
	OutputLookupGenFile(LookupGenResultsFilePath);	
	
	SetMapObjectProperties(WorkingAsset, TextureAsset, LookupFilePath, StubMapDataFilePath, Material);

	WorkingAsset->SetLastParamsUsed(MapGenPreset->MapEditorDetails);
	WorkingAsset->SetRootTexture(PreviewRootTexture);
	WorkingAsset->SetMapGen(TempMapGenerator);
	WorkingAsset->SetMapSaved(true);
	
	// FString Message;
	UEditorLoadingAndSavingUtils::SavePackages({TextureAsset->GetPackage(), Material->GetPackage()}, true);
}

void FMapEditorApp::AddReferencedObjects(FReferenceCollector& Collector)
{
	if(MapGenPreset)
	{
		Collector.AddReferencedObject(MapGenPreset);
	}
	if(WorkingAsset)
	{
		Collector.AddReferencedObject(WorkingAsset);
	}
}

void FMapEditorApp::AddToolbarExtender()
{
	// Get the toolbar builder
	FToolMenuOwnerScoped OwnerScoped(this);
    
	// Create the main editor toolbar
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.MapEditorApp.Toolbar");
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Default");
        
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(
			"SetMapDataEditor",
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					SetCurrentMode(MapDataEditorModeName);
				}),
				FCanExecuteAction().CreateLambda([this]() -> bool
				{
					// return GetCurrentMode() != MapDataEditorModeName && WorkingAsset->IsMapSaved();
					return true;
				}),
				FIsActionChecked::CreateLambda([this]() -> bool
				{
					return GetCurrentMode() == MapDataEditorModeName;
				})
			),
			NSLOCTEXT("MapEditor", "MapDataTabLabel", "Details"),
			NSLOCTEXT("MapEditor", "MapDataTabTooltip", "Show the details tab"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details")
		));

		Section.AddEntry(FToolMenuEntry::InitToolBarButton(
			"SetMapGenEditor",
			FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					SetCurrentMode(MapEditorGenModeName);
				}),
				FCanExecuteAction().CreateLambda([this]() -> bool
				{
					return !IsModeCurrent(MapEditorGenModeName);
				}),
				FIsActionChecked::CreateLambda([this]() -> bool
				{
					return IsModeCurrent(MapEditorGenModeName);
				})
			),
			NSLOCTEXT("MapEditor", "MapGenTabLabel", "Map Lookup Gen"),
			NSLOCTEXT("MapEditor", "MapGenTabTooltip", "Show Map Lookup Generator"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details")
		));
	}
}

TObjectPtr<UTexture2D> FMapEditorApp::CreateLookupTexture(const MapGenerator::TileMap& TileMap)
{
	uint8_t* buffer = TileMap.ConvertTileMapToRawBuffer();
	const int Width = TileMap.Width();
	const int Height = TileMap.Height();
	TObjectPtr<UTexture2D> Texture = CreateTexture(buffer, Width, Height);
	// if(buffer)
	// {
	// 	delete buffer;
	// 	buffer = nullptr;
	// }
	return Texture;
}

TObjectPtr<UTexture2D> FMapEditorApp::CreateTexture(uint8* Buffer, unsigned Width, unsigned Height)
{
	if(!Buffer)
		return nullptr;

	// TODO - CORRECT THIS FUNCTION 
	//Create a string containing the texture's path
	FString PackageName = TEXT("/Game/Preview/");
	FString BaseTextureName = FString("PreviewTexture");
	PackageName += BaseTextureName;
 
	//Create the package that will store our texture
	UPackage* Package = CreatePackage(*PackageName);
	GLog->Log("project dir:" + FPaths::ProjectDir());
 
	//Create a unique name for our asset. For example, if a texture named ProcTexture already exists the editor
	//will name the new texture as "ProcTexture_1"
	FName TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*BaseTextureName), EUniqueObjectNameOptions::GloballyUnique);
	Package->FullyLoad();
 
	TObjectPtr<UTexture2D> NewTexture = NewObject<UTexture2D>(Package, TextureName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	
	// Set texture properties
	NewTexture->Source.Init(Width, Height, 1, 1, TSF_BGRA8);
	
	// Lock the mipmap and write the buffer data
	uint8* MipData = NewTexture->Source.LockMip(0);
	int32 BufferSize = Width * Height * 4; // BGRA8 format
	FMemory::Memcpy(MipData, Buffer, BufferSize);
	NewTexture->Source.UnlockMip(0);

	NewTexture->CompressionSettings = TC_EditorIcon;
	NewTexture->MipGenSettings = TMGS_NoMipmaps;
	
	// Update the texture to reflect changes
	NewTexture->PostEditChange();

	// TODO - CHANGE THIS This function should not manage the buffer memory
	//Since we don't need access to the pixel data anymore free the memory
	delete[] Buffer;
	Buffer = nullptr;
	
	return NewTexture;
}

TObjectPtr<UTexture2D> FMapEditorApp::CreateTextureSimple(uint8* Buffer, unsigned Width, unsigned Height)
{
	if(!Buffer)
		return nullptr;
	
	TObjectPtr<UTexture2D> NewTexture = NewObject<UTexture2D>();
	
	// Set texture properties
	NewTexture->Source.Init(Width, Height, 1, 1, TSF_BGRA8);
	// Lock the mipmap and write the buffer data
	uint8* MipData = NewTexture->Source.LockMip(0);
	int32 BufferSize = Width * Height * 4; // BGRA8 format
	FMemory::Memcpy(MipData, Buffer, BufferSize);
	NewTexture->Source.UnlockMip(0);

	NewTexture->CompressionSettings = TC_EditorIcon;
	NewTexture->MipGenSettings = TMGS_NoMipmaps;

	// TODO - CHANGE THIS This function should not manage the buffer memory
	//Since we don't need access to the pixel data anymore free the memory
	delete[] Buffer;
	Buffer = nullptr;
	
	return NewTexture;
}

void FMapEditorApp::SetMapObjectProperties(UMapObject* MapObject, UTexture2D* Texture, const FString& LookupFilePath,
                                           const FString& MapDataFilePath, UMaterialInstanceConstant* Material) const
{
	MapObject->SetLookupTexture(Texture);
	MapObject->SetLookupFilePath(LookupFilePath);
	if(Material)
	{
		MapObject->SetMaterialOverride(Material);
	}

	if(MapGenPreset && MapGenPreset->TileDataStructType && MapGenPreset->OceanTileDataType)
	{
		MapObject->StructType = MapGenPreset->TileDataStructType;
		MapObject->OceanStructType = MapGenPreset->OceanTileDataType;
		MapObject->SetMapDataFilePath(MapDataFilePath);
	}
	MapObject->PostEditChange();
}

void FMapEditorApp::OutputLookupJson(const FString& FilePath) const
{
	int32 Id = 0;
	TArray<FTileIdData> OutputData;
	TSharedPtr<MapGenerator::Map> Map = TempMapGenerator;
	OutputData.Reserve(Map->GetLookupTileMap().GetCellMap().size());
	for (const auto& Cell : Map->GetLookupTileMap().GetCellMap())
	{
		OutputData.Emplace(FTileIdData(Cell.second.ConvertToHex().c_str(), Id));
		Id++;
	}
	
	bool bResult = false;
	FString Message;
	UAtkDataManagerFunctionLibrary::WriteArrayToJsonFile(FilePath, OutputData, bResult, Message);
	UE_LOG(LogInteractiveMapEditor, Warning, TEXT("%s"), *Message);
}

void FMapEditorApp::OutputLookupGenFile(const FString& FilePath) const
{
	int32 OceanTiles = 0;
	int32 LandTiles = 0;
	const MapGenerator::TileMap& TileMap = TempMapGenerator->GetLookupTileMap();
	for (const auto& [Position, Color] : TileMap.GetCellMap())
	{
		if(TileMap.IsTileOfType(MapGenerator::TileType::LAND, Position.x, Position.y))
		{
			LandTiles++;
		}
		else if(TileMap.IsTileOfType(MapGenerator::TileType::WATER, Position.x, Position.y))
		{
			OceanTiles++;
		}
	}
	const FMapLookupGenFeedback GenInputsResults(MapGenPreset->MapEditorDetails, LandTiles, OceanTiles);
	bool bResult = false;
	FString Message;
	UAtkDataManagerFunctionLibrary::WriteStructToJsonFile(FilePath, GenInputsResults, bResult, Message);
	if(!bResult)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to Save map gen results %s"), *Message);
	}
}

void FMapEditorApp::OutputStubMapDataJson(const FString& FilePath) const
{
	TArray<FInstancedStruct> Output;
	TSharedPtr<MapGenerator::Map> Map = TempMapGenerator;
	const uint32 Size = Map->GetLookupTileMap().GetCellMap().size();
	Output.Reserve(Size);
	uint32 Index = 0;
	UE_LOG(LogInteractiveMapEditor, Warning, TEXT("CellMap size %d"), Size);
	
	const MapGenerator::TileMap& TileMap = Map->GetLookupTileMap();
	for (const auto& [Position, Color] : TileMap.GetCellMap())
	{
		FInstancedStruct Struct;
		// Create a struct dependent on the type of tile
		if(TileMap.IsTileOfType(MapGenerator::TileType::LAND, Position.x, Position.y))
		{
			Struct.InitializeAs(MapGenPreset->TileDataStructType);
		}
		else if(TileMap.IsTileOfType(MapGenerator::TileType::WATER, Position.x, Position.y))
		{
			Struct.InitializeAs(MapGenPreset->OceanTileDataType);
		}
		else
		{
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("Tile in LookupTileMap is undefined!!"));
			return;
		}
		
		if(!UAtkStructUtilsFunctionLibrary::SetPropertyValueInStruct(Struct, "ID", Index))
		{
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to create stub Map Data of type %s - ID is missing"),
					*MapGenPreset->TileDataStructType->GetName());
			return;
		}
		
		Output.Emplace(Struct);
		Index++;
	}
	
	UAtkDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePath, Output);
}

UMaterialInstanceConstant* FMapEditorApp::CreateMaterialInstanceAsset(UTexture2D* Texture,
	const FString& PackagePath) const
{
	if(MapGenPreset && MapGenPreset->Material)
	{
		UObject* Asset = UAtkAssetCreatorFunctionLibrary::CreateAssetInPackageWithUniqueName(PackagePath, UMaterialInstanceConstant::StaticClass(), "MI_LookupMaterial");
		UMaterialInstanceConstant* MaterialInstance= Cast<UMaterialInstanceConstant>(Asset);
		MaterialInstance->SetParentEditorOnly(MapGenPreset->Material);
		MaterialInstance->SetTextureParameterValueEditorOnly(FName("DynamicTexture"), Texture);
		MaterialInstance->SetTextureParameterValueEditorOnly(FName("LookupTexture"), Texture);
		MaterialInstance->PostEditChange();
		return MaterialInstance;
	}

	return nullptr;
}

UTexture2D* FMapEditorApp::CreateLookupTextureAsset(const FString& PackagePath) const
{
	TSharedPtr<MapGenerator::Map> Map = TempMapGenerator;
	uint8_t* Buffer = Map->GetLookupTileMap().ConvertTileMapToRawBuffer();
	
	FString Message;
	bool bResult = false;
	UTexture2D* Texture = UAtkAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(PackagePath, "LookupTexture", Buffer, Map->Width(), Map->Height(), bResult, Message);
	UE_LOG(LogInteractiveMapEditor, Warning, TEXT("%s"), *Message);
	
	if(Texture)
	{
		Texture->CompressionSettings = TC_EditorIcon;
		Texture->MipGenSettings = TMGS_NoMipmaps;
		Texture->PostEditChange();
	}
	
	if(Buffer)
	{
		delete[] Buffer;
		Buffer = nullptr;
	}
	
	return Texture;
}

void FMapEditorApp::LoadPreviewTexturesFromMapMapObject(const UMapObject* MapObject)
{
	if(!MapObject)
		return;

	PreviewLookupTexture = MapObject->LookupTexture;
	PreviewRootTexture = MapObject->GetRootTexture();
	
	TSharedPtr<MapGenerator::Map> MapGen = MapObject->GetMapGen();
	if(MapGen->IsValid())
	{
		const MapGenerator::TileMap& TileMap = MapGen->GetLookupTileMap();
		PreviewLookupTextureLand = CreateTexture(TileMap.GetLandTileMap(), TileMap.Width(), TileMap.Height());
		PreviewLookupTextureOcean = CreateTexture(TileMap.GetOceanTileMap(), TileMap.Width(), TileMap.Height());
		PreviewBorderTexture = CreateTexture(TileMap.GetBordersTileMap(), TileMap.Width(), TileMap.Height());
		PreviewVisitedTilesTexture = CreateTexture(TileMap.GetVisitedTileMap(), TileMap.Width(), TileMap.Height());
	}
}

TWeakObjectPtr<UTexture2D> FMapEditorApp::GetLookupTexture() const
{
	return PreviewLookupTexture;
}

TWeakObjectPtr<UTexture2D> FMapEditorApp::GetLookupLandTexture() const
{
	return PreviewLookupTextureLand;
}

TWeakObjectPtr<UTexture2D> FMapEditorApp::GetLookupOceanTexture() const
{
	return PreviewLookupTextureOcean;
}

TWeakObjectPtr<UTexture2D> FMapEditorApp::GetRootTexture() const
{
	return PreviewRootTexture;
}

TWeakObjectPtr<UTexture2D> FMapEditorApp::GetBorderTexture() const
{
	return PreviewBorderTexture;
}

TWeakObjectPtr<UTexture2D> FMapEditorApp::GetVisitedTilesTexture() const
{
	return PreviewVisitedTilesTexture;
}

void FMapEditorApp::UpdateEntrySelected(int32 Index) const
{
	FName ModeName = GetCurrentMode();
	if(ModeName == MapDataEditorModeName)
	{
		TSharedPtr<FMapEditorDataAppMode> DataEditorMode = StaticCastSharedPtr<FMapEditorDataAppMode>(GetCurrentModePtr());
		if(DataEditorMode.IsValid())
		{
			DataEditorMode->EditableStructListDisplay->SetSelection(Index);
		}
	}
}
