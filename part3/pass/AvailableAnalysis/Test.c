
void test() {
  int a, b, c, d, e, f;
  e = b + c;
  if (e > 0) {
    e = a;
  } else {
    e = b + c;
  }
  a = e + c;
}

int main(int argc, char** argv) {
	test();
  return 0;
}
