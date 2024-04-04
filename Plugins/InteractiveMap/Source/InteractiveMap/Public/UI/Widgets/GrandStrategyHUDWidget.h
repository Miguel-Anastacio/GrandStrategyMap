// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Map/MapEnums.h"
#include "GrandStrategyHUDWidget.generated.h"

/**
 * This is the base HUD widget for the Grand Strategy map plugin.
 * It provides functionality to interact with the game map and save data.
 */
class UCustomButtonWidget;
UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UGrandStrategyHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetGameMapReference();

protected:
	void NativeOnInitialized() override;
	UFUNCTION()
	void SetMapMode(UCustomButtonWidget* button);

	UFUNCTION()
	void SaveDataToJson(UCustomButtonWidget* button);
protected:	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> PoliticalMapButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> ReligiousMapButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> CultureMapButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> TerrainMapButton;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> SaveDataButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<UCustomButtonWidget> SaveCountryButton;
	
	// Save Province Data File
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString DirectoryPath = FString("D:\\Dev\\Unreal\\GrandStrategyData\\Spain\\");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString FileName = FString("ProvinceCustom.json");
	// Save Country Editing File
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString FileNameCountry = FString("CountryCustom.json");

	class AInteractiveMap* GameMap;
};
