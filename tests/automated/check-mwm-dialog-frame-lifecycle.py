from pathlib import Path

source = Path("clients/mwm/WmManage.c").read_text(encoding="utf-8")
withdraw_start = source.index("void WithdrawDialog")
withdraw_end = source.index("END OF FUNCTION WithdrawDialog", withdraw_start)
withdraw = source[withdraw_start:withdraw_end]
remanage_start = source.index("void ReManageDialog")
remanage_end = source.index("END OF FUNCTION ReManageDialog", remanage_start)
remanage = source[remanage_start:remanage_end]

assert "if (pCD->clientFrameWin != None)" in withdraw
assert "if (pCD->clientFrameWin != None)" in remanage
assert withdraw.index("clientFrameWin != None") < withdraw.index("XUnmapWindow")
assert remanage.index("clientFrameWin != None") < remanage.index("XMapWindow")
print("PASS MWM dialog frame lifecycle validates frame resources")
