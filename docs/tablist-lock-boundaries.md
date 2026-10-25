# XmTabList lock boundaries

Internal tab-list copy and free helpers now operate under an already-held
process lock. Public `XmTabListCopy` and `XmTabListFree` remain the locking
boundaries, while mutation paths can clean private lists without recursively
acquiring the lock.
