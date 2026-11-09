#!/usr/bin/env python3
"""Check bounded MWM accelerator-display formatting."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nAppendAccelText")
end = source.index("} /* END OF FUNCTION ProcessAccelText */", start)
body = source[start:end]

assert "count > SIZE_MAX - *used - 1" in body
assert "required = *used + count + 1;" in body
assert "XtRealloc((char *)*destP" in body
assert "*capacity = newCapacity;" in body
assert "while (startP < endP && *startP != '<')" in body
assert "while (startP < endP && *startP != '>')" in body
assert "if (chlen <= 0)" in body
assert "return False;" in body
assert body.count("AppendAccelText(destP, destCapacity, &destUsed") >= 4

accelerator_start = source.index("static Boolean ParseWmAccelerator")
accelerator_end = source.index("} /* END OF FUNCTION ParseWmAccelerator */", accelerator_start)
accelerator = source[accelerator_start:accelerator_end]
assert "acceleratorLength = (size_t)(lineP - *linePP)" in accelerator
assert "ProcessAccelText(*linePP, lineP," in accelerator
assert "acceleratorCapacity = acceleratorLength + 1;" in accelerator
assert "&acceleratorText, &acceleratorCapacity" in accelerator
assert "XtFree((char *)acceleratorText);" in accelerator
assert accelerator.index("XtFree((char *)acceleratorText);") < accelerator.index(
    "status = FALSE;", accelerator.index("XtFree((char *)acceleratorText);")
)
assert accelerator.index("menuItem->accelState = state;") > accelerator.index(
    "if (ProcessAccelText"
)

print("PASS MWM accelerator display text is bounded and rejects invalid multibyte input")
