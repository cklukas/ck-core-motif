from pathlib import Path

source = Path("clients/mwm/WmGraphics.c").read_text(encoding="utf-8")
assert "#include <stdint.h>" in source
for name, marker in (("void WmDrawString", "XDrawString"),
                     ("void WmDrawXmString", "XmStringDraw")):
    start = source.index(name)
    end = source.index("END OF FUNCTION", start)
    body = source[start:end]
    assert "== None" in body
    assert body.index("== None") < body.index(marker)
print("PASS MWM text wrappers validate drawables")
