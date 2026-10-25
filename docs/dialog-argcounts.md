# Dialog argument counts

Command and MessageBox convenience constructors add one internal argument to
the caller's list. They now reject maximum counts and byte sizes that cannot
represent the additional argument in the host or Xt allocation widths before
that addition and allocation can wrap. Non-empty lists also require a valid
source pointer; an empty null list remains valid. Normal argument lists and
dialog resources are unchanged.
