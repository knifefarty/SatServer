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

    // Pull IP from SML Config if available
    FString ConfiguredIP = ULocalServerConnectConfig::GetConfiguredServerIP();
    if (!ConfiguredIP.IsEmpty())
    {
        TargetIP = ConfiguredIP;
    }

    // Bind Button Click
    if (btn_Connect)
    {
        btn_Connect->SetIsFocusable(true);
        btn_Connect->OnClicked.RemoveAll(this);
        btn_Connect->OnClicked.AddDynamic(this, &ULocalServerConnectButton::HandleButtonClicked);
    }

    // Set Label Text
    if (txt_Label)
    {
        txt_Label->SetText(ButtonText);
    }

    // Ensure Focus Highlight starts hidden
    if (FocusHighlightBorder)
    {
        FocusHighlightBorder->SetVisibility(ESlateVisibility::Hidden);
    }
}

void ULocalServerConnectButton::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnAddedToFocusPath(InFocusEvent);

    // Show highlight indicator when navigated via Gamepad D-pad
    if (FocusHighlightBorder)
    {
        FocusHighlightBorder->SetVisibility(ESlateVisibility::Visible);
    }
}

void ULocalServerConnectButton::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnRemovedFromFocusPath(InFocusEvent);

    // Hide highlight indicator when focus leaves
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
    FString EffectiveIP = TargetIP.TrimStartAndEnd();
    if (EffectiveIP.IsEmpty())
    {
        EffectiveIP = TEXT("192.168.1.89");
    }

    FString Command = FString::Printf(TEXT("open %s"), *EffectiveIP);

    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    }

    if (PC)
    {
        UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), Command, PC);
    }
    else if (GEngine)
    {
        GEngine->Exec(GetWorld(), *Command);
    }
}

void ULocalServerConnectButton::SetTargetIP(const FString& InIP)
{
    TargetIP = InIP;
}
