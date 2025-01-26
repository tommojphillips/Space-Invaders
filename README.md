## Intel 8080 Space Invaders Emulator

A Space Invaders Emulator for Windows written in C using SDL2 for renderering and imgui for a simple menu and debug windows.

---

#### Input

I've mapped both player 1 and player 2 controls to the same keys.

| Key         | Desc                       |
| ---         | ----------------------     |
| Esc         | Toggle Main Menu           |
| 1           | Player one start           |
| 2           | Player two start           |
| Space       | Fire                       |
| Left Arrow  | Left                       |
| Right Arrow | Right                      |
| 3           | Insert Coin                |
| C           | Insert Coin                |

 ---

### Dependencies 
 - IMGUI v1.91.6 - https://github.com/ocornut/imgui/releases/tag/v1.91.6
 - SDL2 v2.30.10 - https://github.com/libsdl-org/SDL/releases/tag/release-2.30.10
 - SDL2_Image v2.8.4 - https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.4

---

### Building

The project is built in Visual Studio 2022

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

  6. Download SDL2 Image VC package [`SDL2_Image-devel-X.X.X-VC.zip`](https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.4/SDL2_image-devel-2.8.4-VC.zip) (direct download link)
     - Extract the `include` and `lib` folders into `Space-Invaders/lib/SDL2_Image` you will have to create the `SDL2_Image` directory
  
  7. Open `vc\Space-Invaders.sln` in visual studio and build and run

The project directory structure should look like this:

```
Space-Invaders\
      | -- lib\
      |        | -- i8080\
      |        |              | -- i8080.h
      |        |              | -- i8080.c
      |        |              | -- i8080_defines.h
      |        |              | -- i8080_mnem.c
      |        |
      |        | -- imgui\
      |        |          | -- backends\
      |        |
      |        | -- imgui-club\
      |        |          | -- imgui_memory_editor\
      |        |
      |        | -- SDL2\
      |        |         | -- include\
      |        |         | -- lib\
      |        |
      |        | -- SDL2_Image\
      |                        | -- include\
      |                        | -- lib\
      |         
      | -- src\
      |
      | -- vc\
      |       | -- Space-Invaders.sln
      |       | -- Space-Invaders.vcxproj
```
