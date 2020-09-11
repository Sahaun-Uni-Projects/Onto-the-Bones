std::vector<std::string> get_level_layout(int level) {
	std::vector<std::string> layout;

	switch (level) {
		case 1:
			layout.push_back("1 1 1 0 0 1 0 1 1 1 1");
			layout.push_back("1 1 1 0 0 1 0 1 1 1 0");
			layout.push_back("P 1 1 1 1 1 0 1 0 1 G");
			layout.push_back("1 1 b 0 0 1 1 1 0 1 c");
			layout.push_back("1 1 1 0 0 1 1 1 0 1 1");
			layout.push_back("1 1 1 1 1 1 1 1 1 1 1");
		break;

		case 2:
			layout.push_back("1 1 1 0 0 1 0 1 1 1 0");
			layout.push_back("P 1 1 1 1 1 0 1 0 1 G");
			layout.push_back("1 1 1 0 0 1 1 1 0 1 1");
			layout.push_back("1 1 1 0 0 1 1 1 0 a 1");
		break;
	}

	return layout;
}