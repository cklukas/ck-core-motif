#!/usr/bin/env python3
"""Check Container button-up handling rejects missing action parameters."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerHandleBtn1Up(\n        Widget")
end = source.index("ContainerHandleBtn2Down(\n        Widget", start)
region = source[start:end]
assert "if (num_params == NULL || *num_params < 1)" in region
assert region.index("if (num_params == NULL || *num_params < 1)") < region.index("params[0]")

print("PASS Container button-up handling checks action parameters")
