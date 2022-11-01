// multi-channel noise generator
N = 8;

polyrandom(N) = scramble(N) ~ _
    with {scramble(1) = * (1103515245) : + (12345);
    scramble(n) = scramble(1)  <: scramble(n - 1),
    _;
};

polynoise(N) = polyrandom(N) : par(i, N, / (2147483647.0));