# The caller should loop on this
atomic_increment :
  ll r2,  0(r4)
  add r2, r2, r5
  sc  r2, 0(r4)
  jr r31

# With internal loop
atomic_increment2 :
  ll r2,  0   (r4)
  add r2, r2, r5
  sc  r2, 0   (r4)
  beq r2, r0, atomic_increment
  jr r31
