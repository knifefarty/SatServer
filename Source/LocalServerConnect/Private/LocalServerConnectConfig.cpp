#include "LocalServerConnectConfig.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/ConfigCacheIni.h"

ULocalServerConnectConfig::ULocalServerConnectConfig()
{
    ServerIP = TEXT("192.168.1.89");
}

FString ULocalServerConnectConfig::GetConfigFilePath()
{
    return FPaths::ProjectSavedDir() / TEXT("Config") / TEXT("Windows") / TEXT("LocalServerConnect.ini");
}

FString ULocalServerConnectConfig::GetConfiguredServerIP()
{
    FString SavedIP;
    FString ConfigPath = GetConfigFilePath();
    if (GConfig && GConfig->GetString(TEXT("LocalServerConnect"), TEXT("ServerIP"), SavedIP, ConfigPath))
    {
        if (!SavedIP.TrimStartAndEnd().IsEmpty())
        {
            return SavedIP.TrimStartAndEnd();
        }
    }

    // Check Fallback in FactoryGame/Configs/LocalServerConnect.cfg
    FString FallbackPath = FPaths::ProjectDir() / TEXT("FactoryGame/Configs/LocalServerConnect.cfg");
    if (FPaths::FileExists(FallbackPath))
    {
        FString FileContent;
        if (FFileHelper::LoadFileToString(FileContent, *FallbackPath))
        {
            FString Trimmed = FileContent.TrimStartAndEnd();
            if (!Trimmed.IsEmpty())
            {
                return Trimmed;
            }
        }
    }

    return TEXT("192.168.1.89");
}

void ULocalServerConnectConfig::SaveConfiguredServerIP(const FString& NewIP)
{
    FString CleanIP = NewIP.TrimStartAndEnd();
    if (CleanIP.IsEmpty())
    {
        CleanIP = TEXT("192.168.1.89");
    }

    FString ConfigPath = GetConfigFilePath();
    if (GConfig)
    {
        GConfig->SetString(TEXT("LocalServerConnect"), TEXT("ServerIP"), *CleanIP, ConfigPath);
        GConfig->Flush(false, ConfigPath);
    }

    // Also persist to FactoryGame/Configs/LocalServerConnect.cfg
    FString FallbackPath = FPaths::ProjectDir() / TEXT("FactoryGame/Configs/LocalServerConnect.cfg");
    FFileHelper::SaveStringToFile(CleanIP, *FallbackPath);
}
