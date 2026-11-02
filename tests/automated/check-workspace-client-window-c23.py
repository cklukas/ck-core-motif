#!/usr/bin/env python3
"""Check Workspace client-window discovery uses typed C23 declarations."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "programs"
    / "workspace"
    / "client_win.c"
).read_text(encoding="utf-8")
wsm_source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "programs"
    / "workspace"
    / "wsm.c"
).read_text(encoding="utf-8")

contracts = (
    "static Window TryChildren(Display *, Window, Atom, unsigned int **);",
    "IsClientWindow(Display *dpy, Window win, unsigned int **retdata)",
    "TryChildren(Display *dpy, Window win, Atom wm_state, unsigned int **retdata)",
    "TryChildren(dpy, children[i], wm_state, retdata);",
)
for contract in contracts:
    assert contract in source, f"missing typed Workspace contract: {contract}"

assert "Window IsClientWindow(dpy, win, retdata)" not in source
assert "static Window TryChildren (dpy, win, WM_STATE, retdata)" not in source
assert "main(int argc, char **argv)" in wsm_source
assert "int main(argc, argv)" not in wsm_source

print("Workspace client-window discovery uses typed C23 Xlib contracts")
