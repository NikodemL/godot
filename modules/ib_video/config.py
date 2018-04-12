# config.py
import os
import shutil

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
						  'd3d9.lib'])

	if not os.path.exists('bin'):
		os.makedirs('bin')
		
	shutil.copyfile('modules/ib_video/GL/Win32/glew32.dll', 'bin/glew32.dll')
	shutil.copyfile('modules/ib_video/ffmpeg/win32/bin/avcodec-57.dll', 'bin/avcodec-57.dll')
	shutil.copyfile('modules/ib_video/ffmpeg/win32/bin/avdevice-57.dll', 'bin/avdevice-57.dll')
	shutil.copyfile('modules/ib_video/ffmpeg/win32/bin/avfilter-6.dll', 'bin/avfilter-6.dll')
	shutil.copyfile('modules/ib_video/ffmpeg/win32/bin/avformat-57.dll', 'bin/avformat-57.dll')
	shutil.copyfile('modules/ib_video/ffmpeg/win32/bin/avutil-55.dll', 'bin/avutil-55.dll')
	shutil.copyfile('modules/ib_video/ffmpeg/win32/bin/swresample-2.dll', 'bin/swresample-2.dll')
	shutil.copyfile('modules/ib_video/ffmpeg/win32/bin/swscale-4.dll', 'bin/swscale-4.dll')
	pass
