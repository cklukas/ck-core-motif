# XmTabList lock boundaries

`XmTabListFree` acquires the Motif process lock. Insert and copy failure paths
previously called it while already holding that lock, risking a self-deadlock.
Those paths now release the lock before invoking the public free routine;
normal ownership and cleanup behavior is unchanged.
