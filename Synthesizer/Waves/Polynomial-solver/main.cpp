#include "poly.cpp"

using namespace std;

int main() {
	const polynomial x = { { 1, 1 } };
	polynomial p = (x ^ 3) + 5 * (x ^ 2) - 1;

	auto sol = p.Laguerre();
	for (auto &i : sol) {
		cout << polynomial::cmplx_to_string(i) << endl;
	}
	return 0;
}
