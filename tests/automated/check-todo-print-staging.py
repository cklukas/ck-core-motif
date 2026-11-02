#!/usr/bin/env python3
"""Check Todo uses safe print staging and typed manage callback contracts."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2] / "demos" / "programs" / "todo" / "todo.c"
).read_text(encoding="utf-8")
io_source = (
    Path(__file__).resolve().parents[2] / "demos" / "programs" / "todo" / "io.c"
).read_text(encoding="utf-8")

contracts = (
    "manageCB(Widget widget, Widget w_to_manage, XtPointer callback_data)",
    "home_directory = user != NULL ? user->pw_dir : \".\";",
    "static FILE *\nCreatePrintFile(char **pathname)",
    'temporary_directory = getenv("TMPDIR");',
    'temporary_directory = ".";',
    '"%s/xmdtodo-XXXXXX"',
    "descriptor = mkstemp(template_path);",
    "to = MIN(maxpages - 1, cb -> last - 1);",
    "if (pages[i] == NULL)",
    "(void)unlink(temporary_path);",
)
for contract in contracts:
    assert contract in source, f"missing Todo staging contract: {contract}"

assert 'XmdPrintDocument("/tmp/.todoout", cb);' not in source
assert "void manageCB( widget, w_to_manage, callback_data)" not in source
assert "ParseNewLines(char *label)" in io_source
assert "PrintWithNewLines(FILE *output, char *label)" in io_source
assert "void ParseNewLines(label)" not in io_source

print("Todo uses typed callbacks and unique guarded print staging")
