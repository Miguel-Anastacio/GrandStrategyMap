// Copyright 2024 An@stacioDev All rights reserved.
#include "Assets/AssetCreatorFunctionLibrary.h"
#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "UtilityModule.h"
#include "Assets//TextureFromBufferFactory.h"

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

TArray<uint8> UAssetCreatorFunctionLibrary::ReadTextureToArray(UTexture2D* Texture)
{
	// Initialize an empty TArray to hold the texture data
	TArray<uint8> DataArray;
	// DataArray.reser
	if(!Texture)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Invalid texture!"));
		return DataArray;
	} 

	// Lock the texture for reading
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);

	// Copy the texture data into the TArray
	const uint8* Data = static_cast<const uint8*>(TextureData);
	DataArray.Append(Data, Mip.BulkData.GetBulkDataSize()); // Append data to the TArray

	// Unlock the texture
	Mip.BulkData.Unlock();

	return DataArray;
}

FColor UAssetCreatorFunctionLibrary::GetColorFromUV(uint32 Width, uint32 Height, const FVector2D& Uv,
	const TArray<uint8>& DataBuffer)
{
	//int32 Index = (y * Width + x) * 4;
	if(Width * Height * 4 != DataBuffer.Num())
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Buffer size does not match texture size"));
		return FColor();
	}
	const uint32 y = Uv.Y * Height;
	const uint32 x = Uv.X * Width;

	uint32 index = (y * Width + x) * 4;

	index = index % 4 + index;

	if (index > Width * Height * 4 - 4)
	{
		UE_LOG(LogUtilityModule, Warning, TEXT("Invalid index from uvs"));
		return FColor();
	}

	return FColor(DataBuffer[index+2],
		DataBuffer[index + 1],
		DataBuffer[index],
		DataBuffer[index + 3]);
}

FColor UAssetCreatorFunctionLibrary::GetColorFromUV(UTexture2D* texture, const FVector2D& uv, const TArray<uint8>& DataBuffer)
{
	if(!texture)
		return FColor();

	const int32 width = texture->GetSizeX();
	const int32 height = texture->GetSizeY();
	
	
	if(width * height * 4 != DataBuffer.Num())
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Buffer size does not match texture size"));
		return FColor();
	}

	//int32 Index = (y * Width + x) * 4;
	const int32 y = uv.Y * height;
	const int32 x = uv.X * width;

	int32 index = (y * width + x) * 4;

	index = index % 4 + index;

	if (index < 0 || index > width * height * 4 - 4)
	{
		UE_LOG(LogUtilityModule, Warning, TEXT("Invalid index from uvs"));
		return FColor();
	}
	

	return FColor(DataBuffer[index+2],
		DataBuffer[index + 1],
		DataBuffer[index],
		DataBuffer[index + 3]);
}

FColor UAssetCreatorFunctionLibrary::	GetColorFromUV(UTexture2D* Texture, const FVector2D& Uv, const uint8* DataBuffer)
{
	if(!Texture)
		return FColor();

	const int32 width = Texture->GetSizeX();
	const int32 height = Texture->GetSizeY();
	
	
	if(!DataBuffer)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Null Buffer"));
		return FColor();
	}

	//int32 Index = (y * Width + x) * 4;
	const int32 y = Uv.Y * height;
	const int32 x = Uv.X * width;

	int32 index = (y * width + x) * 4;

	index = index % 4 + index;

	if (index < 0 || index > width * height * 4 - 4)
	{
		UE_LOG(LogUtilityModule, Warning, TEXT("Invalid index from uvs"));
		return FColor();
	}
	
	// Data read from texture is in GBRA format
	return FColor(DataBuffer[index+1],
		DataBuffer[index + 2],
		DataBuffer[index],
		DataBuffer[index + 3]);
}



const uint8* UAssetCreatorFunctionLibrary::ReadTextureToBuffer(UTexture2D* Texture) 
{
	if(!Texture)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("Invalid texture!"));
		return nullptr;
	}
	if(Texture->CompressionSettings != TC_EditorIcon)
	{
		UE_LOG(LogUtilityModule, Error, TEXT("%s has wrong compression settings, please use UserInterface"), *Texture->GetName());
	}
	// Lock the texture for reading
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);
	const uint8* Data = static_cast<const uint8*>(TextureData);
	// Unlock the texture
	Mip.BulkData.Unlock();

	return  Data;
}
