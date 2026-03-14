// Copyright 2024 An@stacioDev All rights reserved.
#include "Editor/CreateMapUIElements.h"
#include "CreateWidgetAssetAction.h"
#include "UserWidgets/DoubleTextBlock.h"
#include "EditorUtilityLibrary.h"
#include "MapObject.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "Components/VerticalBox.h"
#include "GenericWidget/GenericStructWidget.h"
#include "GenericWidget/GenericWidgetDataMap.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UI/Widgets/CustomButtonWidget.h"
#include "UI/Widgets/GrandStrategyHUDWidget.h"
#include "UI/Widgets/MapModeSelectorWidget.h"
#include "UI/Widgets/TileSelectedWidget.h"
#define LOCTEXT_NAMESPACE "CustomAssetActions"

#undef LOCTEXT_NAMESPACE
UCreateMapUIElements::UCreateMapUIElements()
{
	SupportedClasses.Add(UMapObject::StaticClass());
}

void UCreateMapUIElements::CreateMapDataWidgets(TSubclassOf<UUserWidget> DefaultWidgetForFields) const
{
	CreateMapDataWidgetsImpl(DefaultWidgetForFields);
}

void UCreateMapUIElements::CreateMapModeSelectorWidget(
	TSubclassOf<class UCustomButtonWidget> DefaultWidgetForButtons) const
{
	CreateMapModeSelectorWidgetImpl(DefaultWidgetForButtons);
}

UBlueprint* UCreateMapUIElements::CreateTileSelectedWidget(UBlueprint* LandDataBp, UBlueprint* OceanDataBp)
{
	const TArray<UObject *> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	UBlueprint *Blueprint = nullptr;
	
	for (UObject *Asset : SelectedAssets)
	{
		UMapObject* MapObject = Cast<UMapObject>(Asset);
		if (!MapObject)
			return Blueprint;
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());
		Blueprint = CreateTileSelectedWidget(MapObject, PackagePath, LandDataBp, OceanDataBp);
	}
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
	return Blueprint;
}

void UCreateMapUIElements::CreateMapHUDWidget(UBlueprint* TileSelectedWidgetBp, UBlueprint* MapModeSelectorWidgetBlueprint) 
{
	const TArray<UObject *> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	
	for (UObject *Asset : SelectedAssets)
	{
		UMapObject* MapObject = Cast<UMapObject>(Asset);
		if (!MapObject)
			return;
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());
		CreateMapHUDWidget(MapObject, PackagePath, TileSelectedWidgetBp, MapModeSelectorWidgetBlueprint);
		
	}
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
}

void UCreateMapUIElements::CreateAll(TSubclassOf<UUserWidget> DefaultWidgetForFields,
                                     TSubclassOf<class UCustomButtonWidget> DefaultWidgetForButtons) const
{
	const auto [BlueprintLand, BlueprintOcean] = CreateMapDataWidgetsImpl(DefaultWidgetForFields);
	UBlueprint* TileSelectedBp = CreateTileSelectedWidget(BlueprintLand, BlueprintOcean);
	UBlueprint* MapModeSelectBp = CreateMapModeSelectorWidgetImpl(DefaultWidgetForButtons);
	
	CreateMapHUDWidget(TileSelectedBp, MapModeSelectBp);
}

UCreateMapUIElements::DataWidgetsBps UCreateMapUIElements::CreateMapDataWidgetsImpl(TSubclassOf<UUserWidget> DefaultWidgetForFields)
{
	const TArray<UObject *> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	DataWidgetsBps DataWidgetsBlueprints(nullptr, nullptr);
	for (UObject *Asset : SelectedAssets)
	{
		const UMapObject* MapObject = Cast<UMapObject>(Asset);
		if (!MapObject)
			return DataWidgetsBlueprints;
		
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());
		DataWidgetsBlueprints.BlueprintLand = CreateWidgetForStruct(MapObject->StructType, PackagePath, DefaultWidgetForFields);
		DataWidgetsBlueprints.BlueprintOcean = CreateWidgetForStruct(MapObject->OceanStructType, PackagePath, DefaultWidgetForFields);
	}
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
	return DataWidgetsBlueprints;
}

