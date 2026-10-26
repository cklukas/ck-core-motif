from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
start = source.index("void ShowActiveIconText")
end = source.index("END OF FUNCTION ShowActiveIconText", start)
body = source[start:end]

assert "if (!XGetWindowAttributes" in body
assert body.index("if (!XGetWindowAttributes") < body.index(
    "window_attribs.event_mask ="
)
print("PASS MWM active icon attributes query checks status")
