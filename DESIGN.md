# Design notes

Four decisions that shape this codebase, why they were made, and what was considered instead. Known limitations are listed at the end.

---

## 1. Time is passed in, not read inside the class

**Decision.** `BamReassembler::onFrame(frame, nowMs)` and `checkTimeout(nowMs)` receive the current time as a parameter. The class never reads a clock. The application passes `std::chrono::steady_clock` time in milliseconds.

**Why.** The J1939 T1 timeout (750 ms between packets) is the most important failure path of BAM, so it must be tested. With time as an input, a test simply says "now it is 1000 ms" and checks the result: no `sleep()`, no slow or flaky tests. The class also stays independent of the platform clock.

**Alternative.** Call `steady_clock::now()` inside the class. Tests would then have to sleep for almost a second per timeout case, and would still be timing-dependent on a loaded CI machine. A full injectable `IClock` interface would also work, but a plain integer parameter is simpler and enough for one consumer.

**Monotonic clock.** The application uses `steady_clock`, not the wall clock, because the wall clock can jump (NTP, manual changes) and would make timeouts fire too early or never.

---

## 2. One BAM session per sender

**Decision.** Each `BamReassembler` follows exactly one source address and ignores frames from any other sender. The application keeps one reassembler per source address in a `std::map<uint8_t, BamReassembler>`, created the first time that sender is heard.

**Why.** On a real bus several ECUs can send BAM transfers at the same time. The TP.DT data packets carry only a sequence number, not the PGN, so the only way to know which transfer a packet belongs to is its source address. With a single shared session, packet 2 from ECU B would be appended to ECU A's message and produce a corrupted result. This was a real bug in an early version, found in review, and is now covered by the `OtherSenderIgnored` test.

**Alternative.** A fixed array of 256 reassemblers indexed by source address: no allocation at all, but about 460 KB of memory, almost all unused. The map allocates once per new sender and then reuses the same object for every later transfer from it.

---

## 3. `epoll` with a 100 ms timeout instead of a blocking `read()`

**Decision.** `SocketCanSource::read()` waits in `epoll_wait()` for at most 100 ms. If no frame arrives it returns `false`, and the main loop calls `checkTimeout()` on every reassembler before waiting again.

**Why.** A blocking `read()` only returns when a frame arrives. If an ECU sends a TP.CM and then goes silent (powered off, disconnected), the program stays inside `read()` forever, the T1 timeout is never checked, and the half-built transfer is never dropped. With a bounded wait, the loop wakes up at least every 100 ms, so timeouts fire on a silent bus. This is demonstrated with `cansend vcan0 1CECFF00#200A0002FFCAFE00`, which prints `BAM timeout from SA 0` about 750 ms later with no further traffic.

**Alternative.** `select()` or `poll()` would also give a timeout; they were not chosen because `epoll` keeps the watch list in the kernel and scales to several descriptors (a second CAN interface, a timer, a signal fd) without rebuilding the list on every call. A non-blocking socket with a busy loop was rejected because it wastes CPU. A separate timer thread was rejected because it adds locking for very little benefit.

**Interface unchanged.** The wait lives entirely inside `SocketCanSource`. `ICanSource::read()` still returns `true` or `false`, so the decoders, `FakeCanSource` and the unit tests did not change.

---

## 4. Fixed-size buffer instead of `std::vector`

**Decision.** `BamReassembler` stores the payload in `uint8_t data_[1785]`, plus a byte counter.

**Why.** 1785 is the largest payload BAM can carry: at most 255 packets × 7 bytes. Because the maximum is known in advance, the memory can be reserved once, when the object is created, and receiving a transfer never allocates. On embedded targets this matters: allocation time is not deterministic, the heap can fragment over long uptimes, and some safety-oriented coding standards forbid dynamic allocation after start-up. Out-of-range sizes (below 9 or above 1785 bytes) are rejected before any byte is copied.

**Alternative.** `std::vector<uint8_t>` with `push_back()` (the first version of this class) is simpler, but it allocates during reception. `reserve(totalSize)` at the start of each transfer would reduce that to one allocation per transfer, but it is still an allocation in the receive path.

---

## Verification

- 34 GoogleTest unit tests for the library, including every BAM failure path (missing, repeated or out-of-order packet, timeout with and without traffic, bad sizes, short frames, other senders, new announcement mid-transfer).
- The same tests run under AddressSanitizer and UndefinedBehaviorSanitizer in CI. The setup was checked by injecting a missing `return` and an out-of-bounds read; both were reported.
- 5 pytest unit tests for the simulator's frame builders.
- Manual end-to-end runs on `vcan0` with the Python ECU simulator and `cansend`.

## Known limitations

- Only BAM is implemented; connection-mode transfers (RTS/CTS) are not.
- Reserved, error and "not available" SPN values are all reported as "not available".
- DM1 lamp flash states and the SPN conversion method bit are ignored (version 4 is assumed).
- The CAN interface name (`vcan0`) is fixed in `main.cpp`.
- There is no automated end-to-end test that runs the monitor and the simulator together.
