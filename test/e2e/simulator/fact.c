// RUN: %gcc %s -o fact
// RUN: %simulator fact --log trace

unsigned fact(unsigned n) {
  if (n < 1)
    return 1;
  return n * fact(n - 1);
}

int main() {
  fact(5);
  asm("ecall");
}
