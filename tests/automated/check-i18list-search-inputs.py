#!/usr/bin/env python3
"""Check I18List selection/search APIs validate public inputs."""

import re
from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)

selected_start = source.index("XmI18ListGetSelectedRows(Widget w)")
selected_end = source.index("\n}\n", selected_start)
selected = source[selected_start:selected_end]
assert "if (w == NULL)" in selected
assert "return NULL;" in selected
assert selected.index("if (w == NULL)") < selected.index(
    "XmI18List_row_data(ilist)"
)

search_match = re.search(
    r"(?m)^XmI18ListDoSearch\(Widget w, String str, Boolean reset\)", source
)
assert search_match is not None
search_start = search_match.start()
search_end = source.index("\n/*  Function Name:", search_start)
search = source[search_start:search_end]
assert "if (w == NULL || str == NULL)" in search
assert "return False;" in search
assert search.index("if (w == NULL || str == NULL)") < search.index(
    "XmStringCreateLocalized(str)"
)

print("PASS I18List selection and search APIs validate inputs")

find_matches = list(re.finditer(
    r"(?m)^XmMultiListRowInfo \*XmI18ListFindRow\(Widget w, String str, int \*found_column,",
    source,
))
assert find_matches
find_start = find_matches[-1].start()
find_end = source.index("\n/*  Function Name:", find_start)
find = source[find_start:find_end]
assert "if (w == (Widget) NULL || str == NULL || found_column == NULL)" in find
assert find.index("if (w == (Widget) NULL") < find.index(
    "XmStringCreateLocalized(str)"
)
