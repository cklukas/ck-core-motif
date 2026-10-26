from pathlib import Path

source = Path("clients/mwm/WmInitWs.c").read_text(encoding="utf-8")
start = source.index("void CopyArgv")
end = source.index("END OF FUNCTION CopyArgv", start)
body = source[start:end]

assert body.count("dpy2Argv = argv;") == 2
assert body.count("dpy2Argc = argc;") >= 2
for fallback in [body.index("dpy2Argv = argv;"), body.rindex("dpy2Argv = argv;")]:
    assert body.index("dpy2Argc = argc;", fallback) > fallback
print("PASS MWM CopyArgv fallback preserves secondary-display argc")
