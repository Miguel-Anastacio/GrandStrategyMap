// Copyright 2024 An@stacioDev All rights reserved.
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "Factories/TextureFromBufferFactory.h"

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

UObject* UAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(const FString& AssetPath, TArray<uint8>& Data,
											uint32 Width, uint32 Height, bool& bOutSuccess, FString& OutInfoMessage)
{
	uint8* buffer = Data.GetData();
	return CreateTextureAssetFromBuffer(AssetPath, buffer, Width, Height, bOutSuccess, OutInfoMessage);
}

UObject* UAssetCreatorFunctionLibrary::CreateTextureAssetFromBuffer(const FString& AssetPath, uint8* Data,
	uint32 Width, uint32 Height, bool& bOutSuccess, FString& OutInfoMessage)
{
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
	TArray<UPackage*> modifiedPackages = TArray<UPackage*>();
	FEditorFileUtils::GetDirtyPackages(modifiedPackages);

	TArray<UObject*> assets = TArray<UObject*>();
	OutResult = true;
	OutInfoMessage = ("Get Modified Assets Succeeded");
	for(const UPackage* package : modifiedPackages)
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
