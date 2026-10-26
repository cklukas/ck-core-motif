#!/usr/bin/env python3
"""Ensure DragBS property writers bound Xlib's signed length parameter."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c").read_text()
assert source.count("dataSize > UINT32_MAX || dataSize > (size_t)INT_MAX") == 2
assert source.count("(int)dataSize") >= 2
print("PASS DragBS property lengths are bounded")

atom_writer = source[source.index("static void \nWriteAtomsTable"):source.index("static Boolean \nReadAtomsTable")]
assert "(uint64_t)atomsTable->entries[i].atom > UINT32_MAX" in atom_writer
assert "(uint64_t)atomsTable->entries[i].time > UINT32_MAX" in atom_writer
assert "XtMalloc((Cardinal)dataSize)" in atom_writer
assert "(CARD16)atomsTable->numEntries" in atom_writer
assert "(CARD32)dataSize" in atom_writer
assert "(CARD32)atomsTable->entries[i].atom" in atom_writer
assert "(CARD32)atomsTable->entries[i].time" in atom_writer
