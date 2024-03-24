// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "ManagerHUD.generated.h"

/**
 * 
 */
struct FProvinceData;
class UGrandStrategyHUDWidget;
class UProvinceEditorWidget;
UCLASS()
class CLICKABLEMAP_API AManagerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void DisplayProvinceEditorWidget(const FProvinceData& provinceData, FName id);
	UFUNCTION(BlueprintCallable)
	void SetProvinceEditorVisibility(ESlateVisibility visibility);

	UFUNCTION(BlueprintCallable)
	void SetInteractiveMapReference(class AInteractiveMap* map);

protected:
	void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UGrandStrategyHUDWidget> HudWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UProvinceEditorWidget> ProvincedEditorWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UGrandStrategyHUDWidget> HudWidget;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UProvinceEditorWidget> ProvinceEditorWidget;

	class AInteractiveMap* GameMapReference;
};
