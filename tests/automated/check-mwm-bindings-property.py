from pathlib import Path

source = Path("clients/mwm/WmInitWs.c").read_text(encoding="utf-8")
start = source.index("GetBindingsProperty(")
body = source[start:source.index("static void", start)]

assert "status = XGetWindowProperty" in body
assert "(status != Success)" in body
assert body.index("status != Success") < body.index("*binding = (String) prop")
print("PASS MWM bindings property validates XGetWindowProperty status")
