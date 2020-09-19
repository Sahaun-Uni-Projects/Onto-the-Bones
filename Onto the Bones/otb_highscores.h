struct Score {
	std::string name;
	int tries, kills;

	Score() {}
	Score(std::string name, int kills, int tries) : name(name), kills(kills), tries(tries) {}

    friend std::istream& operator >> (std::istream& in, Score& score) {
        in >> score.name >> score.tries >> score.kills;
        return in;
    }

    friend std::ostream& operator << (std::ostream& out, Score& score) {
		out << score.name << " " << score.tries << " " << score.kills;
        return out;
    }
};
std::vector<Score> Highscores;
std::fstream fileHighscore;

void highscore_clear() {
	Highscores.clear();
}

void highscore_prune() {
	std::sort(Highscores.begin(), Highscores.end(), [](Score& s1, Score& s2) -> bool {
		if (s1.kills == s2.kills) return (s1.tries < s2.tries);
		return (s1.kills > s2.kills);
	});
	while (int(Highscores.size()) > MAX_HIGHSCORE_COUNT) Highscores.pop_back();
}

void highscore_save() {
	remove(FILE_HIGHSCORE);
	fileHighscore.open(FILE_HIGHSCORE, std::fstream::in | std::fstream::out | std::fstream::app);
	for (int i = 0; i < int(Highscores.size()); ++i) fileHighscore <<  Highscores[i] << "\n";
	fileHighscore.close();
}

void highscore_push(Score& score) {
	Highscores.push_back(score);
	highscore_prune();
	highscore_save();
}

void highscore_load() {
	highscore_clear();
	
	Score score;

	fileHighscore.open(FILE_HIGHSCORE, std::fstream::in | std::fstream::out | std::fstream::app);
	while (fileHighscore >> score) Highscores.push_back(score);
	fileHighscore.close();

	highscore_prune();
}