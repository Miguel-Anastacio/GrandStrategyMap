// Copyright 2024 An@stacioDev All rights reserved.
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#if WITH_EDITOR
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "UtilityModuleEditor.h"
#include "WidgetBlueprint.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Factories/TextureFromBufferFactory.h"
#include "Kismet2/BlueprintEditorUtils.h"
#endif

UObject *UAtkAssetCreatorFunctionLibrary::CreateAssetInPackageWithUniqueName(const FString &PackagePath,
																			 UClass *AssetClass, const FString &BaseName, UFactory *Factory)
{
#if WITH_EDITOR
	// Ensure the asset name is unique
	FString OutPackageName = BaseName;
	FString UniqueAssetName = BaseName;
	FAssetToolsModule &AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(PackagePath + BaseName, TEXT(""), OutPackageName, UniqueAssetName);

	// Create package
	UPackage *Package = CreatePackage(*OutPackageName);
	if (!Package)
	{
		return nullptr;
	}
	FString Message;
	bool bResult = false;
	return CreateAsset(OutPackageName, AssetClass, Factory, bResult, Message);
#else
	return nullptr;
#endif
}

UObject *UAtkAssetCreatorFunctionLibrary::CreateAsset(const FString &AssetPath, UClass *AssetClass, UFactory *Factory,
													  bool &bOutSuccess, FString &OutInfoMessage)
{
#if WITH_EDITOR
	IAssetTools &AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
	UFactory *Factory1 = Factory;
	if (!Factory1)
	{
		for (UFactory *fac : AssetTools.GetNewAssetFactories())
		{
			if (fac->SupportedClass == AssetClass)
			{
				Factory1 = fac;
				break;
			}
		}
	}
	if (!Factory1)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Asset Failed - Was not able to find factory for Asset Class. '%s'"), *AssetPath);
		return nullptr;
	}

	if (Factory1->SupportedClass != AssetClass)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Asset Failed - Factory cannot create desired Asset Class. '%s'"), *AssetPath);
		return nullptr;
	}
	FString path = FPaths::GetPath(AssetPath);
	UObject *Asset = AssetTools.CreateAsset(FPaths::GetBaseFilename(AssetPath), FPaths::GetPath(AssetPath), AssetClass, Factory1);
	if (!Asset)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Asset Failed - Either the path is invalid or the asset already exists '%s'"), *AssetPath);
		return nullptr;
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Create Asset Successfully Created '%s'"), *AssetPath);
	return Asset;
#else
	return nullptr;
#endif
}

UTexture2D *UAtkAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(const FString& PackagePath, const FString& AssetBaseName, TArray<uint8> &Data,
																		  int Width, int Height, bool &bOutSuccess, FString &OutInfoMessage)
{
#if WITH_EDITOR
	if (Data.IsEmpty())
	{
		bOutSuccess = false;
		UE_LOG(LogUtilityModuleEditor, Error, TEXT("Create Texture Asset From Array Failed: Array is empty"));
		return nullptr;
	}

	uint8 *Buffer = Data.GetData();
	return CreateTextureAssetFromBuffer(PackagePath, AssetBaseName, Buffer, Width, Height, bOutSuccess, OutInfoMessage);
#else
	return nullptr;
#endif
}

UTexture2D *UAtkAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(const FString& PackagePath, const FString& AssetBaseName, uint8 *Data,
																		  uint32 Width, uint32 Height, bool &bOutSuccess, FString &OutInfoMessage)
{
#if WITH_EDITOR
	if (!Data)
	{
		bOutSuccess = false;
		UE_LOG(LogUtilityModuleEditor, Error, TEXT("Create Texture Asset From Buffer Failed: Buffer is NULL"));
		return nullptr;
	}
	UTexture2DFromBufferFactory *Factory = NewObject<UTexture2DFromBufferFactory>();
	Factory->Height = Height;
	Factory->Width = Width;
	Factory->Buffer = Data;

	return Cast<UTexture2D>(CreateAssetInPackageWithUniqueName(PackagePath, UTexture2D::StaticClass(), AssetBaseName, Factory));
#else
	return nullptr;
#endif
}

