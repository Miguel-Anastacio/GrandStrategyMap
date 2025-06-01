// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "InstancedStruct.h"
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
	void SetAppMode(TWeakPtr<class FMapEditorDataAppMode> appMode);
	void SetStructInstance(const FInstancedStruct& Struct);
#endif
	
#if WITH_EDITORONLY_DATA
	int ID = -1;
private:	
	TWeakPtr<class FMapEditorDataAppMode> AppMode;
#endif

	FInstancedStruct Previous;
};
