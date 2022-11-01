#pragma region TODO
/*
use karatsuba algorithm or fft for multiplication
*/
#pragma endregion

#pragma once
#include <algorithm>
#include <complex>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

using std::pair;
using std::vector;
using std::string;
using std::ostringstream;

using ld = long double;
using cmplx = std::complex<ld>;

const ld _error = 1e-12;

class polynomial {
	vector<pair<cmplx, int>> poly;

public:

	polynomial() {
		poly = { { 0,0 } };
	}

	polynomial(const cmplx& z) {
		poly.emplace_back(z, 0);
	}

	polynomial(const std::initializer_list<pair<cmplx, int>>& il) {
		poly = il;
	}

	const auto& get_poly() const {
		return poly;
	}

	bool iszero() const {
		return !(abs(poly[0].first) > _error || poly[0].second);
	}

	void add(const cmplx& a, int b) {
		poly.emplace_back(a, b);
		this->shorten();
	}

	void shorten() {
		for (int i = 0; i < (int)poly.size(); i++)
			for (int j = i + 1; j < (int)poly.size(); j++)
				if (poly[i].second == poly[j].second) {
					poly[i].first += poly[j].first;
					poly.erase(poly.begin() + j--);
				}
		for (int i = 0; i < (int)poly.size(); i++)
			if (abs(poly[i].first) <= _error)
				poly.erase(poly.begin() + i--);

		if (poly.empty())
			poly.emplace_back(0.0, 0);
		sort(poly.begin(), poly.end(), [](pair<cmplx, int> a, pair<cmplx, int> b) {return a.second > b.second; });
	}

	polynomial& operator += (const polynomial& rhs) {
		this->poly.insert(this->poly.end(), rhs.poly.begin(), rhs.poly.end());
		this->shorten();
		return *this;
	}
	polynomial& operator += (const cmplx& rhs) {
		this->poly.emplace_back(rhs, 0);
		this->shorten();
		return *this;
	}

	polynomial& operator -= (const polynomial& rhs) {
		for (int i = 0; i < (int)rhs.poly.size(); i++)
			this->poly.emplace_back(-rhs.poly[i].first, rhs.poly[i].second);
		this->shorten();
		return *this;
	}
	polynomial& operator -= (const cmplx& rhs) {
		return *this += -rhs;
	}

	polynomial& operator *= (const polynomial& rhs) {
		polynomial temp;
		for (int i = 0; i < (int)poly.size(); i++)
			for (int j = 0; j < (int)rhs.poly.size(); j++)
				temp.poly.emplace_back(poly[i].first * rhs.poly[j].first, poly[i].second + rhs.poly[j].second);
		temp.shorten();
		*this = temp;
		return *this;
	}
	polynomial& operator *= (const cmplx& rhs) {
		for (int i = 0; i < (int)poly.size(); i++)
			this->poly[i].first *= rhs;
		return *this;
	}

	polynomial& operator /= (const polynomial& rhs) {
		polynomial res_temp, res, div = *this;
		if (div.iszero())
			return *this;
		if (rhs.iszero()) {
			throw new std::exception("division by zero");
		}

		int n = 30;
		while (n && !div.iszero()) {
			if (div.poly[0].second <= 0)
				n--;
			res_temp.poly.clear();
			res_temp.poly.emplace_back(div.poly[0].first / rhs.poly[0].first, div.poly[0].second - rhs.poly[0].second);
			res += res_temp;
			res_temp *= rhs;
			div -= res_temp;
			div.shorten();
		}
		res.shorten();
		*this = res;
		return *this;
	}
	polynomial& operator /= (const cmplx& rhs) {
		return *this *= (cmplx(1) / rhs);
	}

	polynomial operator ^ (uint32_t p) const {
		polynomial res, a = *this;
		res.add(1, 0);
		for (; p; p /= 2) {
			if (p % 2 != 0)
				res *= a;
			a *= a;
		}
		return res;
	}

	polynomial& operator=(polynomial rhs) {
		swap(*this, rhs);
		return *this;
	}

	cmplx value(const cmplx& z) const {
		cmplx res = 0;
		for (int i = 0; i < (int)poly.size(); i++)
			res += poly[i].first * pow(z, poly[i].second);
		return res;
	}

	polynomial derive() const {
		polynomial d;
		for (auto &i : poly)
			d.add(i.first * cmplx(i.second), i.second - 1);
		return d;
	}

	vector<cmplx> Laguerre() {
		shorten();
		vector<cmplx> res;
		int n = 0;
		for (auto &i : poly)
			n = std::max(n, i.second);

		polynomial X = { { 1, 1 } };
		polynomial p = *this;

		for (int i = 0; i < n; i++) {
			cmplx x = 0;
			polynomial p1 = p.derive();
			polynomial p11 = p1.derive();

			for (int k = 0;; k++) {
				if (abs(p.value(x)) < _error)
					break;

				const cmplx G = p1.value(x) / p.value(x);
				const cmplx H = G * G - p11.value(x) / p.value(x);
				const cmplx d1 = G + sqrt(cmplx(n - 1) * (cmplx(n) * H - G * G));
				const cmplx d2 = G - sqrt(cmplx(n - 1) * (cmplx(n) * H - G * G));
				const cmplx a = cmplx(n) / (abs(d1) > abs(d2) ? d1 : d2);
				if (abs(a) < _error)
					break;
				x -= a;
			}

			res.push_back(x);
			X -= x;
			p /= X;
			X = polynomial({ { 1, 1 } });
		}
		return res;
	}

	static bool iszero(ld d) {
		return fabsl(d) < _error;
	}

	static string cmplx_to_string(const cmplx& z) {
		const bool b1 = iszero(z.real()), b2 = iszero(z.imag());
		ostringstream oss;
		if (b1 && b2)
			oss << "0";
		else if (b1 && !b2)
			oss << z.imag() << 'i';
		else if (!b1 && b2)
			oss << z.real();
		else
			oss << '(' << z.real() << " + " << z.imag() << "i)";
		return oss.str();
	}

	string term_to_string(const pair<cmplx, int>& p) {
		ostringstream oss;
		if (iszero(p.first.real()) && iszero(p.first.imag()))	// if p.first == 0
			oss << "";
		else if (p.second == 0)
			oss << cmplx_to_string(p.first);
		else {
			const bool b1 = fabsl(p.first.real() - 1) < _error && iszero(p.first.imag());	// if p.first == 1
			const bool b2 = p.second == 1;

			if (b1 && b2)
				oss << "x";
			else if (b1 && !b2)
				oss << "x^" << p.second;
			else if (!b1 && b2)
				oss << cmplx_to_string(p.first) << " * x";
			else
				oss << cmplx_to_string(p.first) << " * x^" << p.second;
		}
		return oss.str();
	}

	string to_string() {
		shorten();
		string out = "P(x) = ";
		if (iszero())
			out += "0";
		else {
			for (int i = 0; i < (int)poly.size(); i++) {
				const string cur = term_to_string(poly[i]);
				if (cur.empty())
					continue;
				out += cur;
				if (i != (int)poly.size() - 1)
					out += " + ";
			}
		}
		return out;
	}

	friend std::ostream& operator<<(std::ostream& out, polynomial& p) {
		out << p.to_string();
		return out;
	}

	friend void swap(polynomial& first, polynomial& second) {
		std::swap(first.poly, second.poly);
	}
};
