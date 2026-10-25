# UTM payload failure cleanup

UTM packages own their parameter payload until the transfer is handed to
Motif's selection machinery.  Queue failure, sink rejection, and a missing
destination callback now release both the package and its payload.  Successful
transfers retain the existing ownership handoff to `XmTransferSetParameters`.
