// RUN: %gcc %s -o fact.out
// RUN: %simulator fact.out --cosim > %t
// RUN: %fc %s --input-file %t --check-prefix=CHECK02
// RUN: %fc %s --input-file %t --check-prefix=CHECK01
// RUN: %fc %s --input-file %t --check-prefix=CHECK00
// RUN: rm fact.out

unsigned fact(unsigned n) {
  // n == 2
  // CHECK02: NUM=7
  // CHECK02: x2=0xffffffd0
  // CHECK02: PC=0x00010078
  // CHECK02: -----------------------
  // CHECK02: NUM=8
  // CHECK02: M[0xffffffec]=0x000100e0
  // CHECK02: PC=0x0001007c
  // CHECK02: -----------------------
  // CHECK02: NUM=9
  // CHECK02: M[0xffffffe8]=0x00000000
  // CHECK02: PC=0x00010080
  // CHECK02: -----------------------
  // CHECK02: NUM=10
  // CHECK02: x8=0xfffffff0
  // CHECK02: PC=0x00010084
  // CHECK02: -----------------------
  // CHECK02: NUM=11
  // CHECK02: M[0xffffffdc]=0x00000002
  // CHECK02: PC=0x00010088
  // CHECK02: -----------------------
  // CHECK02: NUM=12
  // CHECK02: x15=0x00000002
  // CHECK02: PC=0x0001008c
  // CHECK02: -----------------------
  // CHECK02: NUM=13
  // CHECK02: PC=0x00010098

  // n == 1
  // CHECK01: NUM=18
  // CHECK01: x2=0xffffffb0
  // CHECK01: PC=0x00010078
  // CHECK01: -----------------------
  // CHECK01: NUM=19
  // CHECK01: M[0xffffffcc]=0x000100a8
  // CHECK01: PC=0x0001007c
  // CHECK01: -----------------------
  // CHECK01: NUM=20
  // CHECK01: M[0xffffffc8]=0xfffffff0
  // CHECK01: PC=0x00010080
  // CHECK01: -----------------------
  // CHECK01: NUM=21
  // CHECK01: x8=0xffffffd0
  // CHECK01: PC=0x00010084
  // CHECK01: -----------------------
  // CHECK01: NUM=22
  // CHECK01: M[0xffffffbc]=0x00000001
  // CHECK01: PC=0x00010088
  // CHECK01: -----------------------
  // CHECK01: NUM=23
  // CHECK01: x15=0x00000001
  // CHECK01: PC=0x0001008c
  // CHECK01: -----------------------
  // CHECK01: NUM=24
  // CHECK01: PC=0x00010098

  // n == 0
  // CHECK00: NUM=29
  // CHECK00: x2=0xffffff90
  // CHECK00: PC=0x00010078
  // CHECK00: -----------------------
  // CHECK00: NUM=30
  // CHECK00: M[0xffffffac]=0x000100a8
  // CHECK00: PC=0x0001007c
  // CHECK00: -----------------------
  // CHECK00: NUM=31
  // CHECK00: M[0xffffffa8]=0xffffffd0
  // CHECK00: PC=0x00010080
  // CHECK00: -----------------------
  // CHECK00: NUM=32
  // CHECK00: x8=0xffffffb0
  // CHECK00: PC=0x00010084
  // CHECK00: -----------------------
  // CHECK00: NUM=33
  // CHECK00: M[0xffffff9c]=0x00000000
  // CHECK00: PC=0x00010088
  // CHECK00: -----------------------
  // CHECK00: NUM=34
  // CHECK00: x15=0x00000000
  // CHECK00: PC=0x0001008c
  // CHECK00: -----------------------
  // CHECK00: NUM=35
  // CHECK00: PC=0x00010090
  if (n < 1) {
    // n == 0
    // CHECK00: NUM=36
    // CHECK00: x15=0x00000001
    // CHECK00: PC=0x00010094
    // CHECK00: -----------------------
    // CHECK00: NUM=37
    // CHECK00: PC=0x000100b4
    // CHECK00: -----------------------
    // CHECK00: NUM=38
    // CHECK00: x10=0x00000001
    // CHECK00: PC=0x000100b8
    // CHECK00: -----------------------
    // CHECK00: NUM=39
    // CHECK00: x1=0x000100a8
    // CHECK00: PC=0x000100bc
    // CHECK00: -----------------------
    // CHECK00: NUM=40
    // CHECK00: x8=0xffffffd0
    // CHECK00: PC=0x000100c0
    // CHECK00: -----------------------
    // CHECK00: NUM=41
    // CHECK00: x2=0xffffffb0
    // CHECK00: PC=0x000100c4
    // CHECK00: -----------------------
    // CHECK00: NUM=42
    // CHECK00: PC=0x000100a8
    return 1;
  }

  // n == 2
  // CHECK02: NUM=14
  // CHECK02: x15=0x00000002
  // CHECK02: PC=0x0001009c
  // CHECK02: -----------------------
  // CHECK02: NUM=15
  // CHECK02: x15=0x00000001
  // CHECK02: PC=0x000100a0
  // CHECK02: -----------------------
  // CHECK02: NUM=16
  // CHECK02: x10=0x00000001
  // CHECK02: PC=0x000100a4
  // CHECK02: -----------------------
  // CHECK02: NUM=17
  // CHECK02: x1=0x000100a8
  // CHECK02: PC=0x00010074
  //
  // ...... calling fact(1)
  //
  // CHECK02: NUM=51
  // CHECK02: x14=0x00000001
  // CHECK02: PC=0x000100ac
  // CHECK02: -----------------------
  // CHECK02: NUM=52
  // CHECK02: x15=0x00000002
  // CHECK02: PC=0x000100b0
  // CHECK02: -----------------------
  // CHECK02: NUM=53
  // CHECK02: x15=0x00000002
  // CHECK02: PC=0x000100b4
  // CHECK02: -----------------------
  // CHECK02: NUM=54
  // CHECK02: x10=0x00000002
  // CHECK02: PC=0x000100b8
  // CHECK02: -----------------------
  // CHECK02: NUM=55
  // CHECK02: x1=0x000100e0
  // CHECK02: PC=0x000100bc
  // CHECK02: -----------------------
  // CHECK02: NUM=56
  // CHECK02: x8=0x00000000
  // CHECK02: PC=0x000100c0
  // CHECK02: -----------------------
  // CHECK02: NUM=57
  // CHECK02: x2=0xfffffff0
  // CHECK02: PC=0x000100c4
  // CHECK02: -----------------------
  // CHECK02: NUM=58
  // CHECK02: PC=0x000100e0

  // n == 1
  // CHECK01: NUM=25
  // CHECK01: x15=0x00000001
  // CHECK01: PC=0x0001009c
  // CHECK01: -----------------------
  // CHECK01: NUM=26
  // CHECK01: x15=0x00000000
  // CHECK01: PC=0x000100a0
  // CHECK01: -----------------------
  // CHECK01: NUM=27
  // CHECK01: x10=0x00000000
  // CHECK01: PC=0x000100a4
  // CHECK01: -----------------------
  // CHECK01: NUM=28
  // CHECK01: x1=0x000100a8
  // CHECK01: PC=0x00010074
  //
  // ...... calling fact(0)
  //
  // CHECK01: NUM=43
  // CHECK01: x14=0x00000001
  // CHECK01: PC=0x000100ac
  // CHECK01: -----------------------
  // CHECK01: NUM=44
  // CHECK01: x15=0x00000001
  // CHECK01: PC=0x000100b0
  // CHECK01: -----------------------
  // CHECK01: NUM=45
  // CHECK01: x15=0x00000001
  // CHECK01: PC=0x000100b4
  // CHECK01: -----------------------
  // CHECK01: NUM=46
  // CHECK01: x10=0x00000001
  // CHECK01: PC=0x000100b8
  // CHECK01: -----------------------
  // CHECK01: NUM=47
  // CHECK01: x1=0x000100a8
  // CHECK01: PC=0x000100bc
  // CHECK01: -----------------------
  // CHECK01: NUM=48
  // CHECK01: x8=0xfffffff0
  // CHECK01: PC=0x000100c0
  // CHECK01: -----------------------
  // CHECK01: NUM=49
  // CHECK01: x2=0xffffffd0
  // CHECK01: PC=0x000100c4
  // CHECK01: -----------------------
  // CHECK01: NUM=50
  // CHECK01: PC=0x000100a8
  return n * fact(n - 1);
}

int main() {
  // CHECK: NUM=1
  // CHECK: x2=0xfffffff0
  // CHECK: PC=0x000100cc
  // CHECK: -----------------------
  // CHECK: NUM=2
  // CHECK: M[0xfffffffc]=0x00000000
  // CHECK: PC=0x000100d0
  // CHECK: -----------------------
  // CHECK: NUM=3
  // CHECK: M[0xfffffff8]=0x00000000
  // CHECK: PC=0x000100d4
  // CHECK: -----------------------
  // CHECK: NUM=4
  // CHECK: x8=0x00000000
  // CHECK: PC=0x000100d8
  // CHECK: -----------------------
  // CHECK: NUM=5
  // CHECK: x10=0x00000002
  // CHECK: PC=0x000100dc
  // CHECK: -----------------------
  // CHECK: NUM=6
  // CHECK: x1=0x000100e0
  // CHECK: PC=0x00010074
  fact(2);

  // CHECK: NUM=59
  // CHECK: PC=0x000100e4
  asm("ecall");
}
