// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "Modules/ModuleManager.h"

class MapEditorMenu;
class UMapEditorPreset;
DECLARE_LOG_CATEGORY_EXTERN(LogInteractiveMap, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInteractiveMapEditor, Log, All);
static FName ViewportTab = FName(TEXT("ViewportTab"));
static FName ParametersTab = FName(TEXT("DetailsTab"));

class FInteractiveMapEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterMenus() const;
	void LaunchMapEditor() const;
	void LaunchTerrainCreator() const;
	void RegisterSlateStyles() const;

	// Custom Assets
	static EAssetTypeCategories::Type CustomAssetCategory;
	static TSharedPtr<FSlateStyleSet> CustomAssetsEditorSlateStyle;
	
	// Probably move to another class
	const TSharedRef<FTabManager::FLayout> CreateCustomLayout() const;
	void RegisterTabs();
	TSharedRef<SDockTab> SpawnMainTab(const FSpawnTabArgs& Args) const;

private:
	FName MapEditorMainTab = FName(TEXT("MapEditorMainTab"));
	MapEditorMenu* _MapEditorMenu;


};
