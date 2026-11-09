#!/usr/bin/env python3
"""Verify Exm Container owns generated default tab lists per instance."""

from pathlib import Path


root = Path(__file__).resolve().parents[2] / "demos" / "unsupported" / "Exm"
source = (root / "Container.c").read_text(encoding="utf-8")
header = (root / "ContainerP.h").read_text(encoding="utf-8")

assert "XmTabList               generated_tablist;" in header
assert "static int Num_tab" not in source
assert "static XmTab * Tab_pool" not in source
assert "static XmTabList Tab_list" not in source
assert "GetDumbTabList(cw, tab_size," in source
assert "cw->container.generated_tablist = NULL;" in source
assert "XmTabListFree(cw->container.generated_tablist);" in source
assert "cw->container.generated_tablist = tab_list;" in source
assert "while (i != 0)" in source
assert "(size_t)asked_num_tab > SIZE_MAX / sizeof(*tab_pool)" in source

print("Exm Container default tab lists are bounded and owned per widget")
