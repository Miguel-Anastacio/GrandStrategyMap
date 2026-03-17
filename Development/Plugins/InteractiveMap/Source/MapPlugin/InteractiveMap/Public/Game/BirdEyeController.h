// Copyright 2024 An@stacioDev All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 4, 4)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "BirdEyeController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;

// Delegate signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProvinceClickedSignature, int, ProvinceID, FInstancedStruct, data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProvinceHoveredSignature, FColor, Color, int, ProvinceID);

/**
 * Base Controller class for interactive maps, handles input for mouse clicks and camera zoom
 */
UCLASS(Abstract, NotBlueprintable)
class INTERACTIVEMAP_API ABirdEyeController : public APlayerController
{
    GENERATED_BODY()

public:
    /** Default constructor. */
    ABirdEyeController();

    /** Mapping context for the controller. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext *DefaultMappingContext;

    /** Action for selecting objects. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction *SelectAction;

    /** Action for mouse scrolling (zoom). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction *MouseScrollAction;

#if WITH_EDITORONLY_DATA
    // Debug actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input| Debug", meta = (AllowPrivateAccess = "true"))
    UInputAction *DisplayLookupTextureAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input| Debug", meta = (AllowPrivateAccess = "true"))
    UInputAction *LogMapDataAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input| Debug", meta = (AllowPrivateAccess = "true"))
    UInputAction *LogMapLookupTableAction;
#endif

    // Delegate that is broadcast whenever the user clicks on a tile
    UPROPERTY(BlueprintAssignable)
    FOnProvinceClickedSignature ProvinceClickedDelegate;

    // Delegate that is broadcast whenever the user hovers tile
    UPROPERTY(BlueprintAssignable)
    FOnProvinceHoveredSignature ProvinceHoveredDelegate;

    // Delegate that is broadcast whenever the user clicks on the map but not in any tile
    UPROPERTY(BlueprintAssignable)
    FOnMapClickedSignature MapClickedDelegate;

protected:
    /** Sets up the input component. */
    virtual void SetupInputComponent() override;

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    /** Handles mouse click events. */
    UFUNCTION(BlueprintCallable, Category = "Input Actions")
    void MouseClick();

    /** Handles camera zoom events. */
    UFUNCTION(BlueprintCallable, Category = "Input Actions")
    void CameraZoom(const FInputActionInstance &Instance);

    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void HideHUD();

    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void ShowProvinceInfo(int Id, const FInstancedStruct &Data);

    UFUNCTION(BlueprintCallable, Category = "Input Reactions")
    void HighlightProvince(const FColor &Color);
#if WITH_EDITOR
    // Debug helper to display lookup texture at runtime
    UFUNCTION(BlueprintCallable, Category = "Input Reactions Debug")
    void SetLookupTextureActive();
    UFUNCTION(BlueprintCallable, Category = "Input Reactions Debug")
    void LogMapData();
    UFUNCTION(BlueprintCallable, Category = "Input Reactions Debug")
    void LogMapLookupTable();
#endif

    class AClickableMap *PerformLineTraceToFindMap(FHitResult &OutHit, bool &OutResultUnderCursor) const;

protected:
    /** Offset for mouse click on Z-axis. */
    UPROPERTY(EditAnywhere, Category = "Mouse Click")
    float zOffset = 300.0f;
    /** Define channel where the mouse traces are performed
     *   For the ClickableMap to work make sure that at least one of the map meshes blocks this channel
     */
    UPROPERTY(EditAnywhere, Category = "Mouse Click")
    TEnumAsByte<ECollisionChannel> MouseTraceChannel = ECC_Visibility;

    /** Flag indicating if a province is selected. */
    bool bProvinceSelected = false;

    /** Reference to the interactive map. */
    UPROPERTY(Transient, BlueprintReadOnly, Category="Map")
    class AClickableMap *Map;

    /** Reference to the map pawn. */
    UPROPERTY(Transient)
    class AMapPawn *MapPawn;
};
