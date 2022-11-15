
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

// int main(int argc, char** argv) {
// 	test();
//   return 0;
// }
// void test()
// {
//   int p,q,r,s,u,v;
//   do{
//   p=q+r;
//   s=p+q;
//   u=s*v;
//   if(v>0)
//   {
//     v=r+u;
//   }
//   else{
//     q=s*u;
//   }
//   q=v+r;
//   } while(q>0);
// }

// void test() {
//   int a, b, c, d, e, f;
//   a = 0;
//   c = 1;
//   do
//   {
//       c = a + 1;
//       c = c * b;
//       if (b > 9) {
//           f = d * c;
//           c = f - 3;
//       } else {
//           a = e + 1;
//           e = d / 2;
//       }
//       a = b;
//   } while (a < 9);
//   a = a + 1;
// }

// int main(int argc, char** argv) {
// 	test();
//     return 0;
// }