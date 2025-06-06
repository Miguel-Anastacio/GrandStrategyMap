// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapEditor.h"
#include "ToolMenus.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Styling/SlateStyleRegistry.h"

DEFINE_LOG_CATEGORY(LogInteractiveMapEditor);

#define LOCTEXT_NAMESPACE "FInteractiveMapModule"

void FInteractiveMapEditorModule::StartupModule()
{
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been loaded"));
	RegisterSlateStyles();
}
void FInteractiveMapEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	UE_LOG(LogInteractiveMapEditor, Log, TEXT("Map Generator Editor module has been unloaded"));
}

TSharedPtr<FSlateStyleSet> FInteractiveMapEditorModule::CustomAssetsEditorSlateStyle;

void FInteractiveMapEditorModule::RegisterSlateStyles() const
{
	// Register the slate styles	
	const FString ProjectResourceDir = FPaths::ProjectPluginsDir()/TEXT("CustomEditableAssets/Resources");
	
	CustomAssetsEditorSlateStyle = MakeShareable(new FSlateStyleSet("CustomEditableAssetsEditor"));
	CustomAssetsEditorSlateStyle->Set("CustomAsset.Thumbnail", new FSlateImageBrush(FString(ProjectResourceDir/"Icon128.png"), FVector2D(128.0f, 128.0f)));
	CustomAssetsEditorSlateStyle->Set("ClassIcon.CustomActor", new FSlateImageBrush(FString(ProjectResourceDir/"Icon128.png"), FVector2D(32.0f, 32.0f)));
	CustomAssetsEditorSlateStyle->Set("ClassIcon.UAssetDefinition_CustomObject", new FSlateImageBrush(FString(ProjectResourceDir/"Icon128.png"), FVector2D(32.0f, 32.0f)));
	
	FSlateStyleRegistry::RegisterSlateStyle(*CustomAssetsEditorSlateStyle.Get());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInteractiveMapEditorModule, MapEditor)