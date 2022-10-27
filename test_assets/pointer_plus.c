int main() {
  int* p;
  alloc_four(&p, 1, 2, 4, 8);
  int *q;
  q = p + 2;
  *q;
  q = p + 3;
  return *q;
}
