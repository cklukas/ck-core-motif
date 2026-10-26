from pathlib import Path

source = Path("clients/mwm/WmXSMP.c").read_text(encoding="utf-8")
start = source.index("static Boolean\ngetProxyClientInfo")
end = source.index("cmpProxyClientProc", start)
region = source[start:end]

assert "textProperty.value == NULL || textProperty.nitems == 0" in region
assert "i + 1 < textProperty.nitems" in region
assert "textProperty.nitems - 1" not in region
print("PASS MWM XSMP proxy text-property bounds are checked")
