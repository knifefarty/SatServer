#include "LocalServerConnectButton.h"
#include "LocalServerConnectConfig.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ULocalServerConnectButton::ULocalServerConnectButton(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ButtonText = FText::FromString(TEXT("Connect to Local Host"));
    TargetIP = TEXT("192.168.1.89");
    SetIsFocusable(true);
}

void ULocalServerConnectButton::NativeConstruct()
{
    Super::NativeConstruct();

    FString ConfiguredIP = ULocalServerConnectConfig::GetConfiguredServerIP();
    if (!ConfiguredIP.IsEmpty())
    {
        TargetIP = ConfiguredIP;
    }

    if (btn_Connect)
    {
        btn_Connect->IsFocusable = true;
        btn_Connect->OnClicked.RemoveAll(this);
        btn_Connect->OnClicked.AddDynamic(this, &ULocalServerConnectButton::HandleButtonClicked);
    }

    if (txt_Label)
    {
        txt_Label->SetText(ButtonText);
    }

    if (FocusHighlightBorder)
    {
        FocusHighlightBorder->SetVisibility(ESlateVisibility::Hidden);
    }
}

void ULocalServerConnectButton::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnAddedToFocusPath(InFocusEvent);

    if (FocusHighlightBorder)
    {
        FocusHighlightBorder->SetVisibility(ESlateVisibility::Visible);
    }
}

void ULocalServerConnectButton::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnRemovedFromFocusPath(InFocusEvent);

    if (FocusHighlightBorder)
    {
        FocusHighlightBorder->SetVisibility(ESlateVisibility::Hidden);
    }
}

void ULocalServerConnectButton::HandleButtonClicked()
{
    ExecuteServerConnect();
}

void ULocalServerConnectButton::ExecuteServerConnect()
{
    ConnectToConfiguredServer(this);
}

void ULocalServerConnectButton::ConnectToConfiguredServer(UObject* WorldContextObject)
{
    FString EffectiveIP = ULocalServerConnectConfig::GetConfiguredServerIP();
    if (EffectiveIP.TrimStartAndEnd().IsEmpty())
    {
        EffectiveIP = TEXT("192.168.1.89");
    }

    FString Command = FString::Printf(TEXT("open %s"), *EffectiveIP);

    UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
    APlayerController* PC = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;

    if (PC && World)
    {
        UKismetSystemLibrary::ExecuteConsoleCommand(World, Command, PC);
    }
    else if (GEngine)
    {
        GEngine->Exec(World, *Command);
    }
}

void ULocalServerConnectButton::SetTargetIP(const FString& InIP)
{
    TargetIP = InIP;
}
