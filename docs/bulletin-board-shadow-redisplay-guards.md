---
nav_exclude: true
---
# BulletinBoard shadow redisplay guards

`XmBulletinBoard` redraws its manager shadow after redisplaying child gadgets.
The expose path previously assumed a realized window, valid shadow GCs, and
non-zero dimensions whenever shadow thickness was configured.

The shadow request now waits for those prerequisites.  Gadget redisplay and
cached geometry updates still run, and valid realized boards retain their
existing shadow type and dimensions.
