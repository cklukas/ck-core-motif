#!/usr/bin/env python3
"""Check that mwm error wrappers do not use fixed diagnostic buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmError.c").read_text()
io_start = source.index("int\nWmXIOErrorHandler")
io_end = source.index("} /* END OF FUNCTIONS WmXIOErrorHandler */", io_start)
io_region = source[io_start:io_end]
warning_start = source.index("void\nWarning")
warning_end = source.index("} /* END OF FUNCTION Warning */", warning_start)
warning_region = source[warning_start:warning_end]
assert "char  err[100]" not in io_region
assert "sprintf (err" not in io_region
assert "snprintf(NULL, 0" in io_region
assert "char pch[MAXWMPATH+1]" not in warning_region
assert "sprintf (pch" not in warning_region
assert "snprintf(NULL, 0" in warning_region
assert "XtFree(formattedMessage);" in warning_region
print("mwm error diagnostics are dynamically bounded")
