# MWM client-frame state changes

Active and inactive frame transitions update both the frame and, when
decoupled, its title child. Both entry points now require a realized frame and
only touch the title child when it exists, keeping focus/appearance callbacks
safe during partial teardown.