UBlueprint* UCreateMapUIElements::CreateMapModeSelectorWidgetImpl(
	TSubclassOf<class UCustomButtonWidget> DefaultWidgetForButtons)
{
	const TArray<UObject *> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	UBlueprint *Blueprint = nullptr;
	for (UObject *Asset : SelectedAssets)
	{
		UMapObject* MapObject = Cast<UMapObject>(Asset);
		if (!MapObject)
			return nullptr;
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());
		Blueprint = CreateMapModeWidget(MapObject, PackagePath, DefaultWidgetForButtons);
	}
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
	return Blueprint;
}

UBlueprint* UCreateMapUIElements::CreateWidgetForStruct(UScriptStruct* Struct, const FString& PackagePath, TSubclassOf<UUserWidget> DefaultWidgetForFields)
{
	UStruct *AssetClass = Struct;
	// create widget map
	if (UPropGenWidgetMapDataAsset *WidgetMapDataAsset = UCreateWidgetFromAssetAction::CreateWidgetMapDataAsset(PackagePath, AssetClass->GetName()))
	{
		if (!DefaultWidgetForFields)
		{
			DefaultWidgetForFields = UWPropGenDoubleTextBlock::StaticClass();
		}

		WidgetMapDataAsset->CreateFromData(AssetClass, DefaultWidgetForFields);
		// create WBP_widget
		UE_LOG(LogTemp, Warning, TEXT("Create Widget from Asset succeeded on: %s"),  *AssetClass->GetName());
		return UCreateWidgetFromAssetAction::CreateBlueprintDerivedFromGenericStructWidget(PackagePath, AssetClass->GetName(), WidgetMapDataAsset);
	}
	return nullptr;
}

UBlueprint* UCreateMapUIElements::CreateMapModeWidget(UMapObject* MapObject, const FString& PackagePath,
	TSubclassOf<UCustomButtonWidget> DefaultWidgetForButtons)
{
	const UStruct *AssetClass = MapObject->GetClass();
	if (!DefaultWidgetForButtons)
	{
		DefaultWidgetForButtons = UCustomButtonWidget::StaticClass();
	}
	
	UBlueprint *Blueprint = UAtkAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(PackagePath, UMapModeSelectorWidget::StaticClass(),
																							 TEXT("/WBP_MapModeSelector_") + AssetClass->GetName());
	if (UMapModeSelectorWidget *DefaultObject = GetDefaultObject<UMapModeSelectorWidget>(Blueprint))
	{
		DefaultObject->MapModeSelectButton = DefaultWidgetForButtons;
		DefaultObject->MapObject = MapObject;
		DefaultObject->CreatePanelSlots();
	}
	return Blueprint;
}

UBlueprint* UCreateMapUIElements::CreateTileSelectedWidget(UMapObject* MapObject, const FString& PackagePath, UBlueprint* LandDataBp, UBlueprint* OceanDataBp)
{
	const UStruct *AssetClass = MapObject->GetClass(); 
	UBlueprint *Blueprint = UAtkAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(PackagePath, UTileSelectedWidget::StaticClass(),
																							 TEXT("/WBP_TileSelected_") + AssetClass->GetName());
	UTileSelectedWidget* DefaultObject = GetDefaultObject<UTileSelectedWidget>(Blueprint);
	if (!DefaultObject)
	{
		return nullptr;
	}
	UAtkWidgetEditorFunctionLibrary::CreateRootWidget<UVerticalBox>(DefaultObject, FName("MainPanel"));
	
	const UWPropGenGeneric* LandWidgetDefaultObject = GetDefaultObject<UWPropGenGeneric>(LandDataBp);
	const UWPropGenGeneric* OceanWidgetDefaultObject = GetDefaultObject<UWPropGenGeneric>(OceanDataBp);
	AddDataWidgetsToMain(DefaultObject, LandWidgetDefaultObject->GetClass(), OceanWidgetDefaultObject->GetClass());
	return Blueprint;
}

