#include "LocalServerConnectConfig.h"

ULocalServerConnectConfig::ULocalServerConnectConfig()
{
    ServerIP = TEXT("192.168.1.89");
}

FString ULocalServerConnectConfig::GetConfiguredServerIP()
{
    const ULocalServerConnectConfig* Config = GetDefault<ULocalServerConnectConfig>();
    return Config ? Config->ServerIP : TEXT("192.168.1.89");
}
