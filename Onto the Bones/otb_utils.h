template<typename T>
void for_each(std::vector<T> vec, std::function<void(T)> func) {
	for (int i = 0; i < int(vec); ++i) func(vec[i]);
}

long long int round(double num) {
	long long int f = floor(num);
	return (((num-f) >= 0.5) ? (f) : (f+1));
}