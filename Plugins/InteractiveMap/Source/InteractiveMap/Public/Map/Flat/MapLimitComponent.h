// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MapLimitComponent.generated.h"
/**
 * Component for managing map limits.
 */
class UBoxComponent;
class UMapVisualComponent;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class INTERACTIVEMAP_API UMapLimitComponent : public USceneComponent
{
    GENERATED_BODY()

public:

 /*   template<typename ReturnType, typename ClassToCreateByDefault>
    ReturnType* CreateDefaultSubobjectHelper(FName SubobjectFName, UClass* classToCreateByDefault, bool bIsRequired, bool bIsTransient) {
        return CreateDefaultSubobject<ReturnType>(SubobjectFName, ReturnType::StaticClass(), classToCreateByDefault, bIsRequired, bIsTransient);
    }*/
    /** Default constructor. */
    UMapLimitComponent();

    /** Initializes the limit component. */
    UFUNCTION(BlueprintCallable, Category = "Limit")
    void InitLimitComponent(UStaticMeshComponent* mapSelectMesh, UStaticMeshComponent* mapBorder,
        UStaticMeshComponent* gameplayMap, UStaticMeshComponent* terrainMap);

    /** Gets the visual component. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Visual")
    class UMapVisualComponent* GetVisualComponent();

    void Attach(USceneComponent* root);
    FORCEINLINE UBoxComponent* GetBoxComponent() { return Box; };

    void CreateVisualComponent(TSubclassOf<UMapVisualComponent> mapVisualClass);

protected:
    /** Called when the game starts. */
    virtual void BeginPlay() override;

    /** Called when overlap begins. */
    UFUNCTION(Category = "Overlap")
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:

    /** Box component for the limit. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limit")
    TObjectPtr<UBoxComponent> Box;

    UPROPERTY(EditDefaultsOnly, Category = "Limit")
    TSubclassOf<UMapVisualComponent> VisualComponentClass;

    /** Map visual component. */
    UPROPERTY(EditAnywhere, Category = "Visual", BlueprintReadOnly)
    TObjectPtr<UMapVisualComponent> MapVisualComponent;
};