#include "LocalServerConnectButton.h"
#include "LocalServerConnectConfig.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/UnrealType.h"

ULocalServerConnectButton::ULocalServerConnectButton(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ButtonText = FText::FromString(TEXT("Connect to Local Host"));
    TargetIP = TEXT("192.168.1.89");
    SetIsFocusable(true);
}

TSharedRef<SWidget> ULocalServerConnectButton::RebuildWidget()
{
    if (!WidgetTree)
    {
        WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
    }

    if (WidgetTree && !WidgetTree->RootWidget)
    {
        UClass* FEButtonClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/FactoryGame/Interface/UI/Menu/Widget_FrontEnd_Button.Widget_FrontEnd_Button_C"));
        if (FEButtonClass)
        {
            ChildFrontEndButton = WidgetTree->ConstructWidget<UUserWidget>(FEButtonClass, TEXT("FE_LocalConnectBtn"));
            if (ChildFrontEndButton)
            {
                WidgetTree->RootWidget = ChildFrontEndButton;

                UFunction* SetTitleFunc = ChildFrontEndButton->FindFunction(FName(TEXT("SetTitle")));
                if (SetTitleFunc)
                {
                    struct FSetTitleParams { FText Title; };
                    FSetTitleParams Params;
                    Params.Title = ButtonText;
                    ChildFrontEndButton->ProcessEvent(SetTitleFunc, &Params);
                }

                UFunction* SetBigFunc = ChildFrontEndButton->FindFunction(FName(TEXT("SetIsBigButton")));
                if (SetBigFunc)
                {
                    struct FSetBigParams { bool bIsBig; };
                    FSetBigParams Params;
                    Params.bIsBig = true;
                    ChildFrontEndButton->ProcessEvent(SetBigFunc, &Params);
                }
            }
        }

        if (!WidgetTree->RootWidget)
        {
            UButton* FallbackBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("FallbackBtn"));
            UTextBlock* FallbackTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FallbackTxt"));
            if (FallbackTxt)
            {
                FallbackTxt->SetText(ButtonText);
                FallbackTxt->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 20));
                FallbackTxt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            }
            if (FallbackBtn)
            {
                if (FallbackTxt)
                {
                    FallbackBtn->AddChild(FallbackTxt);
                }
                WidgetTree->RootWidget = FallbackBtn;
            }
        }
    }

    return Super::RebuildWidget();
}

void ULocalServerConnectButton::NativeConstruct()
{
    Super::NativeConstruct();

    FString ConfiguredIP = ULocalServerConnectConfig::GetConfiguredServerIP();
    if (!ConfiguredIP.IsEmpty())
    {
        TargetIP = ConfiguredIP;
    }

    if (ChildFrontEndButton)
    {
        // Hook click event via Unreal multicast delegate property reflection
        if (FMulticastDelegateProperty* DelegateProp = CastField<FMulticastDelegateProperty>(ChildFrontEndButton->GetClass()->FindPropertyByName(FName(TEXT("OnClicked")))))
        {
            FScriptDelegate Delegate;
            Delegate.BindUFunction(this, FName(TEXT("HandleButtonClicked")));
            DelegateProp->AddDelegate(Delegate, ChildFrontEndButton);
        }
    }
    else if (UButton* FallbackBtn = Cast<UButton>(WidgetTree ? WidgetTree->RootWidget : nullptr))
    {
        FallbackBtn->OnClicked.RemoveAll(this);
        FallbackBtn->OnClicked.AddDynamic(this, &ULocalServerConnectButton::HandleButtonClicked);
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
