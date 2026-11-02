#!/usr/bin/env python3
"""Check GetSubRes uses typed callbacks and bounded report formatting."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "programs"
    / "getsubres"
    / "getsubres.c"
).read_text(encoding="utf-8")

contracts = (
    "GetClassPointer(String class)",
    "GetPrintRes(String name)",
    "main(int argc, char **argv)",
    "QuitCB(Widget w, XtPointer client_data, XtPointer call_data)",
    "HelpCB(Widget w, XtPointer client_data, XtPointer call_data)",
    "formatted_length = vsnprintf(NULL, 0, fmt, count_args);",
    "if (formatted_length < 0 || (unsigned int)formatted_length >",
    "tmp = XtMalloc(tmplen + 1U);",
    "(void)vsnprintf(tmp, (size_t)tmplen + 1U, fmt, args);",
    "if (tmplen > (Cardinal)SHRT_MAX)",
)
for contract in contracts:
    assert contract in source, f"missing GetSubRes report/C23 contract: {contract}"

legacy_definitions = (
    "static WidgetClass GetClassPointer(class)\n",
    "static void GetPrintRes (name)\n",
    "int main(argc, argv)\n",
    "static void QuitCB (w, client_data, call_data)\n",
)
for legacy in legacy_definitions:
    assert legacy not in source, f"legacy GetSubRes definition remains: {legacy!r}"

assert "vsprintf(tmp, fmt, args)" not in source

print("GetSubRes uses typed callbacks and bounded report formatting")
