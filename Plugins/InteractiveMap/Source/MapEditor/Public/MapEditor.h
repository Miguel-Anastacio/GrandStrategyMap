// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	
	// Probably move to another class
	const TSharedRef<FTabManager::FLayout> CreateCustomLayout() const;
	void RegisterTabs();
	TSharedRef<SDockTab> SpawnMainTab(const FSpawnTabArgs& Args) const;

	static bool SaveObject(UObject* Object);

	static UObject* CreateAsset(const FString& assetPath, UClass* assetClass, UFactory* factory, bool& bOutSuccess, FString& OutInfoMessage);
	// static UObject* CreateAssetFrom(const FString& assetPath, UClass* assetClass, UFactory* factory, bool& bOutSuccess, FString& OutInfoMessage, const TArray<UObject*>& Objects);
	static UTexture2D* CreateTexture(const FString& assetPath, bool& bOutSuccess, FString& OutInfoMessage, uint8* Buffer, unsigned Width, unsigned Height);
private:
	FName MapEditorMainTab = FName(TEXT("MapEditorMainTab"));
	MapEditorMenu* _MapEditorMenu;


};
