# Xpm hash probe wrapping

Hash-table collision probing now wraps when the current bucket is the first
bucket before decrementing. The previous order formed a pointer one element
before the allocation, which is undefined behavior even when it was not
dereferenced.
