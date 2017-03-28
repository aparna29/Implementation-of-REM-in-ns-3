# Implementation of Random Exponential Marking (REM) in ns-3

## Course Code CO365

## Assignment : #GP4

### Overview

Random Exponential Marking (REM) [1] is an active queue management algorithm like Random Early Detection (RED) [2], but differs in the following aspects: (i) the way congestion is measured, and (ii) how the measured congestion is related to the dropping probability. REM is implemented in ns-2 [3], and this project aims to implement it in ns-3 [4].

### REM Example

An example program for REM has been provided in

`src/examples/traffic-control/rem-example.cc`

and should be executed as

`./waf --run rem-example`

## References
[1] Athuraliya, S., Low, S. H., Li, V. H., & Yin, Q. (2001). REM: Active queue management. IEEE network, 15(3), 48-53.

[2] Floyd, S., & Jacobson, V. (1993). Random early detection gateways for congestion avoidance. IEEE/ACM Transactions on Networking (ToN), 1(4), 397-413.

[3] The ns-2 network simulator (http://www.isi.edu/nsnam/)

[4] The ns-3 network simulator (http://www.nsnam.org/)
