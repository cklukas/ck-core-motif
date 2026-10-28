---
nav_exclude: true
---
# Transfer completion reentrancy

Transfer contexts now record completion under the process lock before done
callbacks run. Reentrant or concurrent completion attempts return immediately,
so callback delivery and context reclamation occur only once.

Regression coverage: `tests/automated/check-transfer-finish-reentrancy.py`.
