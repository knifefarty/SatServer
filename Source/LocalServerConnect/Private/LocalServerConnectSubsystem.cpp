#include "LocalServerConnectSubsystem.h"
#include "LocalServerConnectButton.h"
#include "Patching/NativeHookManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

bool ALocalServerConnectSubsystem::bHookRegistered = false;

ALocalServerConnectSubsystem::ALocalServerConnectSubsystem()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ALocalServerConnectSubsystem::BeginPlay()
{
    Super::BeginPlay();

    // Skip hook registration on dedicated server instances
    if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
    {
        return;
    }

    RegisterMainMenuHook();
}

void ALocalServerConnectSubsystem::RegisterMainMenuHook()
{
    if (bHookRegistered)
    {
        return;
    }

    // Resolve Widget_MainMenu Blueprint Class
    UClass* MainMenuClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/FactoryGame/Interface/UI/Menu/MainMenu/Widget_MainMenu.Widget_MainMenu_C"));
    if (!MainMenuClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] Failed to locate Widget_MainMenu_C class."));
        return;
    }

    UFunction* ConstructFunc = MainMenuClass->FindFunctionByName(TEXT("Construct"));
    if (!ConstructFunc)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] Failed to locate Construct function on Widget_MainMenu_C."));
        return;
    }

    // Hook after Widget_MainMenu::Construct finishes
    SUBSCRIBE_METHOD_AFTER(ConstructFunc, [](UObject* Context, FFrame& Frame)
    {
        UUserWidget* MainMenu = Cast<UUserWidget>(Context);
        if (MainMenu)
        {
            InjectButtonIntoMainMenu(MainMenu);
        }
    });

    bHookRegistered = true;
    UE_LOG(LogTemp, Log, TEXT("[LocalServerConnect] Successfully hooked Widget_MainMenu::Construct."));
}

void ALocalServerConnectSubsystem::InjectButtonIntoMainMenu(UUserWidget* MainMenuWidget)
{
    if (!MainMenuWidget)
    {
        return;
    }

    // Locate the VerticalBox button container (mButtonBox in vanilla Satisfactory 1.0)
    UVerticalBox* ButtonBox = Cast<UVerticalBox>(MainMenuWidget->GetWidgetFromName(TEXT("mButtonBox")));
    if (!ButtonBox)
    {
        ButtonBox = Cast<UVerticalBox>(MainMenuWidget->GetWidgetFromName(TEXT("MenuButtons")));
    }
    if (!ButtonBox)
    {
        ButtonBox = Cast<UVerticalBox>(MainMenuWidget->GetWidgetFromName(TEXT("VerticalBox_Buttons")));
    }

    if (!ButtonBox)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LocalServerConnect] Could not find VerticalBox button container in Widget_MainMenu."));
        return;
    }

    // Check if the button is already injected to avoid duplicate buttons on reload
    for (UWidget* Child : ButtonBox->GetAllChildren())
    {
        if (Child && Child->IsA(ULocalServerConnectButton::StaticClass()))
        {
            return;
        }
    }

    // Load custom button widget class (or fallback to C++ base)
    TSubclassOf<ULocalServerConnectButton> ButtonClass = LoadClass<ULocalServerConnectButton>(nullptr, TEXT("/LocalServerConnect/UI/WBP_LocalConnectButton.WBP_LocalConnectButton_C"));
    if (!ButtonClass)
    {
        ButtonClass = ULocalServerConnectButton::StaticClass();
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(MainMenuWidget->GetWorld(), 0);
    ULocalServerConnectButton* NewButton = CreateWidget<ULocalServerConnectButton>(PC ? PC : MainMenuWidget->GetOwningPlayer(), ButtonClass);

    if (NewButton)
    {
        // Insert at Index 2 (directly below Continue [0] and Load Game [1])
        const int32 TargetSlotIndex = FMath::Clamp(2, 0, ButtonBox->GetChildrenCount());
        UVerticalBoxSlot* BoxSlot = Cast<UVerticalBoxSlot>(ButtonBox->InsertChildAt(TargetSlotIndex, NewButton));
        
        if (BoxSlot)
        {
            BoxSlot->SetPadding(FMargin(0.0f, 6.0f, 0.0f, 6.0f));
            BoxSlot->SetHorizontalAlignment(HAlign_Fill);
            BoxSlot->SetVerticalAlignment(VAlign_Center);
        }

        UE_LOG(LogTemp, Log, TEXT("[LocalServerConnect] Successfully injected button into Main Menu at index %d."), TargetSlotIndex);
    }
}
