---
nav_exclude: true
---
# Mrm close-failure deletion

`Idb__FU_CloseFile` now attempts to unlink the file even when the descriptor
close fails and the caller requested deletion. This keeps failed temporary or
newly created databases from being retained solely because close reported an
error.
