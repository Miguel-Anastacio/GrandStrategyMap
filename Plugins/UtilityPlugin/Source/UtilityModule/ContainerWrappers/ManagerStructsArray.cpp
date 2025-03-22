// Copyright 2024 An@stacioDev All rights reserved.
#include "ContainerWrappers/ManagerStructsArray.h"

UTkManagerStructsArray::UTkManagerStructsArray(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTkManagerStructsArray::PostInitProperties()
{
	UObject::PostInitProperties();
	SetDelegates(OnStructAdded, OnStructRemoved, OnArraySet, OnArrayCleared);
}

void UTkManagerStructsArray::Add_BP(const FInstancedStruct& DataStruct)
{
	Add(DataStruct);
}

void UTkManagerStructsArray::AddMultiple_BP(const TArray<FInstancedStruct>& DataStructs)
{
	AddMultiple(DataStructs);
}

void UTkManagerStructsArray::Remove_BP(const FInstancedStruct& DataStruct)
{
	Remove(DataStruct);
}

void UTkManagerStructsArray::Clear_BP()
{
	Clear();
}

TArray<FInstancedStruct> UTkManagerStructsArray::GetArray_BP() const
{
	return Get();
}

void UTkManagerStructsArray::SetArray_BP(const TArray<FInstancedStruct>& NewStructs)
{
	Set(NewStructs);
}
