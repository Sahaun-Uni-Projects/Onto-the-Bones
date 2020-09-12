char* to_cstring(const char* text) {
	return ((char*)text);
}

char* to_cstring(std::string& text) {
	return ((char*)text.c_str());
}

int string_length(char* text) {
	return strlen(text);
}

int string_length(std::string text) {
	return text.size();
}

int string_width(char* text, int size = 10) {
	return (string_length(text) * size);
}

int string_width(std::string text, int size = 10) {
	return (string_length(text) * size);
}