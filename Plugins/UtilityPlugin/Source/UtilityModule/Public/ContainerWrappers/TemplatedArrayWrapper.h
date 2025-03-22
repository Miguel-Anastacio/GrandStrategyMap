// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CoreMinimal.h"
/**
 * Wrapper for an array that provides bindable events when the array is modified
 */
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
		Array.Emplace(Value);
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

	TArray<T> Get() const
	{
		return Array;
	}

	void AddMultiple(const TArray<T>& Value)
	{
		Array.Append(Value);
		if(DelegateSet)
			DelegateSet->Broadcast(Array);
	}
};
