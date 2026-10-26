from pathlib import Path

source = Path("clients/mwm/WmManage.c").read_text(encoding="utf-8")
for name, marker in (("WithdrawDialog", "XUnmapWindow"),
                     ("ReManageDialog", "XMapWindow")):
    start = source.index(f"void {name}")
    end = source.index("END OF FUNCTION", start)
    body = source[start:end]
    assert "pCD == NULL" in body
    assert body.index("pCD == NULL") < body.index(marker)
print("PASS MWM dialog context payloads are validated")
