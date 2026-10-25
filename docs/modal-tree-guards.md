# Modal-tree guards

Modal bookkeeping can run while a transient subtree is being removed.  The
recursive mark, unmark, and count-fixup helpers now reject missing nodes and
non-positive decrement counts before touching modal state or pointer-grab
state.  Existing modal behavior for valid trees is unchanged.
