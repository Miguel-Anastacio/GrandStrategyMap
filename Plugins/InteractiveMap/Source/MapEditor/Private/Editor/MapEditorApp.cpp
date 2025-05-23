// Copyright 2025 An@stacioDev All rights reserved.
#include "MapEditorApp.h" 
#include "Editor/NameDefines.h"
#include "MapObject.h"
#include "Asset/SMapObjectViewport.h"
#include "BlueprintLibrary/MiscFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "Editor/MapEditorPreset.h"
#include "Editor/SMapTextureViewer.h"
#include "MapDataMode/MapEditorDataAppMode.h"
#include "MapEditorMode/MapEditorGenAppMode.h"

void FMapEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(tabManager);
}

void FMapEditorApp::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost,
	UObject* InObject)
{
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
	
	SetCurrentMode(MapDataEditorModeName);
}

void FMapEditorApp::OnTexturePreviewClicked(FName ID) const
{
	if(GetCurrentMode() == MapEditorGenModeName)
	{
		UTexture2D* Texture2D = nullptr;
		if(ID == FName("Lookup"))
		{
			Texture2D = WorkingAsset->GetPreviewTextures()[0];
		}
		else if(ID == FName("LookupLand"))
		{
			Texture2D = WorkingAsset->GetPreviewTextures()[1];
		}
		else if(ID == FName("LookupOcean"))
		{
			Texture2D = WorkingAsset->GetPreviewTextures()[2];
		}
		else if(ID == FName("HeightMap"))
		{
			Texture2D = WorkingAsset->GetPreviewTextures()[3];
		}
		
		if(Texture2D)
		{
			MapViewport->UpdatePreviewActorMaterial(MapGenPreset->Material, Texture2D);
		}
	}
}

void FMapEditorApp::GenerateMap()
{
	TSharedPtr<MapGenerator::Map> MapGen = GetWorkingAsset()->GetMapGen();
	
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
			const MapGenerator::MapModeGen GenType = MapGenPreset->FromHeightMap() ? MapGenerator::MapModeGen::FromHeightMap
																						: MapGenerator::MapModeGen::FromMask;
			MapGen->GenerateMap(vector, Width, Height, MapGenPreset->GetLookupMapData(), GenType, ProgressCallback);
			
			UTexture2D* LookupTexture = CreateLookupTexture(MapGen->GetLookupTileMap());
			UTexture2D* LookupLandTexture = CreateTexture(MapGen->GetLookupTileMap().GetLandTileMap(), Width, Height);
			UTexture2D* LookupOceanTexture = CreateTexture(MapGen->GetLookupTileMap().GetOceanTileMap(), Width, Height);
			// UpdateDisplayTexture
			if(UMapObject* MapObject = GetWorkingAsset())
			{
				MapObject->SetPreviewTextures(TArray<UTexture2D*>
												{
													LookupTexture,
													LookupLandTexture,
													LookupOceanTexture,
													Texture
												});
				MapObject->SetLookupTexture(LookupTexture);
			}

			MapViewport->UpdatePreviewActorMaterial(MapGenPreset->Material, LookupTexture);
			
			RestoreTexturePreview();
			
			const uint32 Size = MapGen->GetLookupTileMap().GetCellMap().size();
			// UE_LOG(LogInteractiveMapEditor, Warning, TEXT("CellMap size after gen %d"), Size);

		});
}

void FMapEditorApp::RestoreTexturePreview() const
{
	if(!WorkingAsset)
		return;
	const TArray<UTexture2D*> Textures = WorkingAsset->GetPreviewTextures();
	if(MapTexturePreview && Textures.Num() > 3)
	{
		MapTexturePreview->SetTextures(TArray
							{
								TPair<FName, UTexture2D*>(FName("Lookup"), Textures[0]),
								TPair<FName, UTexture2D*>(FName("LookupLand"), Textures[1]),
								TPair<FName, UTexture2D*>(FName("LookupOcean"), Textures[2]),
								TPair<FName, UTexture2D*>(FName("HeightMap"), Textures[3])
							});
	}

	if(!Textures.IsEmpty())	
		MapViewport->UpdatePreviewActorMaterial(MapGenPreset->Material, Textures[0]);

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
					return GetCurrentMode() != MapDataEditorModeName;
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

	// TODO - CORRECT THIS FUNCTION 
	//Create a string containing the texture's path
	FString PackageName = TEXT("/Game/ProceduralTextures/");
	FString BaseTextureName = FString("LookupTexture");
	PackageName += BaseTextureName;
 
	//Create the package that will store our texture
	UPackage* Package = CreatePackage(*PackageName);
	GLog->Log("project dir:" + FPaths::ProjectDir());
 
	//Create a unique name for our asset. For example, if a texture named ProcTexture already exists the editor
	//will name the new texture as "ProcTexture_1"
	FName TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*BaseTextureName));
	Package->FullyLoad();
 
	TObjectPtr<UTexture2D> NewTexture = NewObject<UTexture2D>(Package, TextureName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	
	// Set texture properties
	NewTexture->Source.Init(Width, Height, 1, 1, TSF_BGRA8);
	
	// Lock the mipmap and write the buffer data
	uint8* MipData = NewTexture->Source.LockMip(0);
	int32 BufferSize = Width * Height * 4; // BGRA8 format
	FMemory::Memcpy(MipData, Buffer, BufferSize);
	NewTexture->Source.UnlockMip(0);
	
	// Update the texture to reflect changes
	NewTexture->PostEditChange();

	// TODO - CHANGE THIS This function should not manage the buffer memory
	//Since we don't need access to the pixel data anymore free the memory
	delete[] Buffer;
	Buffer = nullptr;
	
	return NewTexture;
}