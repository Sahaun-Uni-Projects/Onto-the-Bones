Sprite* levelInstruction;

std::vector<std::string> get_level_layout(int level) {
	std::vector<std::string> layout;
	levelInstruction = sNoone;

	switch (level) {
		case 1:
			layout.push_back("P 1 1 1 1 1 1 1 1 1 1 1 1 G");
		break;

		case 2:			
			layout.push_back("0 0 0 0 1 1 1 0 1");
			layout.push_back("0 0 0 0 1 0 0 0 1");
			layout.push_back("0 0 0 0 1 0 1 1 1");
			layout.push_back("1 1 1 1 1 0 1 0 1");
			layout.push_back("1 P 1 1 1 0 1 0 G");
			layout.push_back("1 1 1 1 1 0 1 0 0");
			layout.push_back("0 0 0 0 1 0 1 1 1");
			layout.push_back("0 0 0 0 1 0 0 0 1");
			layout.push_back("0 0 0 0 1 0 0 0 1");
			layout.push_back("0 0 0 0 1 1 1 1 1");
		break;

		case 3:
			layout.push_back("a 1 1 1 1 1 1 1 1 1 1 1 1 1 1");
			layout.push_back("1 1 P 1 1 1 1 1 1 1 1 1 1 1 G");
		break;

		case 4:
			layout.push_back("1 1 1 1 1 1 1 1 1 1 1");
			layout.push_back("1 1 1 0 0 1 1 1 0 1 1");
			layout.push_back("1 1 b 0 0 1 1 1 0 1 c");
			layout.push_back("P 1 1 1 1 1 0 1 0 1 G");
			layout.push_back("1 1 1 0 0 1 0 1 1 1 0");
			layout.push_back("1 1 1 0 0 1 0 1 1 1 1");
		break;

		case 5:
			layout.push_back("1 1 1 0 0 1 1 1 0 a 1");
			layout.push_back("1 1 1 0 0 1 1 1 0 1 1");
			layout.push_back("P 1 1 1 1 1 0 1 0 1 G");
			layout.push_back("1 1 1 0 0 1 0 1 1 1 0");
		break;

		case 6:
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 c 1 1 0 0 0 0");
			layout.push_back("1 1 1 c G c 1 1 1 1 1");
			layout.push_back("1 1 1 1 c P 1 1 1 1 1");
			layout.push_back("1 1 1 1 1 1 1 1 1 1 1");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
		break;

		case 7:
			layout.push_back("c 1 1 1 1 b 1 1");
			layout.push_back("1 0 0 1 1 0 0 1");
			layout.push_back("1 0 0 1 b 0 0 1");
			layout.push_back("1 0 0 1 1 0 0 1");
			layout.push_back("P 1 1 G a 1 1 1");
		break;

		case 8:
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 c 0 0 0 0");
			layout.push_back("1 1 1 1 1 c 1 1 1 1 1");
			layout.push_back("P 1 1 1 c G 1 1 1 1 1");
			layout.push_back("1 1 1 1 1 c 1 1 1 1 1");
			layout.push_back("0 0 0 0 1 1 c 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
			layout.push_back("0 0 0 0 1 1 1 0 0 0 0");
		break;

		default:
		break;
	}

	std::reverse(layout.begin(), layout.end());
	return layout;
}