call scons p=windows tools=yes vsproj=yes module_mono_enabled=yes mono_glue=no
call bin\godot.windows.tools.32.mono.exe --generate-mono-glue modules/mono/glue
call scons p=windows tools=yes module_mono_enabled=yes mono_glue=yes
call scons p=windows tools=no module_mono_enabled=yes mono_glue=yes
