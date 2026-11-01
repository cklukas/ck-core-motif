from pathlib import Path

files = {
    "clients/mwm/WmInitWs.c": ["(prop == NULL)"],
    "clients/mwm/WmWinInfo.c": ["(clientID != NULL)", "(saveHintFlags != NULL)"],
    "clients/mwm/WmProperty.c": ["(property == NULL)"],
}
for name, needles in files.items():
    source = Path(name).read_text(encoding="utf-8")
    for needle in needles:
        assert needle in source, (name, needle)
property_source = Path("clients/mwm/WmProperty.c").read_text(encoding="utf-8")
assert "(nitems == PROP_WM_STATE_ELEMENTS) &&" in property_source
assert "(nitems >= PROP_MWM_HINTS_ELEMENTS) &&" in property_source
assert "(property != NULL)" in property_source
protocol_start = property_source.index("void ProcessWmProtocols")
protocol_end = property_source.index("\n} /* END OF FUNCTION ProcessWmProtocols */", protocol_start)
protocol = property_source[protocol_start:protocol_end]
assert "if (nitems == 0 || property == NULL ||" in protocol
assert protocol.index("nitems == 0") < protocol.index("XtMalloc")
print("PASS MWM X11 property readers validate payload pointers")
