# MWM embedded-client property growth

`UpdateEmbeddedClientsProperty` now grows its temporary window-ID list through
a replacement pointer. The count is advanced only after allocation succeeds,
and the existing `_DT_WORKSPACE_EMBEDDED_CLIENTS` property is left untouched
if a growth allocation fails. The previous direct `XtRealloc` assignment could
lose the old list and leave the count out of sync.

Allocation byte arithmetic is checked against both host and Xt limits. The
focused source contract is covered by
`tests/automated/check-mwm-embedded-client-growth.py`.
