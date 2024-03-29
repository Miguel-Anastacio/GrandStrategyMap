// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Map/MapEnums.h"
#include "GrandStrategyHUDWidget.generated.h"

class UButtonWidget;
UCLASS(Abstract, BlueprintType)
class CLICKABLEMAP_API UGrandStrategyHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetGameMapReference();

protected:
	void NativeOnInitialized() override;
	UFUNCTION()
	void SetMapMode(UButtonWidget* button);

	UFUNCTION()
	void SaveProvinceDataToJson(UButtonWidget* button);
protected:	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<class UButtonWidget> PoliticalMapButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<class UButtonWidget> ReligiousMapButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<class UButtonWidget> CultureMapButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<class UButtonWidget> TerrainMapButton;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Buttons")
	TObjectPtr<class UButtonWidget> SaveDataButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString DirectoryPath = FString("D:\\Dev\\Unreal\\GrandStrategyData\\Spain\\");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FileSave")
	FString FileName = FString("save");

	//int FilesCreated;

	class AInteractiveMap* GameMap;
};
