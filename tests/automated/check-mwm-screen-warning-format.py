from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
start = source.index("Boolean F_Screen")
end = source.index("Boolean F_InvokeCommand", start)
body = source[start:end]

assert 'const char *format = "Unable to warp' not in body
assert body.count('"Unable to warp to unmanaged screen %d\\n"') == 2
print("PASS MWM unmanaged-screen warning keeps literal format checking")
