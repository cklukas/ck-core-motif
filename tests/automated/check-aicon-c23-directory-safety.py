#!/usr/bin/env python3
"""Verify aicon's typed callbacks and bounded directory population."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "unsupported"
    / "aicon"
    / "aicon.c"
).read_text(encoding="utf-8")

assert "AppModalDialogWarning(Widget w, String str)" in source
assert "PopulateContainer(Widget cont, const char *dir)" in source
assert "static void QuitCB(Widget w, XtPointer client_data, XtPointer call_data)" in source
assert "static void OKCB(Widget w, XtPointer client_data, XtPointer call_data)" in source
assert "!cbs || !cbs->selected_items || cbs->selected_item_count <= 0" in source
assert "pix != XmUNSPECIFIED_PIXMAP &&" in source
assert "XGetGeometry(XtDisplay(widget), pix" in source
assert "No icon directory was selected." in source
assert "The icon directory path is too long." in source
assert "strlen(item->d_name) > sizeof(icon_fullname) - dir_len - 2" in source
assert "closedir(dirp);" in source
assert "if (i != 0)" in source
assert "XtManageChildren(icong, i);" in source
assert "XtFree(dir);" in source
assert "Too many icon files to display." in source
assert "max_allocation / sizeof(*icong)" in source

print("aicon uses typed C23 callbacks and bounds directory-driven icon population")
