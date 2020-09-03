class Sprite {
	int width, height, tex;
	char* path;

	public:
		Sprite(int width, int height, char* path) : width(width), height(height), path(path) {
			this->tex = iLoadImage(path);
		}

		int get_width() {
			return this->width;
		}

		int get_height() {
			return this->height;
		}

		char* get_path() {
			return this->path;
		}

		int get_tex() {
			return this->tex;
		}
};

Sprite *sNoone, *sPlayer, *sEnemy;
void sprites_init() {
	sNoone  = new Sprite(16, 16, ("Sprites/Blank.png"));
	sPlayer = new Sprite(32, 32, ("Sprites/Player.png"));
	sEnemy  = new Sprite(32, 32, ("Sprites/Enemy.png"));
}

void draw_sprite(int x, int y, Sprite* spr) {
	iShowImage(x, y, spr->get_width(), spr->get_height(), spr->get_tex());
}