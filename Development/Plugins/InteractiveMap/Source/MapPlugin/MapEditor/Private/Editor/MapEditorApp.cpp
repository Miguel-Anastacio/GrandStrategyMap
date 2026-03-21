// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorApp.h" 

#include "FileHelpers.h"
#include "Editor/NameDefines.h"
#include "MapObject.h"
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
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "BlueprintLibrary/FilePickerFunctionLibrary.h"
#include "MapDataValidation.h"

FMapEditorApp::~FMapEditorApp()
{
	GEditor->UnregisterForUndo(this);
}

void FMapEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(tabManager);
	LoadPreviewTexturesFromMapObject(WorkingAsset);
	UpdateCurrentTexture(GetLookupTexture());
	TempMapGenerator = WorkingAsset->GetLastMapGen();
	WorkingAsset->ClearTilesSelected();
}

void FMapEditorApp::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost,
	UObject* InObject)
{
	GEditor->RegisterForUndo(this);
	WorkingAsset = Cast<UMapObject>(InObject);
	MapGenPreset = NewObject<UMapEditorPreset>();
	
	WorkingAsset->OnTilesSelectedChanged.BindLambda([this](const TArray<int32>& tiles)
	{
		UpdateHighlightTexture(tiles);
		UpdateEntriesSelected(tiles);
	});
	
	WorkingAsset->OnMapDataChanged.BindLambda([this]()
	{
		RefreshMapDataEditor(true);
	});
	
	AddApplicationMode(MapEditorGenModeName, MakeShareable(new FMapEditorGenAppMode(SharedThis(this))));
	AddApplicationMode(MapDataEditorModeName, MakeShareable(new FMapEditorDataAppMode(SharedThis(this))));
	
	const TArray<UObject*> objectsToEdit{InObject};
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
		if(!TempPreviews.IsEmpty())
		{
			TempPreviews.Pop();
		}
		TempMapGenerator = GetLastMapCreated();
		if(TempMapGenerator)
		{
			UpdatePreviewTextures(TempMapGenerator->GetLookupTileMap());
		}
		PreviewRootTexture = GetLastRootTexture();
		UpdateCurrentTexture(GetLookupTexture());
		RestoreTexturePreview();

		RefreshMapDataEditor(true);
	}
}

