#!/usr/bin/env python3
"""Check widget byte swaps reject failed embedded callback conversions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIswap.c").read_text(
    encoding="utf-8"
)
start = source.index("Cardinal \nUrm__SwapRGMWidgetRecord")
body = source[start:]
guard = "if (Urm__SwapRGMCallbackDesc(callb_desc, widget_rec) != MrmSUCCESS)"

# One callback descriptor may be an argument value and another is the widget's
# creation callback.  Both conversions must fail the enclosing record.
assert body.count(guard) == 2
for offset in (match for match in range(len(body)) if body.startswith(guard, match)):
    failure = body.index("return MrmFAILURE;", offset)
    assert failure > offset

print("Mrm widget byte swaps propagate embedded callback conversion failures")
