from pathlib import Path


for name in ("Link.c", "Prompt.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text(
        encoding="utf-8"
    )
    start = source.index("static void Initialize(Widget request")
    body = source[start:source.index("static void Destroy", start)]
    assert "request->core.width" in body
    assert "request->core.height" in body
    assert "XmStringExtent" in body
print("PASS Link and Prompt derive natural geometry from requested dimensions")
