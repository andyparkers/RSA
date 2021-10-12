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
	Data getRSA() {
		return data;
	}

	void setN(RSA& rsa, int value) {
		rsa.data.n = value;
	}

	friend ostream& operator<<(ostream& out, const RSA& value) {
		out << "N is " << value.data.n << ", p is " << value.data.p << ", q is " << value.data.q
			<< ", Eiler function value is " << value.data.eiler_function << ", e is " << value.data.e
			<< ", d is " << value.data.d << ".";
		return out;
	}

	void factorizeAndFindEilerFunction(int x) {
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

	void findEAndD() {
		vector<int64_t> e_try = { 257, 65537 };
		for (const auto& e : e_try) {
			if (isCoprime(e, data.eiler_function)) {
				data.e = e;
				break;
			}
		}
		if (data.e == 0) {
			for (int64_t e = 20000; e < data.eiler_function; ++e) {
				if (isCoprime(e, data.eiler_function)) {
					data.e = e;
					break;
				}
			}
		}
		for (int64_t k = 0; ; ++k) {
			double d = (k * data.eiler_function + 1) / static_cast<double>(data.e);
			if (floor(d) == d) {
				data.d = d;
				break;
			}
		}
	}

	int64_t getReminder(int64_t number, int64_t power, int64_t diveder) {
		int64_t answer = number;
		for (int i = 0; i < power - 1; ++i) {
			answer = answer * number % diveder;
		}
		return answer;
	}

	vector<int64_t> fromStringToVector(const string& text) {
		vector<int64_t> result;
		for (const auto& letter : text) {
			result.push_back(getReminder(static_cast<int64_t>(alphabet.find(letter)), data.e, data.n));
		}
		return result;
	}

	vector<int64_t> encrypt(const string& text) {
		const vector<int64_t>& result = fromStringToVector(text);
		cout << "{ ";
		bool initial_time = 0;
		for (const auto& elem : result) {
			if (initial_time == 1) {
				cout << ", ";
			}
			cout << elem;
			if (initial_time == 0) {
				initial_time = 1;
			}
		}
		cout << " }" << endl;
		return result;
	}

	string decrypt(const vector<int64_t>& close_text) {
		string answer = "";
		for (const auto& elem : close_text) {
			answer += alphabet[getReminder(elem, data.d, data.n)];
		}
		return answer;
	}

	int gcd(int a, int b, int& x, int& y) {
		if (a == 0) {
			x = 0; y = 1;
			return b;
		}
		int x1, y1;
		int d = gcd(b % a, a, x1, y1);
		x = y1 - (b / a) * x1;
		y = (x1 > 0 ? x1 : x1 + b);
		return d;
	}

	bool isCoprime(int a, int b) {
		for (int gcd = a; ; gcd = b, b = a % b, a = gcd) {
			if (!b) {
				return gcd == 1;
			}
		}
		return 0;
	}
};


int main()
{
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	RSA key;
	cout << "Enter \"n\" value: ";
	int n = 0;
	cin >> n;
	key.setN(key, n);
	cin.ignore();
	key.factorizeAndFindEilerFunction(key.getRSA().n);
	key.findEAndD();
	cout << "Public key:  { " << key.getRSA().e << ", " << key.getRSA().n << " }" << endl;
	cout << "Private key: { " << key.getRSA().d << ", " << key.getRSA().n << " }" << endl;
	while (1) {
		string open_text = "";
		cout << "Enter text to encrypt: ";
		getline(cin, open_text);
		cout << key.decrypt(key.encrypt(open_text)) << endl;
	}
}
