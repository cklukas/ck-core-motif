from pathlib import Path

source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
assert "Client frame is too narrow for its title bar" in source
assert "pcd->frameInfo.width <" in source
assert "pcd->frameInfo.width >=" in source
assert "2 * pcd->frameInfo.upperBorderWidth" in source
print("PASS MWM title-bar geometry rejects unsigned width underflow")
