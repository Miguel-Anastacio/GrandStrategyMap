// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 4, 4)
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "TemplatedArrayWrapper.h"
#include "ManagerStructsArray.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructArrayChange, const FInstancedStruct &, Struct);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructArraySet, const TArray<FInstancedStruct> &, Array);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStructArrayClear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStructChanged, const FInstancedStruct &, OldData, const FInstancedStruct &, NewData);

UCLASS(BlueprintType, Blueprintable, DisplayName = ManagerStructsArray)
class UTILITYMODULE_API UTkManagerStructsArray : public UObject
{
	GENERATED_BODY()
public:
	UTkManagerStructsArray(const FObjectInitializer &ObjectInitializer);
	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintCallable, Category = ManagerStructsArray, DisplayName = Add)
	void Add_BP(const FInstancedStruct &DataStruct);

	UFUNCTION(BlueprintCallable, Category = ManagerStructsArray, DisplayName = AddMultiple)
	void AddMultiple_BP(const TArray<FInstancedStruct> &DataStructs);

	UFUNCTION(BlueprintCallable, Category = ManagerStructsArraym, DisplayName = Remove)
	void Remove_BP(const FInstancedStruct &DataStruct);

	UFUNCTION(BlueprintCallable, Category = ManagerStructsArray, DisplayName = Clear)
	void Clear_BP();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = ManagerStructsArray, DisplayName = GetArray)
	TArray<FInstancedStruct> GetArray_BP() const;

	UFUNCTION(BlueprintCallable, Category = ManagerStructsArray, DisplayName = SetArray)
	void SetArray_BP(const TArray<FInstancedStruct> &NewStructs);
	UFUNCTION(BlueprintCallable, Category = ManagerStructsArray, DisplayName = At)
	FInstancedStruct &At_BP(const int Index);

	UFUNCTION(BlueprintCallable, Category = ManagerStructsArray, DisplayName = SetAt)
	void SetAt(const int Index, const FInstancedStruct &NewStruct);
	UPROPERTY(BlueprintAssignable, Category = ManagerStructsArray)
	FOnStructArrayChange OnStructAdded;

	UPROPERTY(BlueprintAssignable, Category = ManagerStructsArray)
	FOnStructArrayChange OnStructRemoved;

	UPROPERTY(BlueprintAssignable, Category = ManagerStructsArray)
	FOnStructArraySet OnArraySet;

	UPROPERTY(BlueprintAssignable, Category = ManagerStructsArray)
	FOnStructArrayClear OnArrayCleared;

	UPROPERTY(BlueprintAssignable, Category = ManagerStructsArray)
	FOnStructChanged OnStructChanged;

protected:
	TArrayWrapper<FInstancedStruct, FOnStructArrayChange, FOnStructArraySet, FOnStructArrayClear> ArrayWrapper;
};
