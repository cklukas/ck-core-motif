#!/usr/bin/env python3
"""Check transactional ownership of AutoSuggestBox's no-results label."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c"
text = source.read_text(encoding="utf-8")

assert "static const char *no_results_text(XmAutoSuggestBoxWidget box)" in text
assert "XmStringCreateLocalized(no_results_text(box))" in text

start = text.rindex("static Boolean SetValues(")
end = text.index("\nstatic XtGeometryResult GeometryManager", start)
set_values = text[start:end]
assert "String replacement = XtNewString(no_results_text(box));" in set_values
assert "XmString label = NULL;" in set_values
assert "if (replacement == NULL)" in set_values
assert "if (label == NULL)" in set_values
assert "box->auto_suggest.no_results_label = old->auto_suggest.no_results_label;" in set_values
assert set_values.index("if (label == NULL)") < set_values.index(
    "XtFree(old->auto_suggest.no_results_label);"
)
assert "if (label != NULL)" in set_values

print("AutoSuggestBox no-results label replacement preserves old state on failure")
