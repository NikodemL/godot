# config.py

def can_build(platform):
    return True

def configure(env):
    env.Append(LINKFLAGS=['modules/ib_video/ffmpeg/win32/lib/avcodec.lib',
                          'modules/ib_video/ffmpeg/win32/lib/avdevice.lib',
                          'modules/ib_video/ffmpeg/win32/lib/avfilter.lib',
                          'modules/ib_video/ffmpeg/win32/lib/avformat.lib',
                          'modules/ib_video/ffmpeg/win32/lib/avutil.lib',
                          'modules/ib_video/ffmpeg/win32/lib/swresample.lib',
                          'modules/ib_video/ffmpeg/win32/lib/swscale.lib',
                          'modules/ib_video/GL/Win32/glew32.lib',
                          'modules/ib_video/GL/Win32/glew32s.lib',
						  'd3d9.lib'])

    #env.Append(LIBPATH=["C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86"])
    
    print(env["LINKFLAGS"])
    pass
