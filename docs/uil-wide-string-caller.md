# UIL wide-string semantic-action failure

The wide-string semantic action now checks the constructor result before
writing expression or source metadata.  Allocation failure therefore exits
the action cleanly instead of dereferencing a missing semantic node.
