// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "ManagerStructsArray.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTest, const FInstancedStruct&, Struct);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArrayTest, const TArray<FInstancedStruct>&, Array);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClearSignature);

template<typename T, typename DelegateChange, typename DelegateArraySet, typename DelegateArrayClear>
class TArrayWrapper
{
protected:
	TArray<T> Array;
	DelegateChange* DelegateAdd;
	DelegateChange* DelegateRemoved;
	DelegateArraySet* DelegateSet;
	DelegateArrayClear* DelegateClear;
	
public:
	TArrayWrapper() :
		DelegateAdd(nullptr),
		DelegateRemoved(nullptr),
		DelegateSet(nullptr),
		DelegateClear(nullptr)
	{}
	void SetDelegates(DelegateChange& AddDel, DelegateChange& RemoveDel,
				DelegateArraySet& SetDel, DelegateArrayClear& ClearDel)
	{
		DelegateAdd = &AddDel;
		DelegateRemoved = &RemoveDel;
		DelegateSet = &SetDel;
		DelegateClear = &ClearDel;
	}
	
protected:
	void Add(const T& Value)
	{
		Array.Add(Value);
		if(DelegateAdd)
			DelegateAdd->Broadcast(Value);
	}

	void Remove(const T& Value)
	{
		const int32 Index = Array.IndexOfByPredicate([Value](const T& Element)
		{
			return Element == Value;
		});
		
		if (Index != INDEX_NONE)
		{
			Array.RemoveAt(Index);
			if(DelegateRemoved)
				DelegateRemoved->Broadcast(Value);
		}
	}

	void Clear()
	{
		Array.Empty();
		if(DelegateClear)
			DelegateClear->Broadcast();
	}

	void Set(const TArray<T>& Value)
	{
		Array = Value;
		if(DelegateSet)
			DelegateSet->Broadcast(Value);
	}

};

UCLASS(BlueprintType, Blueprintable)
class UTILITYMODULE_API UTkManagerStructsArray : public UObject, public TArrayWrapper<FInstancedStruct, FOnTest, FOnArrayTest, FOnClearSignature>
{
	GENERATED_BODY()
public:
	UTkManagerStructsArray(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitProperties() override;
	UFUNCTION(BlueprintCallable, Category=ManagerStructsArray)
	void Add_BP(const FInstancedStruct& DataStruct);
	UFUNCTION(BlueprintCallable, Category=ManagerStructsArray)
	void Remove_BP(const FInstancedStruct& DataStruct);
	UFUNCTION(BlueprintCallable, Category=ManagerStructsArray)
	void Clear_BP();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=ManagerStructsArray)
	TArray<FInstancedStruct> GetArray() const;
	
	UFUNCTION(BlueprintCallable, Category=ManagerStructsArray)
	void SetArray_BP(const TArray<FInstancedStruct>& NewStructs);

	UPROPERTY(BlueprintAssignable, Category=ManagerStructsArray)
	FOnTest OnStructAdded;
	UPROPERTY(BlueprintAssignable, Category=ManagerStructsArray)
	FOnTest OnStructRemoved;
	UPROPERTY(BlueprintAssignable, Category=ManagerStructsArray)
	FOnArrayTest OnArraySet;
	UPROPERTY(BlueprintAssignable, Category=ManagerStructsArray)
	FOnClearSignature OnArrayCleared;
	
};
