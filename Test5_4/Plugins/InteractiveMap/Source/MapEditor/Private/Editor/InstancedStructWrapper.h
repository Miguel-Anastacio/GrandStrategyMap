// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 4, 4)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "InstancedStructWrapper.generated.h"
UCLASS()
class UInstancedStructWrapper : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = Data, EditAnywhere)
	FInstancedStruct StructInstance;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	void SetApp(TWeakPtr<class FMapEditorApp> appMode); 
	void SetStructInstance(const FInstancedStruct& Struct);
#endif
	
#if WITH_EDITORONLY_DATA
	int ID = -1;
private:	
	TWeakPtr<class FMapEditorApp> App;
#endif

	FInstancedStruct Previous;
};
