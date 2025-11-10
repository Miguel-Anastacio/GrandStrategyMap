// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapModeSelectorWidget.generated.h"
class UCustomButtonWidget;
class UMapObject;
//
class UWCustomEditableText;

UCLASS(Abstract, BlueprintType)
class INTERACTIVEMAP_API UMapModeSelectorWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Sets the reference to the interactive map. */
    UFUNCTION(BlueprintCallable, Category = "MapModeSelectorWidget")
    void SetInteractiveMapReference(class AClickableMap* Map);
#if WITH_EDITOR
    UFUNCTION(CallInEditor, BlueprintCallable, Category = "MapModeSelectorWidget")
    void CreatePanelSlots() const;
    void CreateMainPanel() const;
#endif
public:
#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
    UMapObject* MapObject;

    UPROPERTY(EditAnywhere, Category = "MapModeSelectorWidget")
    TSubclassOf<UUserWidget> MapModeSelectButton;
#endif
    
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    UFUNCTION()
    void SetMapMode(UCustomButtonWidget* ButtonWidget);
protected:
    UPROPERTY(EditAnywhere, Category = "MapModeSelectorWidget")
    uint8 Columns = 5;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Grid Panel")
    class UGridPanel* GridPanel;
    
    /** Reference to the interactive map. */
    UPROPERTY()
    TWeakObjectPtr<class AClickableMap> GameMapReference;

    UPROPERTY()
    TMap<FName, UUserWidget*> MapModeButtonMapping;

};
