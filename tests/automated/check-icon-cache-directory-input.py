#!/usr/bin/env python3
"""Check IconFile cache construction rejects absent/empty directory names."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("MakeCachedDirEntry(String dirName)")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "if (dirName == NULL || *dirName == '\\0')" in function
assert function.index("dirName == NULL") < function.index("opendir")
print("PASS IconFile cache directory input guard")
