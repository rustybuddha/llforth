\ RUN: %{compile} %t && %t | FileCheck %s

: main

branch .bar
.foo:
999999 .
branch .end
.bar:
888888 .
branch .foo

.end:
bye

;

\ CHECK: 888888 999999