# Intel 8080 Space Invaders Emulator

An Emulator for The Original 1978 Space Invaders Arcade Machine. Written in C using SDL2 for the renderering.

 - [Input](#input)
 - [Usage](#usage)
 - [Building](#building)
 - [Screenshots](#screenshots)

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
| F11         | Full Screen                |
| P           | Pause/Unpause              |
| F5          | Save Machine State         |
| F9          | Load Machine State         |

---

## Usage

 This project uses the same names for the roms as the MAME project. Roms are loaded from a directory named after their `MAME` name.

Syntax:
```
Space-Invaders.exe <romset>
```

| Options | Desc                        |
| ------- | --------------------------- |
| `-l`    | List all supported romsets  |
| `-f`    | Launch romset in fullscreen |

| romset                       | MAME        |
| -------------------------    | ----------- |
| Space Invaders               | `invaders`  |
| Lunar Rescue                 | `lrescue`   |
| Balloon Bomber               | `ballbomb`  |
| Space Invaders Part II       | `invadpt2`  |
| Space Laser                  | `spclaser`  |
| Space Chaser (set 1)         | `schaser`   |
| Indian Battle                | `indianbt`  |
| Ozma Wars (set 1)            | `ozmawars`  |
| Galaxy Wars (set 1)          | `galxwars`  |
| Galactic  - Batalha Espacial | `galactic`  |

## Building

The project is built in Visual Studio 2022

| Dependencies   |                                                                |
| -------------- | -------------------------------------------------------------- |
 | I8080         | https://github.com/tommojphillips/i8080                        |
 | SDL2 v2.30.10 | https://github.com/libsdl-org/SDL/releases/tag/release-2.30.10 |

---

1. Clone the repo and submodules
  
  ```
  git clone --recurse-submodules https://github.com/tommojphillips/Space-Invaders.git
  ```
    
 2. Download SDL2 VC package [`SDL2-devel-X.X.X-VC.zip`](https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/SDL2-devel-2.30.10-VC.zip) (direct download link)
     - Extract and copy the `include` and `lib` folders into `Space-Invaders/lib/SDL2` you will have to create the `SDL2` directory.

 3. Open `vc\Space-Invaders.sln`, build and run.

## Screenshots

| Space Invaders | Lunar Rescue | Balloon Bomber |
| -                             | -                          |-                              | 
| ![invaders_470_540](https://github.com/user-attachments/assets/0b21da02-f76c-43a2-a58e-7476226d5608) | ![lrescue_470_540](https://github.com/user-attachments/assets/a9e98263-4d80-446d-8889-d1d01a51271f) | ![ballbomb_470_540](https://github.com/user-attachments/assets/acb352f5-e611-4457-995e-e511894af90a) |

| Space Invaders Pt2 | Space Laser | Space Chaser | 
| -                                 | -                          |   -                        | 
| ![invaderspt2_470_540](https://github.com/user-attachments/assets/cd913aad-b211-4de7-aabd-1fa5563c54d8)  | ![space_laser_470_540](https://github.com/user-attachments/assets/ba478b46-e1a0-4613-9188-1ce54bd4154e) | ![schaser_470_540](https://github.com/user-attachments/assets/df1bf496-edf3-4854-b0e3-066a70f81d7c) | 
