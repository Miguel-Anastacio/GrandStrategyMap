#include "Editor/MapEditorMenu.h"
#include "Engine/Texture2D.h"
#include "MapEditor.h"
#include "MapObject.h"
#include "BlueprintLibrary/ADStructUtilsFunctionLibrary.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "Editor/SMapTextureViewer.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"
#include "BlueprintLibrary/FilePickerFunctionLibrary.h"
#include "BlueprintLibrary/SlowTaskProgressBarFunctionLibrary.h"
#include "MapEditor/MapGenerator/source/map/Map.h"
#include "Materials/MaterialInstanceConstant.h"

RMapEditorMenu::RMapEditorMenu() : Map(1028, 1028)
{
	RegisterTabs();
}

RMapEditorMenu::~RMapEditorMenu()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ViewportTab);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ParametersTab);
}

void RMapEditorMenu::RegisterTabs()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ViewportTab, FOnSpawnTab::CreateRaw(this, &RMapEditorMenu::SpawnViewport))
		.SetDisplayName(NSLOCTEXT("YourModule", "ViewportTab", "Viewport"))
		.SetTooltipText(NSLOCTEXT("YourModule", "ViewportTabTooltip", "Open the Viewport tab"));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ParametersTab, FOnSpawnTab::CreateRaw(this, &RMapEditorMenu::SpawnDetailsTab))
	.SetDisplayName(NSLOCTEXT("YourModule", "DetailsTab", "Details"))
	.SetTooltipText(NSLOCTEXT("YourModule", "DetailsTabTooltip", "Open the Details tab"));
}


TSharedRef<SDockTab> RMapEditorMenu::SpawnDetailsTab(const FSpawnTabArgs& Args)
{
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	const TSharedRef<IDetailsView> DetailsView = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsViewArgs);
	DetailsView->SetObjects(TArray<UObject*>{MapEditorPreset});
	
	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		[
			SNew(SVerticalBox)
			// Add the Details View at the top
			+ SVerticalBox::Slot()
			.AutoHeight() // Let the Details View determine its own height
			[
				DetailsView
			]

			// Add a Button Below
			+ SVerticalBox::Slot()
			.Padding(10.0f) // Add padding around the button
			.HAlign(HAlign_Center) // Center the button horizontally
			.VAlign(VAlign_Top)    // Align the button to the top of its slot
			.AutoHeight()          // Let the button determine its own height
			[
				SNew(SButton)
				.Text(FText::FromString("Click Me"))
				.OnClicked_Lambda([this]() -> FReply
				{
					// Button click handler logic
					GenerateMap();
					return FReply::Handled();
				})
			]
			+ SVerticalBox::Slot()
			.Padding(10.0f) // Add padding around the button
			.HAlign(HAlign_Center) // Center the button horizontally
			.VAlign(VAlign_Top)    // Align the button to the top of its slot
			.AutoHeight()          // Let the button determine its own height
			[
				SNew(SButton)
				.Text(FText::FromString("Save"))
				.OnClicked_Lambda([this]() -> FReply
				{
					SaveMap();
					return FReply::Handled();
				})
			]
		];	
}

TSharedRef<SDockTab> RMapEditorMenu::SpawnViewport(const FSpawnTabArgs& Args)
{
	MapEditorPreset = NewObject<UMapEditorPreset>();
	TextureViewer = MakeShared<STextureViewer>();
	// MapEditorPreset->OnObjectChanged.AddRaw(this, &RMapEditorMenu::GenerateMap);
	return SNew(SDockTab)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SAssignNew(TextureViewer, STextureViewer)
		]
	];
}

void RMapEditorMenu::GenerateMap()
{
	UTexture2D* Texture = MapEditorPreset->MapEditorDetails.HeightMapTexture;
	const uint8* Data = UTextureUtilsFunctionLibrary::ReadTextureToBuffer(Texture);
	if(!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read Height Map texture"));
		return;
	}
	 
	// TODO - improve this, adjust MapGeneratorLib
	const uint32 Height = Texture->GetSizeY();
	const uint32 Width = Texture->GetSizeX();
	const std::vector<uint8_t> vector = std::vector(&Data[0], &Data[Width * Height * 4]);
	USlowTaskProgressBarFunctionLibrary::ExecuteSlowTaskWithProgressBar(
		[&, vector, this](TFunction<void(float, std::string_view)> ProgressCallback)
		{
			// Call GenerateMap and pass the progress callback
			Map.GenerateMap(vector, Width, Height, MapEditorPreset->GetLookupMapData(), ProgressCallback);
			LookupTexture = CreateLookupTexture(Map.GetLookupTileMap());
			LookupLandTexture = CreateTexture(Map.GetLookupTileMap().GetLandTileMap(), Width, Height);
			LookupOceanTexture = CreateTexture(Map.GetLookupTileMap().GetOceanTileMap(), Width, Height);
			// UpdateDisplayTexture
			if(TextureViewer && MapEditorPreset)
			{
				TextureViewer->OnTextureChanged(LookupTexture.Get());
				TextureViewer->SetTextures(TArray<UTexture2D*>{LookupTexture.Get(), LookupLandTexture.Get(), LookupOceanTexture.Get(), Texture});
			}

			const uint32 Size = Map.GetLookupTileMap().GetCellMap().size();
			UE_LOG(LogInteractiveMapEditor, Warning, TEXT("CellMap size after gen %d"), Size);

		});
}

