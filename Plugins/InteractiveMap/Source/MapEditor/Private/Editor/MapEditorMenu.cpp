#include "Editor/MapEditorMenu.h"
#include "Engine/Texture2D.h"
#include "MapEditor.h"
#include "TextureCompiler.h"
#include "Editor/SMapTextureViewer.h"
#include "MapEditor/MapGenerator/source/map/Map.h"

MapEditorMenu::MapEditorMenu() : Map(1028, 1028)
{
	RegisterTabs();
}

MapEditorMenu::~MapEditorMenu()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ViewportTab);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ParametersTab);
}


void MapEditorMenu::RegisterTabs()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ViewportTab, FOnSpawnTab::CreateRaw(this, &MapEditorMenu::SpawnViewport))
		.SetDisplayName(NSLOCTEXT("YourModule", "ViewportTab", "Viewport"))
		.SetTooltipText(NSLOCTEXT("YourModule", "ViewportTabTooltip", "Open the Viewport tab"));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ParametersTab, FOnSpawnTab::CreateRaw(this, &MapEditorMenu::SpawnDetailsTab))
	.SetDisplayName(NSLOCTEXT("YourModule", "DetailsTab", "Details"))
	.SetTooltipText(NSLOCTEXT("YourModule", "DetailsTabTooltip", "Open the Details tab"));
	
}


TSharedRef<SDockTab> MapEditorMenu::SpawnDetailsTab(const FSpawnTabArgs& Args) const
{
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	const TSharedRef<IDetailsView> DetailsView = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsViewArgs);
	DetailsView->SetObjects(TArray<UObject*>{MapEditorPreset});
	
	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		.TabRole(ETabRole::PanelTab)
		[
			DetailsView
		];
}

TSharedRef<SDockTab> MapEditorMenu::SpawnViewport(const FSpawnTabArgs& Args)
{
	MapEditorPreset = NewObject<UMapEditorPreset>();
	TextureViewer = MakeShared<STextureViewer>();
	MapEditorPreset->OnObjectChanged.AddRaw(this, &MapEditorMenu::GenerateMap);
	return SNew(SDockTab)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SAssignNew(TextureViewer, STextureViewer)
		]
	];
}

void MapEditorMenu::GenerateMap()
{
	auto texture = MapEditorPreset->MapEditorDetails.HeightMapTexture;
	if (!IsValid(texture))
		return;
	
	FTextureCompilingManager::Get().FinishCompilation({texture});
	
	const uint8* Data = ReadTextureToBuffer(texture);
	
	// temp just to test
	// TODO - improve this, adjust MapGeneratorLib
	int32 Height = texture->GetSizeY();
	int32 Width = texture->GetSizeX();
	std::vector<uint8_t> vector = std::vector<uint8_t>(&Data[0], &Data[Width * Height * 4]);
	Map.GenerateMap(vector, Width, Height);
	LookupTexture = CreateLookupTexture(Map.GetLookupTileMap());

	// UpdateDisplayTexture
	if(TextureViewer && MapEditorPreset)
		TextureViewer->OnTextureChanged(LookupTexture.Get());
}

TObjectPtr<UTexture2D> MapEditorMenu::CreateLookupTexture(const MapGenerator::TileMap& TileMap)
{
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
	uint8_t* buffer = TileMap.ConvertTileMapToRawBuffer();
	const int Width = TileMap.Width();
	const int Height = TileMap.Height();

	// Set texture properties
	NewTexture->Source.Init(Width, Height, 1, 1, TSF_BGRA8);

	// Lock the mipmap and write the buffer data
	uint8* MipData = NewTexture->Source.LockMip(0);
	int32 BufferSize = Width * Height * 4; // BGRA8 format
	FMemory::Memcpy(MipData, buffer, BufferSize);
	NewTexture->Source.UnlockMip(0);
	
	// Update the texture to reflect changes
	NewTexture->PostEditChange();
	
	//Since we don't need access to the pixel data anymore free the memory
	delete[] buffer;
	buffer = nullptr;
	
	return NewTexture;
}

const uint8* MapEditorMenu::ReadTextureToBuffer(UTexture2D* texture) 
{
	// Lock the texture for reading
	FTexture2DMipMap& Mip = texture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);
	const uint8* Data = static_cast<const uint8*>(TextureData);
	// Unlock the texture
	Mip.BulkData.Unlock();

	return  Data;
}