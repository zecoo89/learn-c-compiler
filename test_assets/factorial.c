factorial(n) {
  ans = 1;

  for(i=1;i<=n;i=i+1) {
    ans = ans * i;
  }

  return ans;
}

main() {
  return factorial(4);
}
