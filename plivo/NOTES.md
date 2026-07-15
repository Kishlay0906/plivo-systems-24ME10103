# Design Notes

### System Design
* **Dual-Packet Redundancy:** The sender splits the transmission strategy by pushing out two identical UDP packets for almost every single frame simultaneously.
* **Proactive Drop Protection:** Utilizing the $2.0\times$ bandwidth budget upfront allows the system to achieve immediate drop protection without suffering the latency penalty of waiting for retransmissions.
* **Network Jitter Mitigation:** Because the redundant packets navigate distinct relay paths, the receiver takes the earliest arrival to dramatically compress effective network jitter.
* **Stateless Proxy Receiver:** The receiver operates as a simple, high-performance forwarding proxy that passes everything to the player, delegating deduplication directly to the harness.
* **Bandwidth Cap Compliance:** The sender strictly respects the $2.0\times$ limit by skipping the duplicate transmission on exactly 1 out of every 20 frames[cite: 1].

---

### Target Playout Delays
> **Recommended Grading Delays:** `60ms` for Profile A and `100ms` for Profile B.

---

### Failure Modes & Constraints
* **Burst Loss Vulnerability:** Extended network drop events (swallowing 3+ packets back-to-back) can overwhelm our immediate duplication scheme.
* **Tight Delay Deadlines:** Playout budgets at or below `40ms` cause failures due to underlying network travel times and system overhead mixed with the inability to pre-send data[cite: 1].
