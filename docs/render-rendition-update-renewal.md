# Shared rendition update renewal

Render-table sharing uses copy-on-write when a rendition is updated.  If the
private rendition allocation fails, the update must leave the shared body and
its reference count unchanged.  `XmRenditionUpdate` now restores the reference
count and exits through its normal lock-release path instead of mutating shared
state after a failed renewal.
