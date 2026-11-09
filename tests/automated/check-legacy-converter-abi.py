#!/usr/bin/env python3
"""Keep legacy widget resource converters ABI- and size-safe."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
checks = {
    "IconButton.c": ("CvtStringToIconPlacement",),
    "ButtonBox.c": ("CvtStringToFillOption",),
    "Column.c": (
        "CvtStringToXiAlignment",
        "CvtStringToFillStyle",
        "CvtStringToDistribution",
    ),
}

for filename, names in checks.items():
    source = (root / filename).read_text(encoding="utf-8")
    for name in names:
        start = source.rindex(f"{name}(Display *")
        end = source.index("\n{", start)
        assert "XtPointer *converter_data" in source[start:end], (filename, name)
        registration = source.index(name, source.index("XtSetTypeConverter"))
        assert "(XtTypeConverter)" not in source[registration - 40:registration + len(name)]

button_box = (root / "ButtonBox.c").read_text(encoding="utf-8")
start = button_box.index("CvtStringToFillOption(Display * dpy")
end = button_box.index("\n}\n", start) + 3
converter = button_box[start:end]
capacity = converter.index("if ( toVal->size >= sizeof(XmFillOption) )")
write = converter.index("*state = option;", capacity)
required_size = converter.index("toVal->size = sizeof(XmFillOption);", capacity)
assert capacity < write < required_size

print("PASS legacy resource converters use the Xt ABI and preserve output bounds")
