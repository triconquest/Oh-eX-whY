# [Oh-eX-whY](https://www.youtube.com/watch?v=qsjt7ejIusY)
Personal x64 CS:Source internal, since it has no use anymore, I decided to make it public.

DLL is available in Releases.

## Compilation instructions:
+ In Visual Studio 2022, set the language standard to C++20
+ Head to General -> Advanced and change the Character Set to **Use Multi-Byte Character Set**
+ Now in Linker -> System, change the subsystem to Windows.
+ In the Linker -> Input tab, add Additional Dependencies: **d3d9.lib, d3dx9.lib** (make sure to install the [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812))
+ Find Linker -> General -> Additional Lib Directories and add the absolute path to your *DirectX\Lib\x64*