void RMapEditorMenu::SaveMap() const
{
	FString DirPath = FPaths::ProjectContentDir();
	UFilePickerFunctionLibrary::OpenDirectoryDialog("Select Folder To Save Assets", FPaths::ProjectContentDir(), DirPath);
				
	FPaths::NormalizeDirectoryName(DirPath);
	if(!FPaths::IsUnderDirectory(DirPath, FPaths::ProjectContentDir()))
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Folder to save Map Object must be inside project content"));
		return;
	}
	
	const FString ProjectContentsDir = FPaths::CreateStandardFilename(FPaths::ProjectContentDir());

	const FString CompleteDirPath =  FPaths::CreateStandardFilename(DirPath);
	
	if(!DirPath.RemoveFromStart(ProjectContentsDir))
	{
		return;
	}
	const FString PackagePath = "/Game/" + DirPath + "/";
	UTexture2D* TextureAsset = CreateLookupTextureAsset(PackagePath);
	if(!TextureAsset)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to create material Asset"));
		return;
	}
	
	UMaterialInstanceConstant* Material = CreateMaterialInstanceAsset(TextureAsset, PackagePath);
	if(!Material)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create material Asset"));
		// return;
	}

	const FString LookupFilePath = CompleteDirPath + "/lookup.json";
	OutputLookupJson(LookupFilePath);
	
	const FString StubMapDataFilePath = CompleteDirPath + "/MapDataStub.json";
	OutputStubMapDataJson(StubMapDataFilePath);
	// Outputs the params and the results of the Map Lookup Created
	const FString LookupGenResultsFilePath = CompleteDirPath + "/MapGenParamsResults.json";
	OutputLookupGenFile(LookupGenResultsFilePath);
	
	CreateMapObjectAsset(PackagePath, TextureAsset, LookupFilePath, StubMapDataFilePath, Material);
	
	FString Message;
	UAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
}

void RMapEditorMenu::AddReferencedObjects(FReferenceCollector& Collector)
{
	if(MapEditorPreset)
	{
		Collector.AddReferencedObject(MapEditorPreset);
	}
}

TObjectPtr<UTexture2D> RMapEditorMenu::CreateLookupTexture(const MapGenerator::TileMap& TileMap)
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

TObjectPtr<UTexture2D> RMapEditorMenu::CreateTexture(uint8* Buffer, unsigned Width, unsigned Height)
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

void RMapEditorMenu::OutputLookupJson(const FString& FilePath) const
{
	int32 Id = 0;
	TArray<FTileIdData> OutputData;
	OutputData.Reserve(Map.GetLookupTileMap().GetCellMap().size());
	for (const auto& Cell : Map.GetLookupTileMap().GetCellMap())
	{
		OutputData.Emplace(FTileIdData(Cell.second.ConvertToHex().c_str(), Id));
		Id++;
	}
	bool bResult = false;
	FString Message;
	UDataManagerFunctionLibrary::WriteArrayToJsonFile(FilePath, OutputData, bResult, Message);
	UE_LOG(LogInteractiveMapEditor, Warning, TEXT("%s"), *Message);
}

