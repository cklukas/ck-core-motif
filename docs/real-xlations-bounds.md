# Generated real translation bounds

The legacy `_XmGetRealXlations` helper expands virtual key bindings into real
translation text. It previously concatenated key names and arbitrary action
strings into a 1000-byte stack buffer and asserted only after writing it.

The helper now appends into Xt-owned growable storage with checked size
arithmetic and allocation limits. Long valid action strings are preserved;
allocation overflow returns `NULL` cleanly. The translation text and modifier
format for normal inputs are unchanged.

`tests/automated/check-real-xlations-growth.py` guards the storage contract.
