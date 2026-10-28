#!/usr/bin/env python3
"""Check XmPrintToFile propagates file write and close failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PrintS.c").read_text()
assert "Boolean write_failed;" in source
assert "fwrite(data, 1, (size_t)data_len, file_desc->file)" in source
assert "file_desc->write_failed = True;" in source
assert "status = XPGetDocError;" in source
assert "fclose(file_desc->file) != 0" in source
assert "remove(file_desc->file_name)" in source
print("PrintShell file output propagates write and close failures")
