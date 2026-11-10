#!/usr/bin/env python3
"""Check that transient-window list growth propagates allocation failure."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmManage.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("ListTransientSubtree (")
    end = text.index("} /* END OF FUNCTION ListTransientTree", start)
    region = text[start:end]
    assert "static Boolean" in region
    assert "pCD == NULL || ppWins == NULL || *ppWins == NULL" in region
    assert "*pCount > *pSize" in region
    assert "XtRealloc" in region
    assert "if (resized == NULL)" in region
    assert "Window *resized;" in region
    assert "*ppWins = resized;" in region
    assert "*pSize = new_size;" in region
    assert "*pSize += LTT_INCREMENT" not in region
    assert "if (!ListTransientSubtree" in region
    assert "if (!ListTransientSubtree (pCD, &pWins" in region
    assert "XtMalloc((Cardinal)(iSize * sizeof(Window)))" in region
    tree = region[region.index("static Window *") :]
    assert "resized = (Window *)XtRealloc" in tree
    assert "XtFree((char *)pWins);" in tree
    remanage_start = text.index("static void\nReManageWindow (")
    remanage = text[remanage_start:text.index("} /* END OF FUNCTION ReManageWindow */", remanage_start)]
    assert "if (pCD == NULL || pCD->pSD == NULL)" in remanage
    assert "pWins = ListTransientTree (pCD);" in remanage
    assert "if (pWins == NULL)" in remanage
    assert remanage.index("if (pWins == NULL)") < remanage.index("UnManageWindow (pCD);")
    print("MWM transient list growth propagates allocation failure")


if __name__ == "__main__":
    main()