UTexture2D* RMapEditorMenu::CreateLookupTextureAsset(const FString& PackagePath) const
{
	const auto AssetPath = UAssetCreatorFunctionLibrary::CreateUniqueAssetNameInPackage(PackagePath, "LookupTexture", UTexture2D::StaticClass());
	FString Message;
	bool bResult = false;
	uint8_t* Buffer = Map.GetLookupTileMap().ConvertTileMapToRawBuffer();
	UTexture2D* Texture = UAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(PackagePath + AssetPath, Buffer, Map.Width(), Map.Height(), bResult, Message);
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

UMapObject* RMapEditorMenu::CreateMapObjectAsset(const FString& PackagePath, UTexture2D* Texture, const FString& LookupFilePath, const FString& MapDataFilePath,
													UMaterialInstanceConstant* Material) const
{
	const auto AssetPath = UAssetCreatorFunctionLibrary::CreateUniqueAssetNameInPackage(PackagePath, "MapObject", UMapObject::StaticClass());
	FString Message;
	bool bResult = false;
	UObject* Asset = UAssetCreatorFunctionLibrary::CreateAsset(PackagePath + AssetPath, UMapObject::StaticClass(), nullptr, bResult, Message);
	if(!Asset)
	{
		return nullptr;
	}
	UMapObject* MapObject = Cast<UMapObject>(Asset);
	MapObject->SetLookupTexture(Texture);
	MapObject->SetLookupFilePath(LookupFilePath);
	if(Material)
	{
		MapObject->MaterialOverride = Material;
	}

	if(MapEditorPreset && MapEditorPreset->TileDataStructType)
	{
		MapObject->StructType = MapEditorPreset->TileDataStructType;
		MapObject->OceanStructType = MapEditorPreset->OceanTileDataType;
		MapObject->SetMapDataFilePath(MapDataFilePath);
	}
	
	MapObject->PostEditChange();

	return MapObject;
}

UMaterialInstanceConstant* RMapEditorMenu::CreateMaterialInstanceAsset(UTexture2D* Texture, const FString& PackagePath) const
{
	if(MapEditorPreset && MapEditorPreset->Material)
	{
		const auto AssetPath = UAssetCreatorFunctionLibrary::CreateUniqueAssetNameInPackage(PackagePath, "MI_LookupMaterial", UMaterialInstance::StaticClass());
		FString Message;
		bool bResult = false;
		UObject* Asset = UAssetCreatorFunctionLibrary::CreateAsset(PackagePath + AssetPath, UMaterialInstanceConstant::StaticClass(), nullptr, bResult, Message);
		if(!Asset)
		{
			return nullptr;
		}

		UMaterialInstanceConstant* MaterialInstance= Cast<UMaterialInstanceConstant>(Asset);
		MaterialInstance->SetParentEditorOnly(MapEditorPreset->Material);
		MaterialInstance->SetTextureParameterValueEditorOnly(FName("DynamicTexture"), Texture);
		MaterialInstance->SetTextureParameterValueEditorOnly(FName("LookupTexture"), Texture);
		MaterialInstance->PostEditChange();
		return MaterialInstance;
	}

	return nullptr;
}

void RMapEditorMenu::OutputStubMapDataJson(const FString& FilePath) const
{
	TArray<FInstancedStruct> Output;
	const uint32 Size = Map.GetLookupTileMap().GetCellMap().size();
	Output.Reserve(Size);
	uint32 Index = 0;
	UE_LOG(LogInteractiveMapEditor, Warning, TEXT("CellMap size %d"), Size);
	const MapGenerator::TileMap& TileMap = Map.GetLookupTileMap();
	for (const auto& [Position, Color] : TileMap.GetCellMap())
	{
		FInstancedStruct Struct;
		// Create a struct dependent on the type of tile
		if(TileMap.IsTileOfType(MapGenerator::TileType::LAND, Position.x, Position.y))
		{
			Struct.InitializeAs(MapEditorPreset->TileDataStructType);
		}
		else if(TileMap.IsTileOfType(MapGenerator::TileType::WATER, Position.x, Position.y))
		{
			Struct.InitializeAs(MapEditorPreset->OceanTileDataType);
		}
		else
		{
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("Tile in LookupTileMap is undefined!!"));
			return;
		}
		
		if(!UADStructUtilsFunctionLibrary::SetPropertyValueInStruct(Struct, "ID", Index))
		{
			UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to create stub Map Data of type %s - ID is missing"),
					*MapEditorPreset->TileDataStructType->GetName());
			return;
		}
		
		Output.Emplace(Struct);
		Index++;
	}

	UDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePath, Output);
}

void RMapEditorMenu::OutputLookupGenFile(const FString& FilePath) const
{
	int32 OceanTiles = 0;
	int32 LandTiles = 0;
	const MapGenerator::TileMap& TileMap = Map.GetLookupTileMap();
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
	const FMapLookupGenFeedback GenInputsResults(MapEditorPreset->MapEditorDetails, LandTiles, OceanTiles);
	bool bResult = false;
	FString Message;
	UDataManagerFunctionLibrary::WriteStructToJsonFile(FilePath, GenInputsResults, bResult, Message);
	if(!bResult)
	{
		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to Save map gen results %s"), *Message);
	}
}
