## Building
```powershell
cmake --preset ninja-x64                                # configure
cmake --build --preset ninja-x64-release --target app   # build
.\build\release\app.exe                                 # run
```