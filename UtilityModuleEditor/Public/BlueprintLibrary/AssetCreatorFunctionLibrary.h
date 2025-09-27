// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "AssetCreatorFunctionLibrary.generated.h"

/*
 * Blueprint Function Library to support asset creation and management in the editor.
 * Provides static functions callable from both C++ and Blueprints.
 */
UCLASS(DisplayName="Asset Creator Function Library")
class UTILITYMODULEEDITOR_API UAtkAssetCreatorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/*
	 * Creates a new asset in the specified package path using a unique name based on a base name.
	 * Optionally uses a provided factory.
	 * @param PackagePath - Path where the asset should be created.
	 * @param AssetClass - The class type of the asset.
	 * @param BaseName - Base name to generate a unique asset name.
	 * @param Factory - Optional factory used to create the asset.
	 * @return The newly created asset or nullptr on failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "AssetCreator")
	static UObject* CreateAssetInPackageWithUniqueName(const FString& PackagePath, UClass* AssetClass, const FString& BaseName ,UFactory* Factory = nullptr);
	
	/*
	 * Creates an asset at a specified path.
	 * @param AssetPath - Full path where the asset should be created.
	 * @param AssetClass - Class type of the asset.
	 * @param Factory - Factory used to create the asset.
	 * @param bOutSuccess - Output parameter indicating success.
	 * @param OutInfoMessage - Output message for additional info or errors.
	 * @return The created asset or nullptr on failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "AssetCreator")
	static UObject* CreateAsset(const FString& AssetPath, UClass* AssetClass, UFactory* Factory, bool& bOutSuccess, FString& OutInfoMessage);
	
	/*
	 * Creates a UTexture2D asset from a buffer of raw image data.
	 * Version that accepts a TArray.
	 * @param AssetPath - Path where the texture should be saved.
	 * @param Data - Raw image data buffer.
	 * @param Width - Image width.
	 * @param Height - Image height.
	 * @param bOutSuccess - Indicates if creation was successful.
	 * @param OutInfoMessage - Additional info or error messages.
	 * @return Created texture or nullptr on failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "AssetCreator")
	static UTexture2D* CreateTextureAssetFromBuffer(const FString& PackagePath, const FString& AssetBaseName, TArray<uint8>& Data, int Width, int Height,
													bool& bOutSuccess, FString& OutInfoMessage);

	/*
	 * Overload of CreateTextureAssetFromBuffer that accepts a raw pointer to image data.
	 * @param AssetPath - Path for the texture asset.
	 * @param Data - Raw image data pointer.
	 * @param Width - Image width.
	 * @param Height - Image height.
	 * @param bOutSuccess - Indicates if creation succeeded.
	 * @param OutInfoMessage - Info or error message.
	 * @return Created texture or nullptr.
	 */
	static UTexture2D* CreateTextureAssetFromBuffer(const FString& PackagePath, const FString& AssetBaseName, uint8* Data, uint32 Width, uint32 Height,
													bool& bOutSuccess, FString& OutInfoMessage);

	/*
	 * Generates a unique asset name within a package.
	 * @param PackagePath - Path to the target package.
	 * @param BaseAssetName - Base name to start from.
	 * @param AssetClass - Class of the asset.
	 * @return A unique asset name that doesn't conflict with existing assets.
	 */
	static FString CreateUniqueAssetNameInPackage(const FString& PackagePath, const FString& BaseAssetName);

	/*
	 * Creates a Blueprint asset derived from a specified class.
	 * @param PackagePath - Where to save the Blueprint.
	 * @param Class - Parent class for the new Blueprint.
	 * @param AssetName - Name of the new Blueprint asset.
	 * @return The created UBlueprint object or nullptr on failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "AssetCreator")
	static UBlueprint* CreateBlueprintDerivedFromClass(const FString& PackagePath, UClass* Class,const FString& AssetName);

	/*
	 * Saves a specific asset to disk.
	 * @param AssetPath - Full path of the asset to save.
	 * @param OutInfoMessage - Output message with save status or errors.
	 * @return True if the save was successful.
	 */
	UFUNCTION(BlueprintCallable, Category = "AssetCreator")
	static bool SaveAsset(const FString& AssetPath, FString& OutInfoMessage);

	/*
	 * Gets all modified (unsaved) assets in the current session.
	 * @param OutResult - Indicates success of the operation.
	 * @param OutInfoMessage - Message containing result details.
	 * @return Array of modified assets.
	 */
	UFUNCTION(BlueprintCallable, Category = "AssetCreator")
	static TArray<UObject*> GetModifiedAssets(bool& OutResult, FString& OutInfoMessage);

	/*
	 * Saves all modified assets, optionally prompting the user.
	 * @param bPrompt - Whether to show a save prompt.
	 * @param OutInfoMessage - Save result details.
	 * @return True if assets were saved successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "AssetCreator")
	static bool SaveModifiedAssets(bool bPrompt, FString& OutInfoMessage);
};
