// Fill out your copyright notice in the Description page of Project Settings.
#include "Map/MapUtils.h"

UE_DISABLE_OPTIMIZATION
int32 FProvinceIDData::HexToDecimal(const FString& hex, const TMap<TCHAR, int32>& HexMap)
{
    FString temp = hex.Reverse();
    auto chars = temp.GetCharArray();
    temp = temp.ToUpper();
    int32 total = 0;
    for (int i = 0; i < temp.Len(); i++)
    {
        const int32* value = HexMap.Find(temp[i]);
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
FColor FProvinceIDData::ConvertHexStringToRGB(const FString& color, const TMap<TCHAR, int32>& HexMap)
{
    if (color.StartsWith(FString("#")))
    {
        FString noPrefix = color.RightChop(1);
        FString r = noPrefix.LeftChop(6);
        FString g = noPrefix.Mid(2, 2);
        FString b = noPrefix.Mid(4, 2);
        FString alpha = noPrefix.RightChop(6);

        FColor ColorValue;
        ColorValue.R = HexToDecimal(r, HexMap);
        ColorValue.G = HexToDecimal(g, HexMap);
        ColorValue.B = HexToDecimal(b, HexMap);
        ColorValue.A = HexToDecimal(alpha, HexMap);

        return ColorValue;
    }

    UE_LOG(LogTemp, Warning, TEXT("Color is not hexadecimal format"));
    return FColor();
}
UE_ENABLE_OPTIMIZATION

void FProvinceData::PrintProvinceData()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, ProvinceName);
        GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, Owner.ToString());
        GEngine->AddOnScreenDebugMessage(2, 2.0f, FColor::Green, FString::Printf(TEXT("%lld"), Population));
    }
}
