#include "LocalServerConnectConfig.h"
#include "Configuration/ConfigManager.h"

ULocalServerConnectConfig::ULocalServerConnectConfig()
{
    ModReference = TEXT("LocalServerConnect");
    DisplayName = TEXT("Local Server Connect");
    Description = TEXT("Configuration settings for LAN / Local Server quick connect.");
    ServerIP = TEXT("192.168.1.89");
}

FString ULocalServerConnectConfig::GetConfiguredServerIP()
{
    const ULocalServerConnectConfig* DefaultConfig = GetDefault<ULocalServerConnectConfig>();
    if (DefaultConfig && !DefaultConfig->ServerIP.IsEmpty())
    {
        return DefaultConfig->ServerIP;
    }
    return TEXT("192.168.1.89");
}
