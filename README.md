# Intel 8080 Space Invaders Emulator

An Emulator for The Original 1978 Space Invaders Arcade Machine.
Written in C using SDL2 for renderering and imgui for a GUI.
The emulator is capable of runnning a few different arcade machines: Space Invaders, Lunar Rescue, Balloon Bomber, Space Invaders pt2, Space Laser

 - [Input](https://github.com/tommojphillips/Space-Invaders/master/README.md#input)
 - [Loading ROMSETS](https://github.com/tommojphillips/Space-Invaders/master/README.md#changing-romsets)
 - [Building](https://github.com/tommojphillips/Space-Invaders/master/README.md#building)

| Space Invaders - invaders.zip | Lunar Rescue - lrescue.zip | Balloon Bomber - ballbomb.zip | Space Invaders Pt2 - invadpt2.zip | Space Laser - spclaser.zip |
| -                             | -                          |-                              | -                                 |   -                        |
| ![invaders_470_540](https://github.com/user-attachments/assets/4dd6c515-b1f5-4632-9761-733ad9d67b46) | ![lrescue_470_540](https://github.com/user-attachments/assets/c25fc147-197a-47ec-a009-612686c928b1)  | ![ballbomb_470_540](https://github.com/user-attachments/assets/acb352f5-e611-4457-995e-e511894af90a) | ![invaderspt2_470_540](https://github.com/user-attachments/assets/cd913aad-b211-4de7-aabd-1fa5563c54d8)  | ![space_laser_470_540](https://github.com/user-attachments/assets/ba478b46-e1a0-4613-9188-1ce54bd4154e) |

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
| I           | Spawn Space Ship           |
| U           | Kill Space Ship            |
| F5          | Save Machine State         |
| F9          | Load Machine State         |

---

A Windows binary is available for download on the releases page. See: [Releases](https://github.com/tommojphillips/Space-Invaders/releases)

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

 lrescue\
         | - lrescue.1
         | - lrescue.2
         | - lrescue.3
         | - lrescue.4
         | - lrescue.5
         | - lrescue.6
```

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
  git clone https://github.com/tommojphillips/Space-Invaders.git
  ```
  
  2. CD to `lib/` dir
  
  ```
  cd Space-Invaders/lib
  ```
    
  3. Clone `imgui` into `lib/` dir
  
  ``` 
  git clone --depth 1 --branch v1.91.6 https://github.com/ocornut/imgui.git
  ```
  
  4. Clone `imgui-club` into `lib/` dir

  ```
  git clone https://github.com/ocornut/imgui_club.git
  ```
 
  5. Download SDL2 VC package [`SDL2-devel-X.X.X-VC.zip`](https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/SDL2-devel-2.30.10-VC.zip) (direct download link)
     - Extract and copy the `include` and `lib` folders into `Space-Invaders/lib/SDL2` you will have to create the `SDL2` directory

 6. Open `vc\Space-Invaders.sln` in visual studio and build and run

The project directory structure should look like this:

```
Space-Invaders\
      | -- lib\
      |        | -- i8080\
      |        |          | -- i8080.h
      |        |          | -- i8080.c
      |        |          | -- i8080_mnem.h
      |        |          | -- i8080_mnem.c
      |        |
      |        | -- imgui\
      |        |          | -- backends\
      |        |
      |        | -- imgui-club\
      |        |          | -- imgui_memory_editor\
      |        |
      |        | -- SDL2\
      |                  | -- include\
      |                  | -- lib\        
      | -- src\
      |
      | -- vc\
      |       | -- Space-Invaders.sln
      |       | -- Space-Invaders.vcxproj
```
