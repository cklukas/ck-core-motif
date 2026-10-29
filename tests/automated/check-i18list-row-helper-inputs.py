#!/usr/bin/env python3
"""Check shared I18List row helpers validate widgets before access."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)

selected_start = source.index("GetSelectedRows(XmI18ListWidget i18list, int *num_rows)")
selected_end = source.index("\n}\n", selected_start)
selected = source[selected_start:selected_end]
assert "if (i18list == NULL || num_rows == NULL)" in selected
assert selected.index("if (i18list == NULL") < selected.index(
    "XmI18List_row_data(i18list)"
)

for name in ("SelectRow(", "SelectItems("):
    start = source.index(
        "\n" + name,
        source.index("GetSelectedRows(XmI18ListWidget i18list, int *num_rows)"),
    )
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "if (i18list == NULL)" in region
    assert region.index("if (i18list == NULL)") < region.index(
        "XmI18List_row_data(i18list)"
    )

visible_start = source.index("XmI18ListMakeRowVisible(XmI18ListWidget i18list, int row)")
visible_end = source.index("\n}\n", visible_start)
visible = source[visible_start:visible_end]
assert "if (i18list == NULL)" in visible
assert visible.index("if (i18list == NULL)") < visible.index(
    "MakeCellVisible"
)

print("PASS I18List row helpers validate widgets before row access")
