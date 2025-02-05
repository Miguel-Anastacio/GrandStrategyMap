// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#if WITH_EDITOR
#include "CoreMinimal.h"
#include "AssetCreatorFunctionLibrary.generated.h"

/**
 * 
 */
class UObject;
UCLASS()
class UTILITYMODULEEDITOR_API UAssetCreatorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(Blueprintable, Category= "Asset Creation")
	static UObject* CreateAsset(const FString& AssetPath, UClass* assetClass, UFactory* factory, bool& bOutSuccess, FString& OutInfoMessage);
	
	UFUNCTION(Blueprintable, Category= "Asset Creation")
	static UTexture2D* CreateTextureAssetFromBuffer(const FString& AssetPath, TArray<uint8>& Data, uint32 Width, uint32 Height,
											bool& bOutSuccess, FString& OutInfoMessage);
	
	static UTexture2D* CreateTextureAssetFromBuffer(const FString& AssetPath, uint8* Data, uint32 Width, uint32 Height,
											bool& bOutSuccess, FString& OutInfoMessage);

	UFUNCTION(BlueprintCallable, Category = "Asset Creation")
	static FString CreateUniqueAssetNameInPackage(const FString& PackagePath, const FString& BaseAssetName,UClass* AssetClass);

	UFUNCTION(BlueprintCallable, Category = "Asset Save")
	static bool SaveAsset(const FString& AssetPath, FString& OutInfoMessage);

	UFUNCTION(BlueprintCallable, Category = "Asset Save")
	static TArray<UObject*> GetModifiedAssets(bool& OutResult, FString& OutInfoMessage);

	UFUNCTION(BlueprintCallable, Category = "Asset Save")
	static bool SaveModifiedAssets(bool bPrompt, FString& OutInfoMessage);


	static void MarkWidgetAsModified(const UObject* Object);
	static class UGridPanel* GetGridPanel(const UUserWidget* Widget, const FName& Name);
	static class UWidgetTree* GetWidgetTree(const UUserWidget* Widget);

	static void MarkBlueprintAsModified(const UObject* Object);
	
};
#endif
