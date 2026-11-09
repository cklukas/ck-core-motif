#!/usr/bin/env python3
"""Check IconGadget geometry traits validate outputs and clamp tiny bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text(
    encoding="utf-8"
)

baseline = source[source.rindex("GetBaselines("):]
assert "if (wid == NULL || baselines == NULL || line_count == NULL)" in baseline
assert baseline.index("if (wid == NULL || baselines == NULL || line_count == NULL)") < baseline.index(
    "*line_count = 0;"
)

display_start = source.rindex("GetDisplayRect(")
display = source[display_start:source.index("/********************************", display_start)]
assert "if (wid == NULL || displayrect == NULL)" in display
assert "Dimension highlight;" in display
assert display.count("bounded_position((long long)highlight)") == 2
assert "(*displayrect).width = (w > 2 * highlight) ?" in display
assert "(*displayrect).height = (h > 2 * highlight) ?" in display

margins_start = source.rindex("MarginsProc(")
margins = source[margins_start:source.index("/********************************", margins_start)]
assert "if (w == NULL || margins_rec == NULL)" in margins

print("PASS IconGadget geometry traits validate outputs and clamp tiny bounds")
