call scons platform=windows tools=yes module_mono_enabled=yes mono_glue=no
call bin\godot.windows.tools.32.mono.exe --generate-mono-glue modules/mono/glue
call scons platform=windows tools=yes module_mono_enabled=yes mono_glue=yes
