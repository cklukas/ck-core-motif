#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/BaseClass.c").read_text()
start = source.index("static XmWrapperData")
end = source.index("typedef struct _ExtToContextRec", start)
wrapper = source[start:end]
assert "XtCalloc(1, sizeof(XmBaseClassExtRec))" in wrapper
assert "if (!*wcePtr)" in wrapper
assert "return NULL;" in wrapper
start = source.index("_XmPushWidgetExtData(")
end = source.index("void\n_XmPopWidgetExtData", start)
push = source[start:end]
assert "newData = (XmAssocData) XtCalloc" in push
assert "if (!newData)" in push
assert "return;" in push
print("BaseClass extension allocation guards present")
