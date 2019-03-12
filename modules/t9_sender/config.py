# config.py

def can_build(env, platform):
    # Disable build by default
    return False

def configure(env):
	env.Append(LINKFLAGS=['modules/t9_sender/T9Sender.lib'])
	print(env["LINKFLAGS"])
