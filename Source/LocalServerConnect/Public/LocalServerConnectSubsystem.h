#pragma once

#include "CoreMinimal.h"
#include "Subsystem/ModSubsystem.h"
#include "LocalServerConnectSubsystem.generated.h"

/**
 * Mod Subsystem that handles startup registration and Main Menu UI Injection.
 */
UCLASS()
class LOCALSERVERCONNECT_API ALocalServerConnectSubsystem : public AModSubsystem
{
    GENERATED_BODY()

public:
    ALocalServerConnectSubsystem();

    virtual void BeginPlay() override;

protected:
    /** Register native hook on Widget_MainMenu::Construct */
    void RegisterMainMenuHook();

    /** Injects the connect button into the Main Menu Button Container */
    static void InjectButtonIntoMainMenu(UUserWidget* MainMenuWidget);

private:
    static bool bHookRegistered;
};
