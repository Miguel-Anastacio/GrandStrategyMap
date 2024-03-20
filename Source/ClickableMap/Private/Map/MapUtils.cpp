// Fill out your copyright notice in the Description page of Project Settings.
#include "Map/MapUtils.h"

UE_DISABLE_OPTIMIZATION
int32 FProvinceIDData::HexToDecimal(const FString& hex)
{
    FString temp = hex.Reverse();
    auto chars = temp.GetCharArray();
    temp = temp.ToUpper();
    int32 total = 0;
    for (int i = 0; i < temp.Len(); i++)
    {
        int32* value = HexMap.Find(temp[i]);
        if (value)
        {
            total += FMath::Pow(16.f, i) * (*value);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid Hex value"));
        }
    }
    return total;
}
FColor FProvinceIDData::ConvertHexStringToRGB(FString color)
{
    color.RemoveAt(0, 1);
    FString r = color.LeftChop(6);
    FString g = color.Mid(2, 2);
    FString b = color.Mid(4, 2);
    FString alpha = color.RightChop(6);

    FColor ColorValue;
    ColorValue.R = HexToDecimal(r);
    ColorValue.G = HexToDecimal(g);
    ColorValue.B = HexToDecimal(b);
    ColorValue.A = HexToDecimal(alpha);
    
    return ColorValue;
}
UE_ENABLE_OPTIMIZATION

FProvinceData::FProvinceData(const FString& name, const FString& owner, int64 population)
    : Name(name), Owner(owner), Population(population)
{

}
