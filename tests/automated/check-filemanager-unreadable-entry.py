#!/usr/bin/env python3
"""Check File Manager fallback entries do not dereference failed metadata."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "programs"
    / "filemanager"
    / "readdir.c"
).read_text(encoding="utf-8")

contracts = (
    "readIcon(char *str, Pixmap *icon, Pixmap *mask, Pixel fg, Pixel bg)",
    "path_length = snprintf(buf, sizeof(buf), \"%s/%s\", dir, info->d_name);",
    "if (path_length < 0 || (size_t)path_length >= sizeof(buf))",
    "FileInfoRec *info = ind >= 0 ? &FI[ind] : NULL;",
    "isdir = info != NULL && S_ISDIR(info->statbuf.st_mode);",
    "mode = info != NULL ? info->statbuf.st_mode : 0;",
    "else if (status != -1) {",
    "XmStringCreateLocalized(\"?\")",
    "if (status != -1 && S_ISDIR(FI[ind].statbuf.st_mode)",
    "char *msk = XtMalloc(name_length + 7U);",
)
for contract in contracts:
    assert contract in source, f"missing unreadable-entry guard: {contract}"

assert "static void readIcon(str, icon, mask, fg, bg)" not in source
assert "FileInfoRec *info = &FI[ind];" not in source

print("File Manager unreadable entries use bounded default-icon fallback")
