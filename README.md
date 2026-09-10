# Description
For this project I will be following a Software Rasteriser course by Pikuma's 3D Graphics Programming from Scratch. Since the origin was built
from a linux environment, using make files and the older SDL2 API. I will be recreating it using a more mordern and cross-platform approach.

### My Development Environment:
- **Platform:** Cross-platform  
- **Language:** C   
- **C standard:** C17  
- **Build system generator:** CMake  
- **CMake version:** 4.4  
- **CMake schema version:** 12  
- **CMake generator:** Ninja Multi-Config  
- **API:** SDL3  

## Building
```powershell
cmake --preset ninja-x64                                              # configure
cmake --build --preset ninja-x64-release --target app                 # build
.\build\release\app.exe                                               # run
```
