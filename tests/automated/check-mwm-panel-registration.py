#!/usr/bin/env python3
"""Check panel registration cannot use an unmatched screen record."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmManage.c").read_text(
    encoding="utf-8"
)

helper_start = source.index("FindPanelistScreen (Widget wPanelist)")
helper_end = source.index("RegisterEmbeddedClients", helper_start)
helper = source[helper_start:helper_end]
assert "wPanelist == NULL || wmGD.Screens == NULL || wmGD.numScreens <= 0" in helper
assert "pSD->managed && pSD->wPanelist == wPanelist" in helper
assert "return (NULL);" in helper

for name, argument in (
    ("RegisterEmbeddedClients", "pECD"),
    ("RegisterPushRecallClients", "pPRCD"),
):
    start = source.index(name + " (")
    end = source.index("} /* END OF FUNCTION " + name, start)
    body = source[start:end]
    assert "FindPanelistScreen (wPanelist)" in body
    assert "if (pSD == NULL)" in body
    assert body.index("if (pSD == NULL)") < body.index("pSD->")
    assert f"count < 0 || (count > 0 && {argument} == NULL)" in body

push_start = source.index("RegisterPushRecallClients (")
push_end = source.index("} /* END OF FUNCTION RegisterPushRecallClients", push_start)
push = source[push_start:push_end]
assert "for (i = 0; i < count ; i++, pPRCD++)" in push
assert "pPRCD->tvTimeout.tv_usec = 0;" in push

icon_start = source.index("RegisterIconBoxControl (Widget wPanelist)")
icon_end = source.index("} /* END OF FUNCTION RegisterIconBoxControl", icon_start)
icon = source[icon_start:icon_end]
assert "pSD = FindPanelistScreen (wPanelist);" in icon
assert "if (pSD != NULL)" in icon

match_start = source.index("IsPushRecallClient (ClientData *pCD")
match_end = source.index("} /* END OF FUNCTION IsPushRecallClient", match_start)
match = source[match_start:match_end]
assert "if (ppPRCD == NULL)" in match
assert "*ppPRCD = NULL;" in match
assert "pCD == NULL || pCD->pSD == NULL || pCD->clientName == NULL" in match
assert "pPRCD == NULL || pSD->numPushRecallClients <= 0" in match
assert "pPRCD->pchResName != NULL" in match

embedded_start = source.index("IsEmbeddedClient (ClientData *pCD")
embedded_end = source.index("} /* END OF FUNCTION IsEmbeddedClient", embedded_start)
embedded = source[embedded_start:embedded_end]
assert "if (ppECD == NULL)" in embedded
assert "*ppECD = NULL;" in embedded
assert "pCD == NULL || pCD->pSD == NULL || pCD->clientName == NULL" in embedded
assert "pECD == NULL || pSD->numEmbeddedClients <= 0" in embedded
assert "pECD->pchResName != NULL" in embedded

print("PASS MWM panel registration resolves only managed matching screens")
