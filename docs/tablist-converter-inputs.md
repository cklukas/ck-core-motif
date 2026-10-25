---
nav_exclude: true
---

# Tab-list converter validation and rollback

The tab-list parser now validates its pointer inputs, and the converter treats
`XmTabListInsertTabs` allocation failure transactionally: the temporary tab and
partial list are released and conversion returns `False`. Valid tab parsing
and output negotiation remain unchanged.

The focused regression check is
`tests/automated/check-tablist-converter-inputs.py`.
