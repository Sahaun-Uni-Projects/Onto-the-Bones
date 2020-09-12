// DS
template<typename T>
void for_each(std::vector<T> vec, std::function<void(T)> func) {
	for (int i = 0; i < int(vec); ++i) func(vec[i]);
}

// Math
long long int round(double num) {
	long long int f = floor(num);
	return (((num-f) >= 0.5) ? (f) : (f+1));
}

template<typename T>
T clamp(T n, T mn, T mx) {
	if (mn > mx) std::swap(mn, mx);
	if (n < mn) return mn;
	if (n > mx) return mx;
	return n;
}

template<typename T>
int sign(T n) {
	if (n > 0) return  1;
	if (n < 0) return -1;
	return 0;
}

// Random
int irandom_range(int mn, int mx) {
	if (mn > mx) std::swap(mn, mx);
	return (mn + (std::rand() % (mx-mn+1)));
}

int irandom(int n) {
	return irandom_range(0,n);
}