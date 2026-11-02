#!/usr/bin/env python3
"""Verify the xmapdef demo's C23 contracts and file-viewer bounds."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "unsupported"
    / "xmapdef"
    / "xmapdef.c"
).read_text(encoding="utf-8")

assert "int main(int argc, char **argv)" in source
assert "void CreateApplication(Widget parent, FileData *filedata)" in source
assert "void OpenCB(Widget w, XtPointer client_data, XtPointer call_data)" in source
assert "void ReDraw(FileData *filedata)" in source
assert "while (filedata->num_lines > 0)" in source
assert "--filedata->num_lines;" in source
assert "while (filedata->num_lines-- >= 0)" not in source
assert "fclose(in_file);" in source
assert "limit = slider_size > filedata->num_lines - value" in source
assert "i < limit;" in source
assert "WarnUser(w, \"Cannot open\", file_name);" in source
assert "XtFree(file_name);" in source

print("xmapdef uses typed C23 callbacks and bounds file-viewer line access")
