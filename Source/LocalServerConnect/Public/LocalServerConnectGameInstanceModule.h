#pragma once

#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "LocalServerConnectGameInstanceModule.generated.h"

/**
 * Root Game Instance Module for LocalServerConnect.
 * Automatically discovered by SML to register Widget Blueprint Hooks on boot.
 */
UCLASS()
class LOCALSERVERCONNECT_API ULocalServerConnectGameInstanceModule : public UGameInstanceModule
{
    GENERATED_BODY()

public:
    ULocalServerConnectGameInstanceModule();

    virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;
};
