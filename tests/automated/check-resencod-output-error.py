#!/usr/bin/env python3
"""Check compound-text output construction failures reach the converter."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
assert "Boolean         output_error;" in source
assert source.count("ctx->output_error = False;") >= 2
assert source.count("ok = ok && !ctx->output_error;") >= 2
output_start = source.index("/* outputXmString */")
output_end = source.index("static XmString \nconcatStringToXmString", output_start)
output = source[output_start:output_end]
assert output.count("ctx->output_error = True;") >= 3
print("PASS compound-text output error propagation")
