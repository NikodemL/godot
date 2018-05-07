# config.py

def can_build(platform):
    # Disable build by default
    return True

def configure(env):
	env.Append(LINKFLAGS=['modules/t9_sender/T9Sender.lib'])
	print(env["LINKFLAGS"])
