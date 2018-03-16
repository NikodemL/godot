call scons platform=windows tools=yes module_mono_enabled=yes mono_glue=no
call bin\godot.windows.tools.32.mono.exe --generate-mono-glue modules/mono/glue
call scons platform=windows tools=yes module_mono_enabled=yes mono_glue=yes
call copy bin\godot.windows.tools.32.mono.exe ..\GodotBuild\
call copy bin\mono-2.0-sgen.dll ..\GodotBuild\
call copy bin\mscorlib.dll ..\GodotBuild\
call copy bin\GodotSharpTools.dll ..\GodotBuild\
