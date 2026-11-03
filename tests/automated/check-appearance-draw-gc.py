#!/usr/bin/env python3
"""Check modern shared appearance drawing handles failed temporary GCs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AppearanceDraw.c").read_text(
    encoding="utf-8"
)

def body(signature: str, next_signature: str) -> str:
    start = source.index(signature)
    return source[start:source.index(next_signature, start + len(signature))]

focus = body("Boolean\n_XmAppearanceDrawButtonFocus(", "/* Ordinary buttons share")
focus_create = "gc = XCreateGC(display, drawable, GCForeground | GCGraphicsExposures, &values);"
assert focus_create in focus and "if (!gc) {\n        XDestroyRegion(clip);\n        return True;" in focus
assert focus.index(focus_create) < focus.index("if (!gc) {") < focus.index("XSetRegion")

button = body("Boolean\n_XmAppearanceDrawButton(", "/* Paint the full face")
button_create = "gc=XCreateGC(display,drawable,GCForeground|GCGraphicsExposures,&values);"
assert button_create in button and "if (!gc) {\n        XDestroyRegion(clip);\n        return True;" in button
assert button.index(button_create) < button.index("if (!gc) {") < button.index("XSetRegion")

cascade = body("Boolean\n_XmAppearanceDrawCascadeIndicator(", "/* Choose readable status")
cascade_create = "gc = XCreateGC(display, drawable, GCForeground, &values);"
assert cascade_create in cascade and "if (!gc) return True;" in cascade
assert cascade.index(cascade_create) < cascade.index("if (!gc) return True;") < cascade.index("_XmAppearanceDrawChevron")

print("PASS shared modern appearance drawing guards temporary GCs")
