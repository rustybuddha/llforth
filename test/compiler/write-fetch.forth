\ RUN: %{compile} %t && %t | FileCheck %s

state @ .
999999 state ! state @ .
bye

\ CHECK: 0 999999