# Intel 8080 Space Invaders Emulator

An Emulator for The Original 1978 Space Invaders Arcade Machine.
Written in C using SDL2 for renderering and imgui for a GUI.

A Windows binary is available for download on the releases page. See: [Releases](https://github.com/tommojphillips/Space-Invaders/releases)

---

 - [Input](#input)
 - [Supported ROMSETS](#supported-romsets)
 - [Loading ROMSETS](#changing-romsets)
 - [Building](#building)


---

## Input

Both player 1 and player 2 controls are mapped to the same keys.

| Key         | Desc                       |
| ---         | ----------------------     |
| 1           | Player one start           |
| 2           | Player two start           |
| Space       | Fire                       |
| Left Arrow  | Left                       |
| Right Arrow | Right                      |
| 3 / C       | Insert Coin                |
| Esc         | Toggle Main Menu           |
| F11         | Full Screen                |
| P           | Pause/Unpause              |
| F5          | Save Machine State         |
| F9          | Load Machine State         |

---

## How to use
 This project uses the same names for the roms as the MAME project. Roms are loaded from a directory named after their MAME name. eg.

```
 Space-Invaders.exe

 invaders\
         | - invaders.e
         | - invaders.f
         | - invaders.g
         | - invaders.h
```

 --- 

### Supported ROMSETS:

| Romset name                 | MAME name    |
| --------------------------- | ------------ |
| `Space Invaders`            | invaders.zip |
| `Lunar Rescue`              | lrescue.zip  |
| `Balloon Bomber`            | ballbomb.zip |
| `Space Invaders pt2`        | invadpt2.zip | 
| `Space Laser`               | spclaser.zip |
| `Space Chaser`              | schaser.zip  |
| `Indian Battle`             | indianbt.zip |
| `Ozma Wars`                 | ozmawars.zip |
| `Galaxy Wars`               | galxwars.zip |
| `Galactic`                  | galactic.zip |


| Space Invaders | Lunar Rescue | Balloon Bomber |
| -                             | -                          |-                              | 
| ![invaders_470_540](https://github.com/user-attachments/assets/0b21da02-f76c-43a2-a58e-7476226d5608) | ![lrescue_470_540](https://github.com/user-attachments/assets/a9e98263-4d80-446d-8889-d1d01a51271f) | ![ballbomb_470_540](https://github.com/user-attachments/assets/acb352f5-e611-4457-995e-e511894af90a) |

| Space Invaders Pt2 | Space Laser | Space Chaser | 
| -                                 | -                          |   -                        | 
| ![invaderspt2_470_540](https://github.com/user-attachments/assets/cd913aad-b211-4de7-aabd-1fa5563c54d8)  | ![space_laser_470_540](https://github.com/user-attachments/assets/ba478b46-e1a0-4613-9188-1ce54bd4154e) | ![schaser_470_540](https://github.com/user-attachments/assets/df1bf496-edf3-4854-b0e3-066a70f81d7c) | 

### Changing ROMSETS:

 1.) Open the menu using `Esc`
 
 2.) Select the drop down menu and choose a romset.

![Screenshot 2025-02-05 205313](https://github.com/user-attachments/assets/72a5defc-9057-4d44-a6c2-195882728b04)

---

## Building

The project is built in Visual Studio 2022

#### Dependencies 
 - IMGUI v1.91.6 - https://github.com/ocornut/imgui/releases/tag/v1.91.6
 - SDL2 v2.30.10 - https://github.com/libsdl-org/SDL/releases/tag/release-2.30.10

---

  1. Clone the repo  
  
  ```
  git clone --recurse-submodules https://github.com/tommojphillips/Space-Invaders.git
  ```
  
  2. CD to `lib/` dir
  
  ```
  cd Space-Invaders/lib
  ```
    
  3. Clone `imgui` into `lib/` dir
  
  ``` 
  git clone --depth 1 --branch v1.91.6 https://github.com/ocornut/imgui.git
  ```
  
 4. Download SDL2 VC package [`SDL2-devel-X.X.X-VC.zip`](https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/SDL2-devel-2.30.10-VC.zip) (direct download link)
     - Extract and copy the `include` and `lib` folders into `Space-Invaders/lib/SDL2` you will have to create the `SDL2` directory

 5. Open `vs\Space-Invaders.sln` in visual studio and build and run
