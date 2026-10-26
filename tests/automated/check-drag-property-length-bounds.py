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

target_writer = source[source.index("static void \nWriteTargetsTable"):source.index("static Boolean \nReadTargetsTable")]
assert "(uint64_t)targetsTable->entries[i].targets[j] > UINT32_MAX" in target_writer
assert "XtMalloc((Cardinal)dataSize)" in target_writer
assert "(CARD16)targetsTable->numEntries" in target_writer
assert "(CARD32)targetsTable->entries[i].targets[j]" in target_writer

compare_start = source.index("static int \nAtomCompare")
compare_end = source.index("/*****************************************************************************", compare_start)
compare = source[compare_start:compare_end]
assert "Atom first = *((const Atom *)atom1);" in compare
assert "return first < second ? -1 : (first > second ? 1 : 0);" in compare
assert "*((Atom *) atom1) - *((Atom *) atom2)" not in compare
