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
print("PASS MWM X11 property readers validate payload pointers")
