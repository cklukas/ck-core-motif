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
print("PASS MWM X11 property readers validate payload pointers")