void FMapEditorApp::OnTexturePreviewClicked(FName ID)
{
	TWeakObjectPtr<UTexture2D> Texture2D = nullptr;
	if(ID == FName("Lookup"))
	{
		SetFilterForDataList(nullptr);
		Texture2D = GetLookupTexture();
	}
	else if(ID == FName("LookupLand"))
	{
		SetFilterForDataList(WorkingAsset->StructType);
		Texture2D = GetLookupLandTexture();
	}
	else if(ID == FName("LookupOcean"))
	{
		SetFilterForDataList(WorkingAsset->OceanStructType);
		Texture2D = GetLookupOceanTexture();
	}
	else if(ID == FName("HeightMap"))
	{
		SetFilterForDataList(nullptr);
		Texture2D = GetHighlightTexture();
	}
	else if(ID == FName("Borders"))
	{
		SetFilterForDataList(nullptr);
		Texture2D = GetBorderTexture();
	}
	else if(ID == FName("Visited"))
	{
		SetFilterForDataList(nullptr);
		Texture2D = GetVisitedTilesTexture();
	}
		
	if(Texture2D.IsValid())
	{
		UpdateCurrentTexture(Texture2D);
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
	bool UserProvidedBorders = false;
	// Validate user uploaded borders
	if(MapGenPreset->UploadBorder())
	{
		// validate size
		if(!MapGenPreset->BorderMatchesOriginTexture())
		{
			// throw error and return
			const FText Title = FText::FromString(TEXT("Error - Texture Size does not match"));
			const FText Message = FText::FromString(TEXT("Border Texture size must match Origin Texture"));
			EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
			return;
		}
		UserProvidedBorders = true;
	}
	
	// Convert to filesystem path
	const FString CompleteDirPath = GetAbsoluteDir(WorkingAsset);
	const std::filesystem::path AssetDir(TCHAR_TO_UTF8(*CompleteDirPath));
	const std::filesystem::path MapGenLogPath = AssetDir / "MapGenLog.txt";
	
	TempMapGenerator = MakeShareable(new MapGenerator::Map(1024, 1024, MapGenLogPath));

	// Handle user uploaded borders
	if(UserProvidedBorders)
	{
		UTexture2D* BorderTexture = MapGenPreset->MapEditorDetails.BorderTexture;
		const std::vector<uint8> BufferData =  UAtkTextureUtilsFunctionLibrary::ReadTextureToVector(BorderTexture);
		if(BufferData.empty())
		{
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to read  Border texture"));
			// throw error and return
			const FText Title = FText::FromString(TEXT("Error - Border Texture Settings"));
			const FText Message = FText::FromString(TEXT("Check the Log for more details on the error"));
			EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
			return;
		}
		TempMapGenerator->SetBorderUpload(BufferData);
	}

	// Check if origin texture is valid
	UTexture2D* Texture = MapGenPreset->MapEditorDetails.OriginTexture;
	std::vector<uint8> Data = UAtkTextureUtilsFunctionLibrary::ReadTextureToVector(Texture);
	if(Data.empty())
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to read Height Map texture"));
		// throw error and return
		const FText Title = FText::FromString(TEXT("Error - Height Map Texture Settings"));
		const FText Message = FText::FromString(TEXT("Check the Log for more details on the error"));
		EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
		return;
	}
	 
	const uint32 Height = Texture->GetSizeY();
	const uint32 Width = Texture->GetSizeX();
	UAtkMiscFunctionLibrary::ExecuteSlowTaskWithProgressBar(
		[&, Data, this](TFunction<void(float, std::string_view)> ProgressCallback)
		{
			//Call GenerateMap and pass the progress callback
			constexpr MapGenerator::MapModeGen GenType = MapGenerator::MapModeGen::FromHeightMap;
			TempMapGenerator->SetProgressCallback(ProgressCallback);
			TempMapGenerator->GenerateMap(Data, Width, Height, MapGenPreset->GetLookupMapData(), GenType);
			WorkingAsset->SetLastMapGen(TempMapGenerator);

			// Update the preview textures
			UpdatePreviewTextures(TempMapGenerator->GetLookupTileMap());
			PreviewRootTexture = Texture;
			
			if(UMapObject* MapObject = GetWorkingAsset())
			{
				// Causes slowdown in map gen -> maybe consider custom revert method through UI and not hook into Post Undo
				const FScopedTransaction Transaction(NSLOCTEXT("MapEditor", "UndoEditMapGen", "Map Generated"));
				MapObject->Modify();
				MapObject->SetMapSaved(false);
				MapObject->IncrementCounter();
				MapObject->MarkPackageDirty();
			}
			
			TempPreviews.Emplace(TempMapGenerator, PreviewLookupTexture);
			
			UpdateCurrentTexture(GetLookupTexture());
			ClearHighlightTexture();
			RestoreTexturePreview();
		});
}

