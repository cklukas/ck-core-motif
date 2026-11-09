#!/usr/bin/env python3
"""Check TARGETS conversion reserves every conditionally advertised atom."""

from pathlib import Path


root = Path(__file__).parents[2]

list_source = (root / "lib" / "Xm" / "List.c").read_text(encoding="utf-8")
list_start = list_source.index("if (cs->target == atoms[XmATARGETS])")
list_end = list_source.index("else if ((cs->target", list_start)
list_targets = list_source[list_start:list_end]
assert "XmeStandardTargets(w, 6, &target_count)" in list_targets
assert "if (XA_STRING != C_ENCODING)" in list_targets

i18_source = (root / "lib" / "Xm" / "I18List.c").read_text(encoding="utf-8")
i18_start = i18_source.index("if (cs->target == atoms[XmATARGETS])")
i18_end = i18_source.index("else if ((cs->target", i18_start)
i18_targets = i18_source[i18_start:i18_end]
assert "XmeStandardTargets(w, 7, &target_count)" in i18_targets
assert "if (XA_STRING != C_ENCODING)" in i18_targets
assert "if (ListDragConv->pixmap != None)" in i18_targets

export_start = i18_source.index("else if ((cs->target", i18_start)
export_end = i18_source.index("else if (cs->target == atoms[XmACOMPOUND_TEXT]", export_start)
export_targets = i18_source[export_start:export_end]
assert "if (targs == NULL)" in export_targets
assert "cs->status = XmCONVERT_REFUSE;" in export_targets

string_trans = (root / "demos" / "lib" / "Exm" / "StringTrans.c").read_text(
    encoding="utf-8"
)
string_start = string_trans.index("Atom *targs = XmeStandardTargets(w, 5, &n);")
string_end = string_trans.index("targs[n] = MOTIF_C_S", string_start)
assert "if (targs == NULL)" in string_trans[string_start:string_end]

dnd = (root / "demos" / "programs" / "drag_and_drop" / "DNDDemo.c").read_text(
    encoding="utf-8"
)
assert dnd.count("if (targs == NULL)") >= 2
assert dnd.count("cs -> status = XmCONVERT_REFUSE;") >= 2

print("TARGETS conversion reserves optional atoms and rejects allocation failure")
