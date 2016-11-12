1. Milestone to get to core functionality.
* Implement basic set interface.
* Implement basic list interface.
* Implement iterator/algorithm interface.
* Hook into a real unit-testing framework.
* Actually learn and write a well-designed Makefile
* Hook into the neat green light github testing things for build/tests.
* Exception safety?
* Settle on good design for height generation (use hacker's delight to get leftmost "1" bit?).
* Generate exhaustive tests.
* Generate targeted tests.
* Somehow confirm that internal invariants about pointer consistency hold. 
* Conclusion: be confident in interface/API design and correctness.

2. Milestone: converge on single-core performance.
* Develop benchmarks that differ between this skiplist and stl containers.
* Develop benchmarks in general.
* Measure those differences and develop empirical evidence for their causes.
* Improve as needed.
* Note: be wary of interesting hardware concerns, cache size etc.
* Repeat until satisfied.

3. Milestone to get a correct concurrent implementation:
* Consider how (if at all) the interface of a container should change.
* Exception safety?
* Develop correctness tests.
* Explore the various dimensions of what it is for a multithreaded application to be "correct" (deadlocks, etc).

4. Milestone: converge on multi-core performance.
* Extend previous performance benchmarks to multi-core ones, if that's possible.
* Develop uniquely multi-core based benchmarks.
* Measure, develop emprical evidence for results.
* Act as appropriate.
* Note: be especially wary of interesting hardware concerns.
* Repeat until satisfied.
* Moonshot: can this be competitive with single-threaded version? Can we only pay concurrency cost if "needed"?
