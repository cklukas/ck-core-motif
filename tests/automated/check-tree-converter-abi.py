#!/usr/bin/env python3
"""Keep XmTree resource converters ABI-compatible with XtTypeConverter."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Tree.c").read_text(
    encoding="utf-8"
)

class_init = source[source.index("static void \nClassInit(void)"):source.index("/*\n * ClassPartInitialize")]
for name in ("CvtStringToConnectStyle", "CvtStringToCompressStyle", "CvtStringToLineStyle"):
    start = source.index(f"{name}(Display * dpy")
    end = source.index("\n{", start)
    signature = source[start:end]
    assert "XtPointer *converter_data" in signature, name
    registration = class_init.index(name)
    assert "(XtTypeConverter)" not in class_init[registration - 40:registration + len(name)]

print("PASS XmTree resource converters use the XtTypeConverter ABI")
