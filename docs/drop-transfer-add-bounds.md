# Bound drop-transfer list additions

`XmDropTransferAdd` now builds the copied transfer entries and resized list in
a transactional order.  Count products are checked against Xt's `Cardinal`
limit, source-copy failure leaves the existing lists unchanged, and a failed
list reallocation no longer loses the old pointer or publishes a null entry.

Successful additions retain the existing public behavior.  The focused source
regression is `tests/automated/check-drop-transfer-add-bounds.py`.
