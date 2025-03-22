// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "TemplatedArrayWrapper.h"
#include "ManagerStructsArray.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructArrayChange, const FInstancedStruct&, Struct);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStructArraySet, const TArray<FInstancedStruct>&, Array);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStructArrayClear);

UCLASS(BlueprintType, Blueprintable, DisplayName=ManagerStructsArray)
class UTILITYMODULE_API UTkManagerStructsArray : public UObject, public TArrayWrapper<FInstancedStruct, FOnStructArrayChange, FOnStructArraySet, FOnStructArrayClear>
{
	GENERATED_BODY()
public:
	UTkManagerStructsArray(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintCallable, Category=ManagerStructsArray, DisplayName=Add)
	void Add_BP(const FInstancedStruct& DataStruct);

	UFUNCTION(BlueprintCallable, Category=ManagerStructsArray, DisplayName=AddMultiple)
	void AddMultiple_BP(const TArray<FInstancedStruct>& DataStructs);

	UFUNCTION(BlueprintCallable, Category=ManagerStructsArraym, DisplayName=Remove)
	void Remove_BP(const FInstancedStruct& DataStruct);

	UFUNCTION(BlueprintCallable, Category=ManagerStructsArray, DisplayName=Clear)
	void Clear_BP();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ManagerStructsArray, DisplayName=GetArray)
	TArray<FInstancedStruct> GetArray_BP() const;
	
	UFUNCTION(BlueprintCallable, Category=ManagerStructsArray)
	void SetArray_BP(const TArray<FInstancedStruct>& NewStructs);

	UPROPERTY(BlueprintAssignable, Category=ManagerStructsArray)
	FOnStructArrayChange OnStructAdded;

	UPROPERTY(BlueprintAssignable, Category=ManagerStructsArray)
	FOnStructArrayChange OnStructRemoved;
	
	UPROPERTY(BlueprintAssignable, Category=ManagerStructsArray)
	FOnStructArraySet OnArraySet;

	UPROPERTY(BlueprintAssignable, Category=ManagerStructsArray)
	FOnStructArrayClear OnArrayCleared;
	
};
