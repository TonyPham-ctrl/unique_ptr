== 1. Empty states ==
  a (default) -> null
  b (nullptr) -> null
  a.get() == nullptr: 1
  bool(a): 0
  leaving scope: nothing to destroy, no output expected

== 2. Owning: destructor deletes at end of scope ==
    + Noisy(1) constructed
  p -> Noisy(1)
  p->id = 1, (*p).id = 1
  leaving scope:
    - Noisy(1) destroyed
  after scope (Noisy(1) must already be destroyed above)

== 3. Move constructor: pointer transfers, nothing allocated or deleted ==
    + Noisy(2) constructed
  a (moved-from) -> null
  b -> Noisy(2)
  moved-from unique_ptr is GUARANTEED null (stronger than most std types)
  leaving scope: exactly one destruction
    - Noisy(2) destroyed

== 4. Move assignment: target's old object is deleted ==
    + Noisy(3) constructed
    + Noisy(4) constructed
  b = std::move(a):
    - Noisy(4) destroyed
  a -> null
  b -> Noisy(3)
  leaving scope:
    - Noisy(3) destroyed

== 5. Move assignment into an empty target ==
    + Noisy(5) constructed
  (no destruction should have printed)
  b -> Noisy(5)
  leaving scope:
    - Noisy(5) destroyed

== 6. Self move-assignment must not destroy the object ==
    + Noisy(6) constructed
  a after self-move -> Noisy(6)
  leaving scope:
    - Noisy(6) destroyed

== 7. Assigning nullptr deletes ==
    + Noisy(7) constructed
  a = nullptr:
    - Noisy(7) destroyed
  a -> null

== 8. release(): gives up ownership WITHOUT deleting ==
    + Noisy(8) constructed
  a after release -> null
  leaving scope of a: nothing destroyed, raw is now your problem
  delete raw:
    - Noisy(8) destroyed

== 9. reset(q): takes q, deletes the old object ==
    + Noisy(9) constructed
  a.reset(new Noisy(10)):
    + Noisy(10) constructed
    - Noisy(9) destroyed
  a -> Noisy(10)
  a.reset():
    - Noisy(10) destroyed
  a -> null
  a.reset() on empty: no output expected
  Ordering rule for reset(q): old = ptr; ptr = q; if (old) delete old;
  Store the new pointer BEFORE deleting the old one, so the object is never
  observable in a half-dead state if ~T() touches this unique_ptr.

== 10. swap() ==
    + Noisy(11) constructed
    + Noisy(12) constructed
  a -> Noisy(12)
  b -> Noisy(11)
  no constructions or destructions during swap; leaving scope:
    - Noisy(11) destroyed
    - Noisy(12) destroyed

== 11. Comparisons with nullptr and between pointers ==
    + Noisy(13) constructed
  a != nullptr: 1, b == nullptr: 1
  a == b: 0 (compares get())
  leaving scope:
    - Noisy(13) destroyed

== 12. Returning from a function = ownership goes to the caller ==
    + Noisy(14) constructed
  factory: about to return local (implicit move, no std::move needed)
  caller got -> Noisy(14)
  leaving scope:
    - Noisy(14) destroyed

== 13. Passing by value = the function takes ownership ==
    + Noisy(15) constructed
  sink got -> Noisy(15)
  sink: returning, parameter p goes out of scope
    - Noisy(15) destroyed
  mine after sink -> null
  (Noisy(15) was destroyed inside sink)

== 14. Borrowing: pass T& / T*, owner keeps it ==
    + Noisy(16) constructed
  borrow: reading Noisy(16), not deleting it
  mine after borrow -> Noisy(16)
  leaving scope:
    - Noisy(16) destroyed

== 15. In a vector: reallocation MOVES, never destroys owned objects ==
    + Noisy(100) constructed
    + Noisy(101) constructed
    + Noisy(102) constructed
    + Noisy(103) constructed
  4 pushes done, several reallocations happened, zero destructions printed
  v.erase(v.begin() + 1):
    - Noisy(101) destroyed
  v.clear():
    - Noisy(100) destroyed
    - Noisy(102) destroyed
    - Noisy(103) destroyed

== 16. Converting move: unique_ptr<Derived> -> unique_ptr<Base> ==
  d is null: 1, b->name() = Derived
  leaving scope: both destructors run because ~Base is virtual
      ~Derived
      ~Base (virtual)

== 17. make_unique: forwards args, one expression, no raw new ==
    + Noisy(17) constructed
  p -> Noisy(17)
  leaving scope:
    - Noisy(17) destroyed

== 18. Things that must NOT compile (uncomment one at a time to check) ==
  UP c = a;                 // R1 copy ctor deleted
  c = a;                    // R1 copy assign deleted
  UP d = new Noisy(1);      // R3 explicit ctor: no implicit T* -> unique_ptr
  int x = a;  bool y = a;   // R4 explicit bool
  void f(UP); f(a);         // must be f(std::move(a))

== 19. Compiles but is a BUG (do not run) ==
  Noisy* r = new Noisy(1); UP x(r); UP y(r);   // double delete
  UP x(new Noisy(1)); x.reset(x.get());        // deletes the object it now holds
  UP x; x->id;                                 // null deref, UB
  UP x(&stack_obj);                            // delete on a stack address
  Base without virtual dtor via unique_ptr<Base> // ~Derived never runs

== done ==