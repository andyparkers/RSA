#include "pch.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct Data {
	int64_t n = 0;
	int64_t p = 0;
	int64_t q = 0;
	int64_t eiler_function = 0;
	int64_t e = 0;
	int64_t d = 0;
};

class RSA {
private:
	Data data;
	const string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZабвгдежзийклмнопрстуфхцчшщъыьэюя ,.:!?";

public:
	Data GetRSA() const {
		return data;
	}

	void SetN(int64_t value) {
		data.n = value;
	}

	void SetD(int64_t value) {
		data.d = value;
	}

	friend ostream& operator<<(ostream& out, const RSA& value) {
		out << "N is " << value.data.n << ", p is " << value.data.p << ", q is " << value.data.q
			<< ", Eiler function value is " << value.data.eiler_function << ", e is " << value.data.e
			<< ", d is " << value.data.d << ".";
		return out;
	}

	void MakeKeys() {
		FactorizeAndFindEilerFunction();
		FindEAndD();
	}

	vector<int64_t> Encrypt(const string& text) const {
		vector<int64_t> result;
		result.reserve(text.size());
		for (const auto& letter : text) {
			result.push_back(GetReminder(static_cast<int64_t>(alphabet.find(letter)), data.e, data.n));
		}
		return result;
	}

	string Decrypt(const vector<int64_t>& cipher_text) const {
		string answer = "";
		for (const auto& elem : cipher_text) {
			answer += alphabet[static_cast<size_t>(GetReminder(elem, data.d, data.n))];
		}
		return answer;
	}

private:
	void FactorizeAndFindEilerFunction() {
		int64_t x = data.n;
		vector<int64_t> factors;
		for (int64_t i = 2; i <= sqrt(x); ++i) {
			while (x % i == 0) {
				factors.push_back(i);
				x /= i;
			}
		}
		if (x != 1) {
			factors.push_back(x);
		}
		data.p = factors[0];
		data.q = factors[1];
		data.eiler_function = (data.p - 1) * (data.q - 1);
	}

	void FindEAndD() {
		vector<int64_t> e_try = { 257, 65537 };
		for (const auto& e : e_try) {
			if (IsCoprime(e, data.eiler_function)) {
				data.e = e;
				break;
			}
		}
		if (data.e == 0) {
			for (int64_t e = 20000; e < data.eiler_function; ++e) {
				if (IsCoprime(e, data.eiler_function)) {
					data.e = e;
					break;
				}
			}
		}
		for (int64_t k = 0; ; ++k) {
			double d = (k * data.eiler_function + 1) / static_cast<double>(data.e);
			if (floor(d) == d) {
				data.d = static_cast<int64_t>(d);
				break;
			}
		}
	}

	int64_t GetReminder(int64_t base, int64_t power, int64_t devider) const {
		int64_t answer = 1;
		while (power) {
			if (power % 2 == 0) {
				base *= base;
				base %= devider;
				power /= 2;
			}
			else {
				--power;
				answer *= base;
				answer %= devider;
			}
		}
		answer %= devider;
		return answer >= 0 ? answer : answer + devider;
	}

	int Gcd(int a, int b, int& x, int& y) const {
		if (a == 0) {
			x = 0; y = 1;
			return b;
		}
		int x1, y1;
		int d = Gcd(b % a, a, x1, y1);
		x = y1 - (b / a) * x1;
		y = (x1 > 0 ? x1 : x1 + b);
		return d;
	}

	[[nodiscard]] bool IsCoprime(int64_t a, int64_t b) const {
		for (int64_t gcd = a; ; gcd = b, b = a % b, a = gcd) {
			if (!b) {
				return gcd == 1;
			}
		}
		return 0;
	}
};

template<typename ADT>
ostream& operator<<(ostream& out, const vector<ADT>& vec) {
	out << "{ "s;
	bool first = 1;
	for (const auto& elem : vec) {
		if (!first) {
			out << ", "s;
		}
		first = 0;
		out << elem;
	}
	out << " }"s;
	return out;
}

int main()
{
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	string operation;
	while (1) {
		cout << "Enter \"encrypt\" or \"decrypt\", according to you need: ";
		getline(cin, operation);
		if (operation == "encrypt") {
			RSA key;
			cout << "Enter \"n\" value: ";
			int64_t n = 0;
			cin >> n;
			key.SetN(n);
			cin.ignore();
			key.MakeKeys();
			cout << "Public key:  { " << key.GetRSA().e << ", " << key.GetRSA().n << " }" << endl;
			cout << "Private key: { " << key.GetRSA().d << ", " << key.GetRSA().n << " }" << endl;
			string text = "";
			cout << "Enter text: ";
			getline(cin, text);
			auto cipher_text = key.Encrypt(text);
			cout << "Encrypted text: " << cipher_text << endl;
		}
		else if (operation == "decrypt") {
			RSA key;
			string data;
			cout << "Enter numbers, diveded with comma followed with space: ";
			getline(cin, data);
			vector<int64_t> cipher_text;
			string number = "";
			for (const auto& letter : data) {
				if (letter != ' ') {
					number += letter;
				}
				else {
					cipher_text.push_back(stoi(number));
					number.clear();
				}
			}
			cipher_text.push_back(stoi(number));
			int64_t d, n;
			cout << "Enter \"d\" and \"n\", separated with space: ";
			cin >> d;
			cin.ignore();
			cin >> n;
			key.SetD(d);
			key.SetN(n);
			cout << "Decrypted text: " << key.Decrypt(cipher_text) << endl;
			cin.ignore();
		}
		else {
			cout << "Wrong query, try again please" << endl;
		}
	}
}
