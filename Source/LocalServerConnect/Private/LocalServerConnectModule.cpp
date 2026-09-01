#include "LocalServerConnectModule.h"
#include "LocalServerConnectSubsystem.h"

#define LOCTEXT_NAMESPACE "FLocalServerConnectModule"

void FLocalServerConnectModule::StartupModule()
{
#if !UE_SERVER
    ALocalServerConnectSubsystem::RegisterMainMenuHook();
    UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] StartupModule executed and Main Menu hook registered!"));
#endif
}

void FLocalServerConnectModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FLocalServerConnectModule, LocalServerConnect)
