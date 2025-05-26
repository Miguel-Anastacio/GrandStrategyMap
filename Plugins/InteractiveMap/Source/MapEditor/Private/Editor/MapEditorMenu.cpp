#include "Editor/MapEditorMenu.h"
#include "Engine/Texture2D.h"
#include "MapEditor.h"
#include "MapObject.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "BlueprintLibrary/TextureUtilsFunctionLibrary.h"
#include "Editor/SMapTextureViewer.h"
#include "BlueprintLibrary/FilePickerFunctionLibrary.h"
#include "BlueprintLibrary/MiscFunctionLibrary.h"
#include "Materials/MaterialInstanceConstant.h"

RMapEditorMenu::RMapEditorMenu() 
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

void RMapEditorMenu::SaveMap() const
{
	FString DirPath = FPaths::ProjectContentDir();
	UAtkFilePickerFunctionLibrary::OpenDirectoryDialog("Select Folder To Save Assets", FPaths::ProjectContentDir(), DirPath);
				
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
	
	// CreateMapObjectAsset(PackagePath, TextureAsset, LookupFilePath, StubMapDataFilePath, Material);
	
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
}

void RMapEditorMenu::AddReferencedObjects(FReferenceCollector& Collector)
{
	if(MapEditorPreset)
	{
		Collector.AddReferencedObject(MapEditorPreset);
	}
}

void RMapEditorMenu::OutputLookupJson(const FString& FilePath) const
{
	// int32 Id = 0;
	// TArray<FTileIdData> OutputData;
	// OutputData.Reserve(Map.GetLookupTileMap().GetCellMap().size());
	// for (const auto& Cell : Map.GetLookupTileMap().GetCellMap())
	// {
	// 	OutputData.Emplace(FTileIdData(Cell.second.ConvertToHex().c_str(), Id));
	// 	Id++;
	// }
	// bool bResult = false;
	// FString Message;
	// UAtkDataManagerFunctionLibrary::WriteArrayToJsonFile(FilePath, OutputData, bResult, Message);
	// UE_LOG(LogInteractiveMapEditor, Warning, TEXT("%s"), *Message);
}

UTexture2D* RMapEditorMenu::CreateLookupTextureAsset(const FString& PackagePath) const
{
	// const auto AssetPath = UAtkAssetCreatorFunctionLibrary::CreateUniqueAssetNameInPackage(PackagePath, "LookupTexture", UTexture2D::StaticClass());
	// FString Message;
	// bool bResult = false;
	// uint8_t* Buffer = Map.GetLookupTileMap().ConvertTileMapToRawBuffer();
	// UTexture2D* Texture = UAtkAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(PackagePath + AssetPath, Buffer, Map.Width(), Map.Height(), bResult, Message);
	// UE_LOG(LogInteractiveMapEditor, Warning, TEXT("%s"), *Message);
	//
	// if(Texture)
	// {
	// 	Texture->CompressionSettings = TC_EditorIcon;
	// 	Texture->MipGenSettings = TMGS_NoMipmaps;
	// 	Texture->PostEditChange();
	// }
	//
	// if(Buffer)
	// {
	// 	delete[] Buffer;
	// 	Buffer = nullptr;
	// }
	//
	// return Texture;
	return nullptr;
}

UMapObject* RMapEditorMenu::CreateMapObjectAsset(const FString& PackagePath, UTexture2D* Texture, const FString& LookupFilePath, const FString& MapDataFilePath,
													UMaterialInstanceConstant* Material) const
{
	const auto AssetPath = UAtkAssetCreatorFunctionLibrary::CreateUniqueAssetNameInPackage(PackagePath, "MapObject", UMapObject::StaticClass());
	FString Message;
	bool bResult = false;
	UObject* Asset = UAtkAssetCreatorFunctionLibrary::CreateAsset(PackagePath + AssetPath, UMapObject::StaticClass(), nullptr, bResult, Message);
	if(!Asset)
	{
		return nullptr;
	}
	UMapObject* MapObject = Cast<UMapObject>(Asset);
	MapObject->SetLookupTexture(Texture);
	MapObject->SetLookupFilePath(LookupFilePath);
	if(Material)
	{
		// MapObject->MaterialOverride = Material;
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
		const auto AssetPath = UAtkAssetCreatorFunctionLibrary::CreateUniqueAssetNameInPackage(PackagePath, "MI_LookupMaterial", UMaterialInstance::StaticClass());
		FString Message;
		bool bResult = false;
		UObject* Asset = UAtkAssetCreatorFunctionLibrary::CreateAsset(PackagePath + AssetPath, UMaterialInstanceConstant::StaticClass(), nullptr, bResult, Message);
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
	// TArray<FInstancedStruct> Output;
	// const uint32 Size = Map.GetLookupTileMap().GetCellMap().size();
	// Output.Reserve(Size);
	// uint32 Index = 0;
	// UE_LOG(LogInteractiveMapEditor, Warning, TEXT("CellMap size %d"), Size);
	// const MapGenerator::TileMap& TileMap = Map.GetLookupTileMap();
	// for (const auto& [Position, Color] : TileMap.GetCellMap())
	// {
	// 	FInstancedStruct Struct;
	// 	// Create a struct dependent on the type of tile
	// 	if(TileMap.IsTileOfType(MapGenerator::TileType::LAND, Position.x, Position.y))
	// 	{
	// 		Struct.InitializeAs(MapEditorPreset->TileDataStructType);
	// 	}
	// 	else if(TileMap.IsTileOfType(MapGenerator::TileType::WATER, Position.x, Position.y))
	// 	{
	// 		Struct.InitializeAs(MapEditorPreset->OceanTileDataType);
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Tile in LookupTileMap is undefined!!"));
	// 		return;
	// 	}
	// 	
	// 	if(!UAtkStructUtilsFunctionLibrary::SetPropertyValueInStruct(Struct, "ID", Index))
	// 	{
	// 		UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to create stub Map Data of type %s - ID is missing"),
	// 				*MapEditorPreset->TileDataStructType->GetName());
	// 		return;
	// 	}
	// 	
	// 	Output.Emplace(Struct);
	// 	Index++;
	// }
	//
	// UAtkDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(FilePath, Output);
}

void RMapEditorMenu::OutputLookupGenFile(const FString& FilePath) const
{
	// int32 OceanTiles = 0;
	// int32 LandTiles = 0;
	// const MapGenerator::TileMap& TileMap = Map.GetLookupTileMap();
	// for (const auto& [Position, Color] : TileMap.GetCellMap())
	// {
	// 	if(TileMap.IsTileOfType(MapGenerator::TileType::LAND, Position.x, Position.y))
	// 	{
	// 		LandTiles++;
	// 	}
	// 	else if(TileMap.IsTileOfType(MapGenerator::TileType::WATER, Position.x, Position.y))
	// 	{
	// 		OceanTiles++;
	// 	}
	// }
	// const FMapLookupGenFeedback GenInputsResults(MapEditorPreset->MapEditorDetails, LandTiles, OceanTiles);
	// bool bResult = false;
	// FString Message;
	// UAtkDataManagerFunctionLibrary::WriteStructToJsonFile(FilePath, GenInputsResults, bResult, Message);
	// if(!bResult)
	// {
	// 	UE_LOG(LogInteractiveMapEditor, Error, TEXT("Failed to Save map gen results %s"), *Message);
	// }
}
