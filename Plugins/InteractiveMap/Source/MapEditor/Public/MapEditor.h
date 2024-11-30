// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

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
};
