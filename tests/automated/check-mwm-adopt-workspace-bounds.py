from pathlib import Path

source = Path("clients/mwm/WmManage.c").read_text(encoding="utf-8")
start = source.index("void AdoptInitialClients")
end = source.index("END OF FUNCTION AdoptInitialClients", start)
body = source[start:end]

assert "pSD->numWorkspaces > INT_MAX - 2" in body
assert "SIZE_MAX / sizeof(Window)" in body
assert "UINT_MAX / sizeof(Window)" in body
guard = body.index("pSD->numWorkspaces > INT_MAX - 2")
addition = body.index("nAncillaries = 2 + pSD->numWorkspaces")
assert guard < addition
allocation_failure = body.index("if (!pAncillaryWindows)")
return_after_exit = body.index("return;", allocation_failure)
assert allocation_failure < return_after_exit
assert "unsigned int i,j;" in body
print("PASS MWM initial-client adoption bounds workspace-array arithmetic")
