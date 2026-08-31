# LocalServerConnect - Satisfactory Mod

A lightweight client-side Satisfactory mod (Satisfactory 1.0+ / SML v3.8+ / UE5.3) that injects a **"Connect to Local Host"** button into the vanilla Main Menu. It bypasses manual console IP entry and features native Gamepad D-pad navigation.

---

## 📁 Repository Structure

```text
Server Main Menu/
├── LocalServerConnect.uplugin       # Plugin descriptor & SML dependencies
├── README.md                        # Setup & deployment manual
└── Source/
    └── LocalServerConnect/
        ├── LocalServerConnect.Build.cs
        ├── Private/
        │   ├── LocalServerConnectButton.cpp     # Button logic & open <IP> execution
        │   ├── LocalServerConnectConfig.cpp     # Editable SML config implementation
        │   ├── LocalServerConnectModule.cpp     # Unreal Engine Module lifecycle
        │   └── LocalServerConnectSubsystem.cpp  # Main Menu hook & widget injection
        └── Public/
            ├── LocalServerConnectButton.h
            ├── LocalServerConnectConfig.h
            ├── LocalServerConnectModule.h
            └── LocalServerConnectSubsystem.h
```

---

## 🚀 How to Build & Install

### Method 1: Using the Satisfactory Mod Starter Project (C++)

1. **Clone the Starter Project**:
   Ensure you have the official [SatisfactoryModStarterProject](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/StarterProject/ObtainStarterProject.html) set up with Unreal Engine 5.3.
2. **Copy the Plugin**:
   Copy this `Server Main Menu` folder into your project's `Mods/` directory and rename the folder to `LocalServerConnect`:
   ```text
   SatisfactoryModStarterProject/
   └── Mods/
       └── LocalServerConnect/
           ├── LocalServerConnect.uplugin
           └── Source/
   ```
3. **Generate Visual Studio Project Files**:
   Right-click `FactoryGame.uproject` $\rightarrow$ **Generate Visual Studio project files**.
4. **Compile**:
   Open the solution in Visual Studio 2022 and build `Development Editor - Win64`.
5. **Package**:
   Use the SML Editor toolbar or Unreal Frontend to package the mod into a `.pak` / `.ucas` / `.utoc` distribution file.

---

## 🎮 Gamepad D-Pad Navigation

* The button sets `Is Focusable = true` and inherits from `UUserWidget`.
* Injection occurs into `mButtonBox` (the vanilla `UVerticalBox` holding `Continue`, `Load Game`, etc.) at slot **Index 2**.
* **Seamless Slate Chain**:
  $$\text{Continue [0]} \longleftrightarrow \text{Load Game [1]} \longleftrightarrow \textbf{Connect to Local Host [2]} \longleftrightarrow \text{Join Game / Server Manager [3]}$$
* Gamepad focus is visually indicated with the Pioneer Orange border highlight via `NativeOnAddedToFocusPath` / `NativeOnRemovedFromFocusPath`.

---

## ⚙️ Mod Configuration (Editable IP)

The IP address defaults to `192.168.1.89`, but is completely editable:

1. **In-Game**: Go to **Options** $\rightarrow$ **Mods** $\rightarrow$ **Local Server Connect** $\rightarrow$ Change **Server IP / Hostname**.
2. **Via Config File**: Edit `%LOCALAPPDATA%\FactoryGame\Saved\Config\Windows\LocalServerConnect.json`:
   ```json
   {
     "ServerIP": "192.168.1.89"
   }
   ```

---

## 🧩 Pure Blueprint Alternative

If you do not wish to compile C++, you can replicate the exact same logic using SML Blueprint Hooks:
1. Create a **Blueprint Hook Asset** named `Hook_MainMenu_LocalConnect`.
2. Target `Widget_MainMenu_C::Construct`.
3. In the hook function, call `Get Widget From Name ("mButtonBox")` $\rightarrow$ `Insert Child At (2, WBP_LocalConnectButton)`.
4. In `WBP_LocalConnectButton`, bind `OnClicked` to `Execute Console Command` with string `Append("open ", InIP)`.