void FMapEditorApp::RestoreTexturePreview() const
{
	if(MapTexturePreview)
	{
		MapTexturePreview->SetTextures(GetTexturesPairs());
	}
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
	TempPreviews.Empty();
	// Convert filesystem path back to package path for asset creation
	const FString PackagePath = FPackageName::GetLongPackagePath(WorkingAsset->GetPathName()) + "/";
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
	
	// Get the directory containing the asset
	const FString CompleteDirPath = GetAbsoluteDir(WorkingAsset) + "/" + TextureAsset->GetName();
	
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
					return !IsModeCurrent(MapDataEditorModeName) && WorkingAsset->IsMapSaved();
				}),
				FIsActionChecked::CreateLambda([this]() -> bool
				{
					return IsModeCurrent(MapDataEditorModeName);
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
	const std::vector<uint8_t> buffer = TileMap.ConvertTileMapToBuffer();
	const int Width = TileMap.Width();
	const int Height = TileMap.Height();
	TObjectPtr<UTexture2D> Texture = CreateTexture(buffer, Width, Height);
	return Texture;
}

TObjectPtr<UTexture2D> FMapEditorApp::CreateTexture(const std::vector<uint8>& Buffer, unsigned Width, unsigned Height)
{
	//Create a string containing the texture's path
	FString PackageName = TEXT("/Game/Preview/");
	FString BaseTextureName = FString("PreviewTexture");
	PackageName += BaseTextureName;
 
	//Create the package that will store our texture
	UPackage* Package = CreatePackage(*PackageName);
 
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
	FMemory::Memcpy(MipData, Buffer.data(), BufferSize);
	NewTexture->Source.UnlockMip(0);

	NewTexture->CompressionSettings = TC_EditorIcon;
	NewTexture->MipGenSettings = TMGS_NoMipmaps;
	
	// Update the texture to reflect changes
	NewTexture->PostEditChange();

	return NewTexture;
}

void FMapEditorApp::SetMapObjectProperties(UMapObject* MapObject, UTexture2D* Texture, const FString& LookupFilePath,
                                           const FString& MapDataFilePath, UMaterialInstanceConstant* Material) const
{
	MapObject->SetLookupTexture(Texture);
	MapObject->SetLookupFilePathAndLoad(LookupFilePath);
	if(Material)
	{
		MapObject->SetMaterialOverride(Material);
	}

	if(MapGenPreset && MapGenPreset->TileDataStructType && MapGenPreset->OceanTileDataType)
	{
		MapObject->InitLandStructType(MapGenPreset->TileDataStructType);
		MapObject->InitOceanStructType(MapGenPreset->OceanTileDataType);
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
	const TSharedPtr<MapGenerator::Map> Map = TempMapGenerator;
	const uint32 Size = Map->GetLookupTileMap().GetCellMap().size();
	Output.Reserve(Size);
	uint32 Index = 0;
	
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
	std::vector<uint8_t> Buffer = Map->GetLookupTileMap().ConvertTileMapToBuffer();
	
	FString Message;
	bool bResult = false;
	UTexture2D* Texture = UAtkAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(PackagePath, "LookupTexture", Buffer.data(), Map->Width(), Map->Height(), bResult, Message);
	UE_LOG(LogInteractiveMapEditor, Warning, TEXT("%s"), *Message);
	
	if(Texture)
	{
		Texture->CompressionSettings = TC_EditorIcon;
		Texture->MipGenSettings = TMGS_NoMipmaps;
		Texture->PostEditChange();
	}
	
	return Texture;
}

void FMapEditorApp::SetFilterForDataList(const UScriptStruct* Struct)
{
	DataListStructFilter = Struct;
	TSharedPtr<FMapEditorDataAppMode> AppMode = StaticCastSharedPtr<FMapEditorDataAppMode>(GetCurrentModePtr());
	if(AppMode)
	{
		AppMode->SetFilter(Struct, IsModeCurrent(MapDataEditorModeName));
		
		const TArray<int32> TilesSelected = GetWorkingAsset()->GetTilesSelected();
		if(!TilesSelected.IsEmpty())
		{
			UpdateEntriesSelected(TilesSelected);	
		}
	}
}

TWeakObjectPtr<UTexture2D> FMapEditorApp::GetHighlightTexture() const
{
	return HighlightTexture;
}

void FMapEditorApp::UpdateHighlightTexture(const TArray<int32>& IDs)
{
	if(!CurrentTexture.IsValid())
		return;
	
	if(IDs.IsEmpty())
	{
		ClearHighlightTexture();
		return;
	}
	
	const int32 size = BufferCurrentTextureData.Num();
	std::vector<uint8_t> buffer(size);

	TSet<FColor> Colors;
	Colors.Reserve(IDs.Num());
	for(const int32 ID : IDs)
	{
		Colors.Add(GetWorkingAsset()->GetColor(ID));
	}
	
	const int32 NumPixels = size / 4;
	for(int32 PixelIndex = 0; PixelIndex < NumPixels; ++PixelIndex)
	{
		const int32 ByteIndex = PixelIndex * 4;
		const FColor PixelColor = UAtkTextureUtilsFunctionLibrary::GetColorFromIndex(ByteIndex, BufferCurrentTextureData);
    
		const uint8 Value = Colors.Contains(PixelColor) ? 255 : 0;
		buffer[ByteIndex + 0] = Value;
		buffer[ByteIndex + 1] = Value;
		buffer[ByteIndex + 2] = Value;
		buffer[ByteIndex + 3] = Value;
	}
	const int32 Width =  CurrentTexture.Get()->GetSizeX();
	const int32 Height = CurrentTexture.Get()->GetSizeY();

	if(!HighlightTexture)
	{
		HighlightTexture = CreateTexture(buffer, Width, Height);
	}
	else
	{
		UpdateTextureDataEditor(HighlightTexture, buffer.data(), Width, Height);
	}
	OnHighlightChanged.Broadcast(HighlightTexture);
}

void FMapEditorApp::LoadPreviewTexturesFromMapObject(const UMapObject* MapObject)
{
	if(!MapObject)
		return;

	PreviewRootTexture = MapObject->GetRootTexture();
	
	const TSharedPtr<MapGenerator::Map> LastMapGen = MapObject->GetLastMapGen();
	const TSharedPtr<MapGenerator::Map> MapGen = MapObject->GetMapGen();
	if(LastMapGen.IsValid())
	{
		const MapGenerator::TileMap& TileMap = LastMapGen->GetLookupTileMap();
		UpdatePreviewTextures(TileMap);
	}
	else if(MapGen.IsValid() && MapGen->IsValid())
	{
		const MapGenerator::TileMap& TileMap = MapGen->GetLookupTileMap();
		UpdatePreviewTextures(TileMap);
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

TWeakObjectPtr<UTexture2D> FMapEditorApp::GetCurrentTexture() const
{
	return CurrentTexture;
}

void FMapEditorApp::UpdateCurrentTexture(const TWeakObjectPtr<UTexture2D> Texture)
{
	CurrentTexture = Texture;
	BufferCurrentTextureData = UAtkTextureUtilsFunctionLibrary::ReadTextureToArray(CurrentTexture.Get());
	OnCurrentTextureChanged.Broadcast(Texture);
}

void FMapEditorApp::ClearHighlightTexture()
{
	if(!CurrentTexture.IsValid())
		return;
	const int32 Width =  CurrentTexture.Get()->GetSizeX();
	const int32 Height = CurrentTexture.Get()->GetSizeY();
	const int32 size = Width * Height * 4;
	std::vector<uint8_t> buffer(size);
	for(int i = 0; i < size; i+=4)
	{
		buffer[i + 0] = 0;
		buffer[i + 1] = 0;
		buffer[i + 2] = 0;
		buffer[i + 3] = 0;
	}

	if(!HighlightTexture)
	{
		HighlightTexture = CreateTexture(buffer, Width, Height);
	}
	else
	{
		UpdateTextureDataEditor(HighlightTexture, buffer.data(), Width, Height);
	}
	OnHighlightChanged.Broadcast(HighlightTexture);
}

TArray<TPair<FName, UTexture2D*>> FMapEditorApp::GetTexturesPairs() const
{
	return TArray
	{
		TPair<FName, UTexture2D*>(FName("Lookup"), GetLookupTexture().Get()),
		TPair<FName, UTexture2D*>(FName("LookupLand"), GetLookupLandTexture().Get()),
		TPair<FName, UTexture2D*>(FName("LookupOcean"), GetLookupOceanTexture().Get()),
		TPair<FName, UTexture2D*>(FName("Borders"), GetBorderTexture().Get()),
		TPair<FName, UTexture2D*>(FName("Visited"), GetVisitedTilesTexture().Get()),
		TPair<FName, UTexture2D*>(FName("HeightMap"), GetHighlightTexture().Get())
	};
}

void FMapEditorApp::UpdatePreviewTextures(const MapGenerator::TileMap& TileMap)
{
	const unsigned width = TileMap.Width();
	const unsigned height = TileMap.Height();
	// Maybe do not always create texture, do so only if it is null
	PreviewLookupTextureLand   = CreateTexture(TileMap.GetLandTileMap(), width, height);
	PreviewLookupTextureOcean  = CreateTexture(TileMap.GetOceanTileMap(), width, height);
	PreviewLookupTexture	   = CreateTexture(TileMap.ConvertTileMapToBuffer(), width, height);
	// Debug
	PreviewVisitedTilesTexture = CreateTexture(TileMap.GetCentroidTileMap(), width, height);
	PreviewBorderTexture	   = CreateTexture(TileMap.GetBordersTileMap(), width, height);
}

TSharedPtr<MapGenerator::Map> FMapEditorApp::GetLastMapCreated() const
{
	if(TempPreviews.IsEmpty() && WorkingAsset)
	{
		return WorkingAsset->GetMapGen();
	}
	return TempPreviews.Last().Map;
}

UTexture2D* FMapEditorApp::GetLastRootTexture() const
{
	if(TempPreviews.IsEmpty() && WorkingAsset)
	{
		return WorkingAsset->GetRootTexture();
	}
	return TempPreviews.Last().PreviewRootTexture;
}

void FMapEditorApp::UpdateEntriesSelected(const TArray<int32>& Indexes) const
{
	if(IsModeCurrent(MapDataEditorModeName))
	{
		TSharedPtr<FMapEditorDataAppMode> DataEditorMode = StaticCastSharedPtr<FMapEditorDataAppMode>(GetCurrentModePtr());
		if(DataEditorMode.IsValid())
		{
			DataEditorMode->UpdateEntriesSelected(Indexes);
		}
	}
}

const UScriptStruct* FMapEditorApp::GetFilterForDataList() const
{
	return DataListStructFilter;
}

void FMapEditorApp::UpdateMapData(const FInstancedStruct& Data) const
{
	{
		const FScopedTransaction Transaction(NSLOCTEXT("MapDataEditor", "UndoEditMapData", "Map Data Changed"));
		WorkingAsset->Modify();
		WorkingAsset->UpdateDataInEditor(Data, GetWorkingAsset()->GetTilesSelected());
		WorkingAsset->IncrementCounter();
		WorkingAsset->MarkPackageDirty();
	}
}

void FMapEditorApp::RefreshMapDataEditor(const bool keepSelection) const
{
	if(IsModeCurrent(MapDataEditorModeName))
	{
		TSharedPtr<FMapEditorDataAppMode> AppMode = StaticCastSharedPtr<FMapEditorDataAppMode>(GetCurrentModePtr());
		if(AppMode)
		{
			AppMode->Refresh(keepSelection);
		}
	}
}

void FMapEditorApp::LoadMapDataFromFile() const
{
	TArray<FString> FilePaths;
	const FString Path = WorkingAsset->GetPathName();
	UAtkFilePickerFunctionLibrary::OpenFileDialogJson("Select Map Data Json", Path, FilePaths);
	if(FilePaths.IsEmpty())
		return;
	
	const FString FilePath = FPaths::CreateStandardFilename(FilePaths[0]);

	TArray<const UScriptStruct*> StructTypes {WorkingAsset->StructType, WorkingAsset->OceanStructType};
	// Validate StructTypes are not null
	for(const auto& Struct : StructTypes)
	{
		if(!Struct)
		{
            const FText Message = FText::FromString(TEXT("Please define struct types first"));
			MapDataValidation::ErrorOnLoadMapDataFilePath(Message);
			return;
		}
	}
	// Read file
	const TArray<FInstancedStruct> NewMapData = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson(FilePath, StructTypes);
	if (!MapDataValidation::ValidateMapData(WorkingAsset->GetLookupTable().Num(), NewMapData, StructTypes, FilePath))
	{
		return;
	}
	
	// Present UI to user to confirm
	const FText Title = FText::FromString(TEXT("Changing Map Data file"));
	const FText Message = FText::FromString(TEXT("This will replace all entries in MapData with the contents of file path: ") + FilePath);
	EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, Message, Title);
	// On yes replace MapData of WorkingAsset and create a transaction
	if(Result == EAppReturnType::Yes)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("MapDetailsChange", "UndoMapDataFilePathChange", "Map Data File Path Change"));
		WorkingAsset->Modify();
		WorkingAsset->SetMapData(NewMapData);
		WorkingAsset->SetFilePathMapData(FilePath);
		WorkingAsset->MarkPackageDirty();
	}
}

void FMapEditorApp::UpdateTextureDataEditor(UTexture2D* Texture, const uint8* Data, int32 Width, int32 Height)
{
	if (!Texture || !Data)
	{
		return;
	}

	// Lock the texture for editing
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);
    
	// Copy the new data
	const int32 DataSize = Width * Height * 4; // RGBA
	FMemory::Memcpy(TextureData, Data, DataSize);
    
	// Unlock and update
	Mip.BulkData.Unlock();
	Texture->UpdateResource();
}

FString FMapEditorApp::GetAbsoluteDir(const UObject* Asset)
{
	FString AssetDirectory;
	const FString AssetPath = Asset->GetPathName();
	// Convert package path to filesystem path
	FString AssetFilePath;
	FPackageName::TryConvertLongPackageNameToFilename(AssetPath, AssetFilePath, TEXT(".uasset"));
	FString DirPath = FPaths::GetPath(AssetFilePath);
	FPaths::NormalizeDirectoryName(DirPath);
	return FPaths::CreateStandardFilename(DirPath);
}
