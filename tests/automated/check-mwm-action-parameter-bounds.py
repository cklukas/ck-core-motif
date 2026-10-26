from pathlib import Path

source = Path("clients/mwm/WmResParse.c").read_text(encoding="utf-8")
start = source.index("Add the exec parms if this is an f.action")
end = source.index("exec_parms_done:", start)
region = source[start:end]

assert "size_t total = 1;" in region
assert "SIZE_MAX - total" in region
assert "total > UINT_MAX" in region
assert "XtMalloc((Cardinal)total)" in region
assert "memcpy(&execParms[used]" in region
assert "strcat (pAP->szExecParms" not in region
print("PASS MWM action parameter construction is range checked")
