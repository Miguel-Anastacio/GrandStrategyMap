// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractiveMapEditor, Log, All);

class FInteractiveMapEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterSlateStyles() const;

	// Custom Assets
	static EAssetTypeCategories::Type CustomAssetCategory;
	static TSharedPtr<FSlateStyleSet> CustomAssetsEditorSlateStyle;

};
