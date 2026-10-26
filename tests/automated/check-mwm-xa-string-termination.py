from pathlib import Path

init_ws = Path("clients/mwm/WmInitWs.c").read_text(encoding="utf-8")
start = init_ws.index("static Boolean\nGetBindingsProperty")
end = init_ws.index("static void\nSetBindingsProperty", start)
bindings = init_ws[start:end]
assert "bounded_prop = (String)XtMalloc" in bindings
assert "bounded_prop[num_items] = '\\0';" in bindings
assert "num_items >= UINT_MAX" in bindings

property_source = Path("clients/mwm/WmProperty.c").read_text(encoding="utf-8")
start = property_source.index("void GetDtSessionHints")
end = property_source.index("GetDtWmRequest", start)
session = property_source[start:end]
assert "bounded_property = (char *)XtMalloc" in session
assert "bounded_property[nitems] = '\\0';" in session
assert "nitems >= UINT_MAX" in session
print("PASS MWM XA_STRING properties are explicitly terminated")
