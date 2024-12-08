// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Editor/SMapEditorMenu.h"
#include "Modules/ModuleManager.h"

class UMapEditorPreset;
DECLARE_LOG_CATEGORY_EXTERN(LogInteractiveMap, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInteractiveMapEditor, Log, All);

class FInteractiveMapEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterMenus() const;
	void AddMenuExtensions(FMenuBuilder& MenuBuilder) const;
	void LaunchMapEditor() const;
	void LaunchTerrainCreator() const;


	// Probably move to another class
	const TSharedRef<FTabManager::FLayout> CreateCustomLayout() const;
	void RegisterTabs();

	TSharedRef<SDockTab> SpawnDetailsTab(const FSpawnTabArgs& Args) const;
	TSharedRef<SDockTab> SpawnTextureSelectTab(const FSpawnTabArgs& Args) const;
	TSharedRef<SDockTab> SpawnViewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnMainTab(const FSpawnTabArgs& Args) const;
private:
	FName ViewportTab = FName(TEXT("ViewportTab"));
	FName TextureSelectTab = FName(TEXT("StatsTab"));
	FName ParametersTab = FName(TEXT("DetailsTab"));
	FName MapEditorMainTab = FName(TEXT("MapEditorMainTab"));

	UFUNCTION()
	void GenerateMap();
	
	UMapEditorPreset* MapEditorPreset = nullptr;
	TSharedPtr<SMapEditorMenu> MapEditorMenu;
	
	
	
};
