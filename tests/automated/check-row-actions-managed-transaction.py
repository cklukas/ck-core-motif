"""Check RowActions fills its item table before managing child buttons."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text(
    encoding="utf-8"
)
start = source.index("void XmRowActionsSetItems")
body = source[start : source.index("Cardinal XmRowActionsGetItemCount", start)]
assert "actions->actions.initializing = True;" in body
assert "XtVaCreateManagedWidget" not in body
assert body.index("XtVaCreateWidget") < body.index("XtManageChild")
assert body.index("XtManageChild") < body.rindex("actions->actions.initializing = False;")
print("PASS RowActions manages children only after item initialization")
