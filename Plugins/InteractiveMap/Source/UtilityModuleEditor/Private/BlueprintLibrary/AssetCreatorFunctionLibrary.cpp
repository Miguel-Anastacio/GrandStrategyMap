// Copyright 2024 An@stacioDev All rights reserved.
#if WITH_EDITOR
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "UtilityModuleEditor.h"
//#include "WidgetBlueprint.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Factories/TextureFromBufferFactory.h"
#include "Kismet2/BlueprintEditorUtils.h"

UObject* UAssetCreatorFunctionLibrary::CreateAsset(const FString& assetPath, UClass* assetClass, UFactory* factory,
                                                   bool& bOutSuccess, FString& OutInfoMessage)
{
	IAssetTools& AssetTools =FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

	UFactory* Factory = factory;
	if(!Factory)
	{
		for(UFactory* fac : AssetTools.GetNewAssetFactories())
		{
			if(fac->SupportedClass == assetClass)
			{
				Factory = fac;
				break;
			}
		}
	}
	if(!Factory)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Asset Failed - Was not able to find factory for Asset Class. '%s'"), *assetPath);
		return nullptr;
	}

	if(Factory->SupportedClass != assetClass)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Asset Failed - Factory cannot create desired Asset Class. '%s'"), *assetPath);
		return nullptr;
	}
	FString path = FPaths::GetPath(assetPath);
	UObject* Asset = AssetTools.CreateAsset(FPaths::GetBaseFilename(assetPath), FPaths::GetPath(assetPath), assetClass, Factory);
	if(!Asset)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Asset Failed - Either the path is invalid or the asset already exists '%s'"), *assetPath);
		return nullptr;
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Create Asset Successfully Created '%s'"), *assetPath);
	return Asset;
}

UTexture2D* UAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(const FString& AssetPath, TArray<uint8>& Data,
											uint32 Width, uint32 Height, bool& bOutSuccess, FString& OutInfoMessage)
{
	if(Data.IsEmpty())
	{
		bOutSuccess = false;
		UE_LOG(LogUtilityModuleEditor, Error, TEXT("Create Texture Asset From Array Failed: Array is empty"));
		return nullptr;
	}
	
	uint8* Buffer = Data.GetData();
	return CreateTextureAssetFromBuffer(AssetPath, Buffer, Width, Height, bOutSuccess, OutInfoMessage);
}

UTexture2D* UAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(const FString& AssetPath, uint8* Data,
	uint32 Width, uint32 Height, bool& bOutSuccess, FString& OutInfoMessage)
{
	if(!Data)
	{
		bOutSuccess = false;
		UE_LOG(LogUtilityModuleEditor, Error, TEXT("Create Texture Asset From Buffer Failed: Buffer is NULL"));
		return nullptr;
	}
	UTexture2DFromBufferFactory* Factory = NewObject<UTexture2DFromBufferFactory>();
	Factory->Height = Height;
	Factory->Width = Width;
	Factory->Buffer = Data;
	
	return Cast<UTexture2D>(CreateAsset(AssetPath, UTexture2D::StaticClass(), Factory, bOutSuccess, OutInfoMessage));
}

FString UAssetCreatorFunctionLibrary::CreateUniqueAssetNameInPackage(const FString& PackagePath, const FString& BaseAssetName,UClass* AssetClass)
{
	// Create a package to hold the new asset
	UPackage* Package = CreatePackage(*PackagePath);
	return MakeUniqueObjectName(Package, AssetClass, FName(*BaseAssetName)).ToString();
}

bool UAssetCreatorFunctionLibrary::SaveAsset(const FString& AssetPath, FString& OutInfoMessage)
{
	UObject* asset = StaticLoadObject(UObject::StaticClass(), nullptr, *AssetPath);
	if(!asset)
	{
		OutInfoMessage = FString::Printf(TEXT("Save Asset Failed - Asset is not valid '%s'"),*AssetPath);
		return false;
	}

	UPackage* package = asset->GetPackage();
	if(!package)
	{
		OutInfoMessage = FString::Printf(TEXT("Save Asset Failed - Package is not valid '%s'"),*AssetPath);
		return false;
	}

	bool result = UEditorLoadingAndSavingUtils::SavePackages({package}, false);
	OutInfoMessage = FString::Printf(TEXT("Save Asset %s - '%s'"), *FString(result ? "Success" : "Failed"), *AssetPath);
	return result;
}

TArray<UObject*> UAssetCreatorFunctionLibrary::GetModifiedAssets(bool& OutResult, FString& OutInfoMessage)
{
	TArray<UPackage*> ModifiedPackages = TArray<UPackage*>();
	FEditorFileUtils::GetDirtyPackages(ModifiedPackages);

	TArray<UObject*> assets = TArray<UObject*>();
	OutResult = true;
	OutInfoMessage = ("Get Modified Assets Succeeded");
	for(const UPackage* package : ModifiedPackages)
	{
		UObject* asset = package->FindAssetInPackage();
		if(asset)
		{
			assets.Add(asset);
		}
		else
		{
			if(OutResult)
			{
				OutResult = false;
				OutInfoMessage = FString::Printf(TEXT("Get Modified Assets Failed - Packages with invalid assets "));
			}
			OutInfoMessage += FString::Printf(TEXT("'%s', "), *package->GetName());
		}
	}

	return assets;
}

bool UAssetCreatorFunctionLibrary::SaveModifiedAssets(bool bPrompt, FString& OutInfoMessage)
{
	bool result = false;
	if(bPrompt)
	{
		result = UEditorLoadingAndSavingUtils::SaveDirtyPackagesWithDialog(true, true);
	}
	else
	{
		result = UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
	}
	return result;
}

void UAssetCreatorFunctionLibrary::MarkWidgetAsModified(const UObject* Object)
{
	// const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(Object->GetClass());
	// if (!WidgetBlueprintGeneratedClass)
	// 	return;
	// const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	// if (!Package)
	// 	return;
	// UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());
	// if (!MainAsset)
	// 	return;
	// MainAsset->Modify();
}

UGridPanel* UAssetCreatorFunctionLibrary::GetGridPanel(const UUserWidget* Widget, const FName& Name)
{
	// const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(Widget->GetClass());
	// const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	// UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());
	// if (!MainAsset)
	// {
	// 	return nullptr;
	// }
	// if(!MainAsset->WidgetTree)
	// {
	// 	return nullptr;
	// }
	// UGridPanel* AssetGridPanel = Cast<UGridPanel>(MainAsset->WidgetTree->FindWidget("MainPanel"));
	// return  AssetGridPanel;
	return nullptr;
}

void UAssetCreatorFunctionLibrary::MarkBlueprintAsModified(const UObject* Object)
{
	// const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(Object->GetClass());
	// if (!WidgetBlueprintGeneratedClass)
	// 	return;
	// const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	// if (!Package)
	// 	return;
	// UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());
	// if (!MainAsset)
	// 	return;
	// MainAsset->Modify();
	// FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainAsset);
}

class UWidgetTree* UAssetCreatorFunctionLibrary::GetWidgetTree(const UUserWidget* Widget)
{
	// const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(Widget->GetClass());
	// const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
	// UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());
	// if (!MainAsset)
	// {
	// 	return nullptr;
	// }
	// return MainAsset->WidgetTree;
	return nullptr;
}
#endif