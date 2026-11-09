---
nav_exclude: true
---

# MWM transient-tree traversal

Transient windows form a first-child/next-sibling tree.  Several MWM modal and
restacking operations previously walked that structure recursively, so a deep
but valid dialog chain could consume the process call stack.

The modal-mark, full-modal cleanup, full-modal count fixup, and tree-count
walks now use iterative traversal through the existing `transientLeader`
parent links.  The post-order operations retain their established
children-before-parent ordering.  Counting saturates at `INT_MAX`, causing
existing allocation guards to fail closed rather than wrap.

The focused source regression is:

```sh
python3 tests/automated/check-mwm-transient-tree-walk.py
```
