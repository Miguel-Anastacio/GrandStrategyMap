#include "Asset/MapObjectToolkit.h"
#include "Asset/MapObject.h"
#include "Asset/SMapObjectViewport.h"
#include "Asset/DataDisplay/AdvancedStructWrapper.h"
#include "Asset/DataDisplay/MapDataSettingsPreset.h"
#include "Asset/DataDisplay/STreeJsonDisplay.h"
#include "FileIO/FilePickerFunctionLibrary.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Docking/SDockTab.h"
#include "Internationalization/Text.h"
#include "Framework/Docking/TabManager.h"
#include "Asset/DataDisplay/BasicStructWrapper.h"

FName MapViewportTab = FName(TEXT("MapViewportTab"));
FName MapStatsTab = FName(TEXT("MapStatsTab"));
FName DataSourceTab = FName(TEXT("DataSourceTab"));
FName DataListTab = FName(TEXT("DataListTab"));

void FMapObjectToolkit::InitEditor(const TSharedPtr<IToolkitHost >& InitToolkitHost, UMapObject* Object)
{
	CustomObject = Object;
	DataSettingsPreset = NewObject<UMapDataSettings>();
	DataSettingsPreset->OnObjectChanged.AddSP(this, &FMapObjectToolkit::OpenFiles);
	
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout(FName(TEXT("Layout")))
	->AddArea
	(
		FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Horizontal)
		->Split
		(
			FTabManager::NewSplitter()
			->SetOrientation(Orient_Vertical)
			->SetSizeCoefficient(.65f)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(.5f)
				->AddTab(MapViewportTab, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(.5f)
				->AddTab(MapStatsTab, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
		)
		->Split
		(
			FTabManager::NewSplitter()
			->SetOrientation(Orient_Vertical)
			->SetSizeCoefficient(.65f)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(.6f)
				->AddTab(DataSourceTab, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(.4f)
				->AddTab(DataListTab, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
		)	
	);
	
	InitAssetEditor(EToolkitMode::Standalone, InitToolkitHost, GetToolkitFName(), Layout, true, true, CustomObject.Get());
}

void FMapObjectToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("CustomConfigEditor"));

	// VIEWPORT
	InTabManager->RegisterTabSpawner(MapViewportTab, FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SMapObjectViewport)
				.EditingObject(CustomObject.Get())
			]
		];
	}))
	.SetDisplayName(INVTEXT("Viewport"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// STATS PLACEHOLDER
	InTabManager->RegisterTabSpawner(MapStatsTab, FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		.TabRole(PanelTab)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.Padding(10)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Visibility(EVisibility::HitTestInvisible)
				.TextStyle(FAppStyle::Get(), "Graph.CornerText")
				.Text(FText::FromString("STATS PLACEHOLDER"))
			]
		];
	}))
	.SetDisplayName(INVTEXT("Stats"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// DETAILS VIEW
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	const TSharedRef<IDetailsView> DetailsView = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsViewArgs);
	DetailsView->SetObjects(TArray<UObject*>{DataSettingsPreset.Get()});
	
	InTabManager->RegisterTabSpawner(DataSourceTab, FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		.TabRole(PanelTab)
		[
			DetailsView
		];
	}))
	.SetDisplayName(INVTEXT("Data Source"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// FTestAdvanced AdvancedStruct(1, "Test Name", FLinearColor::Black);
	AdvancedStruct.ID =1;
	AdvancedStruct.Name = "Test Name";
	AdvancedStruct.Color = FLinearColor::Black;
	AdvancedStruct.Population = 3839864;

	InTabManager->RegisterTabSpawner(DataListTab, FOnSpawnTab::CreateLambda([this, InTabManager](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		.TabRole(PanelTab)
		[
			SAssignNew(TreeDisplay, STreeJsonDisplay, InTabManager.ToSharedPtr().Get())
			.StructType(FTestAdvanced::StaticStruct())
			.StructInstances({&AdvancedStruct, &AdvancedStruct, &AdvancedStruct, &AdvancedStruct})
		];
	}))
	.SetDisplayName(INVTEXT("DataList"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FMapObjectToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManagerRef)
{
	CustomObject = nullptr;

	TabManagerRef->UnregisterAllTabSpawners();
	
	FAssetEditorToolkit::UnregisterTabSpawners(TabManagerRef);
}

void FMapObjectToolkit::OpenFiles()
{
	TArray<FString> FilesNames;
	UFilePickerFunctionLibrary::OpenFileDialogJson(FPaths::ProjectDir(), FilesNames);
	// UObject* Object = DataSettingsPreset->MapDataSettings.StructWrapperObject;
	// UObject* Object = DataSettingsPreset->MapDataSettings.StructWrapperObject;
	UBasicStructWrapper* Object = NewObject<UBasicStructWrapper>();	
		// UE_LOG(LogTemp, Display, TEXT("Object pointer: %p"), DataSettingsPreset->MapDataSettings.StructWGetParentNativeClass(rapGetParentNativeClass(GetParentNativeClass(perObject);
	if(Object)
	{
		UE_LOG(LogTemp, Display, TEXT("Object class: %s"), *(Object->GetClass())->GetName());
		// if (Object->IsA<UStructWrapper>())
		// {
		// 	IStructWrapperInterface* Interface = Cast<IStructWrapperInterface>(Object);
		// 	if (Interface)
		// 	{
		// 		UE_LOG(LogTemp, Display, TEXT("Object implements IStructWrapperInterface"));
		// 		Interface->OpenFileOfType(FilesNames, TreeDisplay);
		// 	}
		// }
		
		if(Object->Implements<UStructWrapperInterface>())
		{
			IStructWrapperInterface* Interface = Cast<IStructWrapperInterface>(Object);
			if(Interface)
			{
				UE_LOG(LogTemp, Display, TEXT("%s"), *Object->GetClass()->GetName());
				Interface->OpenFileOfType(FilesNames, TreeDisplay);
			}
		}
	}
	else
	{
		// TODO log something
	}
	if(DataSettingsPreset->MapDataSettings.TestWrapper.Get())
	{
		if(DataSettingsPreset->MapDataSettings.TestWrapper.Get()->ImplementsInterface(UStructWrapperInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Display, TEXT("HEY"));
		}
		UE_LOG(LogTemp, Display, TEXT("%s"), *DataSettingsPreset->MapDataSettings.TestWrapper.Get()->GetName());
	}
	
	// TArray<Struct->get> SelectedFiles;
}


