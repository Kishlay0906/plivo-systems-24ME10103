# Run Log

| Profile | Delay (ms) | Miss % | Overhead | Modifications & Core Rationale |
| :--- | :--- | :--- | :--- | :--- |
| A.json | 60ms | 0.53% | 2.00x | Evaluated immediate dual-packet redundancy for the majority of frames. Successfully mitigated random packet drops and jitter spikes. |
| A.json | 40ms | 5.47% | 2.00x | Attempted aggressive delay reduction to 40ms; failed validation. Base network latency combined with serialization delay breaches the arrival window. Frames cannot be transmitted pre-emptively due to strict source constraints. |
| B.json | 100ms | 0.93% | 2.00x | Deployed the dual-packet redundancy protocol against Profile B's 5% drop rate. Validated successfully; sending decoupled packet copies keeps latency low and maintains reliability. |
| B.json | 80ms | 2.80% | 2.00x | Attempted to optimize Profile B down to an 80ms playout buffer. Miss rate escalated past the 1% threshold due to the 5% drop rate intersecting with the non-duplicated budget-saving frames. |

---

### **Final Optimized Delay Configurations**
* **Profile A:** `60ms`
* **Profile B:** `100ms`
