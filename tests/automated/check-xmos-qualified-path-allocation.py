#!/usr/bin/env python3
"""Check Xmos qualified-path buffers are allocation-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("GetQualifiedDir(String dirSpec)")
end = source.index("\n/****************************************************************/\nString\n_XmOSFindPatternPart", start)
region = source[start:end]

assert region.count("outputBuf = AllocatePathBuffer") == 4
assert "outputBuf = XtMalloc(2);" in region
assert region.count("if (outputBuf == NULL)") == 5
assert region.count("return NULL;") >= 5
init_path_start = source.index("_XmOSInitPath(String")
init_path_end = source.index("\n\f\nint\nXmeMicroSleep", init_path_start)
init_path = source[init_path_start:init_path_end]
assert "size_t local_path_length;" in init_path
assert "local_path_length >= (size_t)UINT_MAX" in init_path
assert "XtMalloc((Cardinal)(local_path_length + 1))" in init_path
print("Xmos qualified-path allocations are guarded")
