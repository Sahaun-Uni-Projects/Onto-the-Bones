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
Sprite *sGround, *sGroundAlert, *sGroundHighlighted;
Sprite *bgSky;
void sprites_init() {
	sNoone  = new Sprite(16, 16, ("Sprites/Blank.png"));
	sPlayer = new Sprite(32, 32, ("Sprites/Player.png"));
	sEnemy  = new Sprite(32, 32, ("Sprites/Enemy.png"));

	sGround = new Sprite(48, 96, ("Sprites/sGround.png"));
	sGroundAlert = new Sprite(48, 96, ("Sprites/sGroundAlert.png"));
	sGroundHighlighted = new Sprite(48, 96, ("Sprites/sGroundHighlighted.png"));

	bgSky = new Sprite(1350, 540, ("Sprites/bgSky.png"));
}

void draw_sprite_ext(int x, int y, Sprite* spr, int xscale, int yscale) {
	if (xscale == 1) iShowImage(x, y, spr->get_width(), spr->get_height(), spr->get_tex());
		else iShowImage(x+spr->get_width(), y, -spr->get_width(), spr->get_height(), spr->get_tex());
}

void draw_sprite(int x, int y, Sprite* spr) {
	draw_sprite_ext(x, y, spr, 1, 1);
}
