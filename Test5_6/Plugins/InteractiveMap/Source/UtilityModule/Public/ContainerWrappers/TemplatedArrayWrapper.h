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
	
	void Add(const T& Value)
	{
		Array.Emplace(Value);
		if(DelegateAdd)
			DelegateAdd->Broadcast(Value);
	}
	
	void InsertAt(int32 Index, const T& Value)
	{
		if (Index >= 0 && Index <= Array.Num())
		{
			Array.Insert(Value, Index);
			if (DelegateAdd)
				DelegateAdd->Broadcast(Value);
		}
	}
	
	bool Pop()
	{
		if (!Array.IsEmpty())
		{
			T RemovedValue = Array.Last();
			Array.Pop();
			if (DelegateRemoved)
				DelegateRemoved->Broadcast(RemovedValue);
			return true;
		}
		return false;
	}
	
	bool Swap(const int32 FirstIndex, const int32 SecondIndex)
	{
		if (ValidIndex(FirstIndex) && ValidIndex(SecondIndex))
		{
			Array.Swap(FirstIndex, SecondIndex);
			if (DelegateSet)
				DelegateSet->Broadcast(Array);
			return true;
		}
		return false;
	}

	T* At(int Index)
	{
		if(ValidIndex(Index))
		{
			return &Array[Index];
		}
		return nullptr;
	}

	bool Remove(const T& Value)
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

			return true;
		}
		return false;
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

	bool SetAt(const int Index, const T& Value)
	{
		if(!ValidIndex(Index))
		{
			return false;
		}
		if(Value != Array[Index])
		{
			Array[Index] = Value;
			return true;
		}
		return false;
	}

	bool IsEmpty()
	{
		return Array.IsEmpty();
	}

	T& Last ()
	{
		return Array.Last();
	}
	
	bool ValidIndex(int32 Index) const
	{
		return Index >= 0 && Index < Array.Num();
	}
};
