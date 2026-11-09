#!/usr/bin/env python3
"""Check remaining public composite queries and timer paths reject teardown."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
checks = {
    "Toast.c": ("static void timeout", "Widget XmCreateToast"),
    "AutoSuggestBox.c": ("static void cancel_popup", "static Boolean dismiss"),
    "Prompt.c": ("Boolean XmPromptGetVisible", "Widget XmCreatePrompt"),
    "FormSection.c": ("Widget XmFormSectionGetContent", "}"),
    "EmptyState.c": ("Widget XmEmptyStateGetMessage", "void XmEmptyStateInvokeAction"),
}

for filename, (start_marker, end_marker) in checks.items():
    source = (root / filename).read_text()
    start = source.index(start_marker)
    end = source.index(end_marker, start + len(start_marker))
    assert "being_destroyed" in source[start:end], filename

print("PASS remaining public queries and timer callbacks reject teardown state")
