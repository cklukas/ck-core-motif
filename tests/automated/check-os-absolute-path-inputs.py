#!/usr/bin/env python3
"""Check _XmOSAbsolutePathName validates all pointer inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("_XmOSAbsolutePathName(String path")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "path == NULL || pathRtn == NULL || buf == NULL" in region
assert "return False;" in region
assert region.index("path == NULL") < region.index("*pathRtn = path")
print("Motif path resolution rejects null pointer inputs")
