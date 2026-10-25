#!/usr/bin/env python3
"""Check FileSelectionBox search-list bounds and cleanup paths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()

file_start = source.index("FileSearchProc(\n", 1000)
file_end = source.index("\nstatic void", file_start)
file_region = source[file_start:file_end]
assert "SIZE_MAX / sizeof(XmString)" in file_region
assert "UINT_MAX / sizeof(XmString)" in file_region
assert "goto file_string_cleanup;" in file_region
assert "fileList = NULL ;" in file_region
assert "file_search_cleanup:" in file_region
assert "file_string_cleanup:" in file_region
assert "FS_ListUpdated(fs) = FALSE;" in file_region

dir_start = source.index("DirSearchProc(\n", 1000)
dir_end = source.index("\nstatic void", dir_start)
dir_region = source[dir_start:dir_end]
assert "SIZE_MAX / sizeof(XmString)" in dir_region
assert "UINT_MAX / sizeof(XmString)" in dir_region
assert "goto dir_string_cleanup;" in dir_region
assert "dir_string_cleanup:" in dir_region
assert "FS_DirectoryValid(fs) = FALSE;" in dir_region
print("FileSelectionBox search lists are bounded and cleaned transactionally")
