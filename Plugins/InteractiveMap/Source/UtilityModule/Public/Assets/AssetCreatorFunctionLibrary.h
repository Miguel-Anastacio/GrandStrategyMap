// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"

#include "AssetCreatorFunctionLibrary.generated.h"

/**
 * 
 */
class UObject;
UCLASS()
class UTILITYMODULE_API UAssetCreatorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(Blueprintable, Category= "Asset Creation")
	static UObject* CreateAsset(const FString& AssetPath, UClass* assetClass, UFactory* factory, bool& bOutSuccess, FString& OutInfoMessage);
	
	UFUNCTION(Blueprintable, Category= "Asset Creation")
	static UObject* CreateTextureAssetFromBuffer(const FString& AssetPath, TArray<uint8>& Data, uint32 Width, uint32 Height,
											bool& bOutSuccess, FString& OutInfoMessage);
	
	static UObject* CreateTextureAssetFromBuffer(const FString& AssetPath, uint8* Data, uint32 Width, uint32 Height,
											bool& bOutSuccess, FString& OutInfoMessage);


	UFUNCTION(BlueprintCallable, Category = "Asset Creation")
	static FString CreateUniqueAssetNameInPackage(const FString& PackagePath, const FString& BaseAssetName,UClass* AssetClass);
	
};