FString UAtkAssetCreatorFunctionLibrary::CreateUniqueAssetNameInPackage(const FString &PackagePath, const FString &BaseAssetName)
{
#if WITH_EDITOR
	// Ensure the asset name is unique
	FString OutPackageName = BaseAssetName;
	FString UniqueAssetName = BaseAssetName;
	FAssetToolsModule &AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(PackagePath + BaseAssetName, TEXT(""), OutPackageName, UniqueAssetName);
	UPackage *Package = CreatePackage(*OutPackageName);
	return  UniqueAssetName;
#else
	return FString();
#endif
}

bool UAtkAssetCreatorFunctionLibrary::SaveAsset(const FString &AssetPath, FString &OutInfoMessage)
{
	bool result = false;
#if WITH_EDITOR
	UObject *asset = StaticLoadObject(UObject::StaticClass(), nullptr, *AssetPath);
	if (!asset)
	{
		OutInfoMessage = FString::Printf(TEXT("Save Asset Failed - Asset is not valid '%s'"), *AssetPath);
		return false;
	}

	UPackage *package = asset->GetPackage();
	if (!package)
	{
		OutInfoMessage = FString::Printf(TEXT("Save Asset Failed - Package is not valid '%s'"), *AssetPath);
		return false;
	}

	result = UEditorLoadingAndSavingUtils::SavePackages({package}, false);
	OutInfoMessage = FString::Printf(TEXT("Save Asset %s - '%s'"), *FString(result ? "Success" : "Failed"), *AssetPath);
#endif
	return result;
}

TArray<UObject *> UAtkAssetCreatorFunctionLibrary::GetModifiedAssets(bool &OutResult, FString &OutInfoMessage)
{
	TArray<UObject *> assets = TArray<UObject *>();
#if WITH_EDITOR
	TArray<UPackage *> ModifiedPackages = TArray<UPackage *>();
	FEditorFileUtils::GetDirtyPackages(ModifiedPackages);

	OutResult = true;
	OutInfoMessage = ("Get Modified Assets Succeeded");
	for (const UPackage *package : ModifiedPackages)
	{
		UObject *asset = package->FindAssetInPackage();
		if (asset)
		{
			assets.Add(asset);
		}
		else
		{
			if (OutResult)
			{
				OutResult = false;
				OutInfoMessage = FString::Printf(TEXT("Get Modified Assets Failed - Packages with invalid assets "));
			}
			OutInfoMessage += FString::Printf(TEXT("'%s', "), *package->GetName());
		}
	}
#endif
	return assets;
}

bool UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(bool bPrompt, FString &OutInfoMessage)
{
	bool result = false;
#if WITH_EDITOR
	if (bPrompt)
	{
		result = UEditorLoadingAndSavingUtils::SaveDirtyPackagesWithDialog(true, true);
	}
	else
	{
		result = UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
	}
#endif
	return result;
}

UBlueprint *UAtkAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(const FString &PackagePath, UClass *Class, const FString &AssetName)
{
#if WITH_EDITOR
	// Ensure the asset name is unique
	FString OutPackageName = AssetName;
	FString UniqueAssetName = AssetName;
	FAssetToolsModule &AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(PackagePath + AssetName, TEXT(""), OutPackageName, UniqueAssetName);

	// Create package
	UPackage *Package = CreatePackage(*OutPackageName);
	if (!Package)
	{
		return nullptr;
	}

	// Create the blueprint class derived from UGenericStructWidget
	UBlueprint *NewBlueprint = FKismetEditorUtilities::CreateBlueprint(
		Class,										   // Parent class
		Package,									   // Asset package
		*FPaths::GetBaseFilename(UniqueAssetName),	   // Asset name
		BPTYPE_Normal,								   // Normal Blueprint
		UWidgetBlueprint::StaticClass(),			   // Base Blueprint class
		UWidgetBlueprintGeneratedClass::StaticClass(), // Generated class type
		FName("CreateAsset")						   // Unique transaction name
	);

	if (NewBlueprint)
	{
		FAssetRegistryModule::AssetCreated(NewBlueprint);
		Package->MarkPackageDirty();
	}

	return NewBlueprint;
#else
	return nullptr;
#endif
}