#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VendorS.c").read_text()
start = source.index("MotifWarningHandler (")
end = source.index("XtWarning(output);", start)
region = source[start:end]
assert "combined_len >= (size_t)UINT_MAX" in region
assert "if (combined == NULL)" in region
assert "combined_len > SIZE_MAX - 2" in region
assert "output_len >= (size_t)UINT_MAX" in region
assert "if (output == NULL)" in region
assert region.index("if (combined == NULL)") < region.index("snprintf(combined")
assert region.index("if (output == NULL)") < region.index("dst = output")
print("VendorShell warning-buffer guards present")
