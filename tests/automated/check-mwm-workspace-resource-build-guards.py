#!/usr/bin/env python3
"""Check non-WSM MWM does not count or use null workspace resource arrays."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmResource.c").read_text(
    encoding="utf-8"
)
start = source.index("void \nProcessWorkspaceResources (WmWorkspaceData *pWS)")
end = source.index("} /* END OF FUNCTION ProcessWorkspaceResources */", start)
body = source[start:end]

resource_start = body.index("if (wmGD.useStandardBehavior)")
resource_end = body.index("#endif /* WSM */", resource_start)
resource_lookup = body[resource_start:resource_end]
assert body.rfind("#ifdef WSM", 0, resource_start) != -1
assert "XtNumber (wmStdWorkspaceResources)" in resource_lookup
assert "XtNumber (wmWorkspaceResources)" in resource_lookup
assert source.count("XtResource *wmStdWorkspaceResources = NULL;") == 1
assert source.count("XtResource *wmWorkspaceResources = NULL;") == 1

print("PASS non-WSM MWM skips workspace resource arrays")