void UCreateMapUIElements::CreateMapHUDWidget(UMapObject* MapObject, const FString& PackagePath,
	UBlueprint* TileSelectedWidgetBp, UBlueprint* MapModeSelectorWidgetBlueprint)
{
	const UStruct *AssetClass = MapObject->GetClass(); 
	UBlueprint *Blueprint = UAtkAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(PackagePath, UGrandStrategyHUDWidget::StaticClass(),
																							 TEXT("/WBP_HUD_") + AssetClass->GetName());
	UGrandStrategyHUDWidget* DefaultObject = GetDefaultObject<UGrandStrategyHUDWidget>(Blueprint);
	if (!DefaultObject)
	{
		return;
	}
	
	UClass* TileSelectedWidgetClass = GetDefaultObject<UTileSelectedWidget>(TileSelectedWidgetBp)->GetClass();
	UClass* MapModeSelectorWidgetClass = GetDefaultObject<UMapModeSelectorWidget>(MapModeSelectorWidgetBlueprint)->GetClass();
	
	DefaultObject->CreateHudElements();
	DefaultObject->SeMapModeSelectorWidgetClass(MapModeSelectorWidgetClass);
	DefaultObject->SetTileDisplayWidgetClass(TileSelectedWidgetClass);
}

void UCreateMapUIElements::AddDataWidgetsToMain(class UTileSelectedWidget* TileSelectedWidget, TSubclassOf<UUserWidget> LandWidget, TSubclassOf<UUserWidget> OceanWidget)
{
	UVerticalBox* AssetGridPanel = Cast<UVerticalBox>(UAtkWidgetEditorFunctionLibrary::GetPanelWidget(TileSelectedWidget, FName("MainPanel")));
	UWidgetTree* MainAssetWidgetTree = UAtkWidgetEditorFunctionLibrary::GetWidgetTree(TileSelectedWidget);
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(TileSelectedWidget->GetClass()->ClassGeneratedBy);
	
	// Clean up GUIDs for widgets that will be removed and current children
	UAtkWidgetEditorFunctionLibrary::ClearChildren(WidgetBP, AssetGridPanel);

	// Create new widgets FIRST
	TArray<UWidget*> NewlyCreatedWidgets;
	// Land Data Widget
	if(UWPropGenGeneric* NewWidget = MainAssetWidgetTree->ConstructWidget<UWPropGenGeneric>(*LandWidget))
	{
		FString UniqueName("ProvinceDataWidget");
		NewWidget->Rename(*UniqueName, MainAssetWidgetTree, REN_DontCreateRedirectors);
		
		// Add to grid (this makes it part of the widget tree)
		AssetGridPanel->AddChildToVerticalBox(NewWidget);
		// Store for GUID registration
		NewlyCreatedWidgets.Add(NewWidget);
	}
	
	// Ocean data widget
	if(UWPropGenGeneric* NewWidget = MainAssetWidgetTree->ConstructWidget<UWPropGenGeneric>(*OceanWidget))
	{
		FString UniqueName("OceanTileDataWidget");
		NewWidget->Rename(*UniqueName, MainAssetWidgetTree, REN_DontCreateRedirectors);
		
		// Add to grid (this makes it part of the widget tree)
		AssetGridPanel->AddChildToVerticalBox(NewWidget);
		// Store for GUID registration
		NewlyCreatedWidgets.Add(NewWidget);
	}
    
	// Register GUIDs for all newly created widgets
	UAtkWidgetEditorFunctionLibrary::RegisterNewlyCreatedWidgets(NewlyCreatedWidgets, WidgetBP);
	
	AssetGridPanel->Modify();
	MainAssetWidgetTree->Modify();
	WidgetBP->Modify();
	UAtkWidgetEditorFunctionLibrary::MarkBlueprintAsModified(TileSelectedWidget);
}
