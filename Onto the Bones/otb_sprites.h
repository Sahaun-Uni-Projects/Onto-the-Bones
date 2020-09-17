class Sprite {
	int width, height, image_index, image_number;
	int xoffset, yoffset;
	std::vector<int> textures;
	std::string path;

	void parse_path() {
		int pathLen = path.size();
		int dirLen = path.find_last_of('/')+1;

		for (int i = dirLen; i < pathLen; ++i) {
			std::string dir = path.substr(0, dirLen);

			while (i < pathLen) {
				if (path[i] == '|') break;
				dir += path[i++];
			}

			textures.push_back(iLoadImage(to_cstring(dir)));
		}
	}

	public:
		Sprite(int width, int height, std::string path, int xoffset = 0, int yoffset = 0) : width(width), height(height), path(path), xoffset(xoffset), yoffset(yoffset) {
			parse_path();
			image_index = 0;
			image_number = textures.size();
		}

		int get_width() {
			return this->width;
		}

		int get_height() {
			return this->height;
		}

		std::string get_path() {
			return this->path;
		}

		int get_image_index() {
			return this->image_index;
		}

		void set_image_index(int index) {
			this->image_index = index;
		}

		void change_index_relative(int di) {
			this->image_index += di;
			if (this->image_index >= this->image_number) this->image_index = 0;
				else if (this->image_index < 0) this->image_index = this->image_number-1;
		}

		int get_tex() {
			return this->textures[image_index];
		}

		int get_tex(int index) {
			return this->textures[index];
		}

		int get_xoffset() {
			return this->xoffset;
		}

		int get_yoffset() {
			return this->yoffset;
		}
};

Sprite *sNoone;
Sprite *sPlayer;
Sprite *sBat, *sSnail, *sHound;
Sprite *sBone;
Sprite *sHeartFull, *sHeartEmpty;
Sprite *sGround, *sGroundAlert, *sGroundHighlighted;
Sprite *bgSky, *bgCredits, *bgHighscores;
Sprite *sHP[4];

void sprites_init() {
	sNoone  = new Sprite(16, 16, "Sprites/sNoone.png");
	sPlayer = new Sprite(37, 32, "Sprites/sDog1.png|sDog2.png", 3);
	sBat    = new Sprite(32, 32, "Sprites/sBat1.png|sBat2.png|sBat1.png");
	sSnail  = new Sprite(41, 31, "Sprites/sSnail1.png|sSnail2.png|sSnail3.png");
	sHound  = new Sprite(57, 32, "Sprites/sHound1.png|sHound4.png", -4);

	sBone		= new Sprite(24, 24, "Sprites/sBone.png");
	sHeartFull  = new Sprite(16, 16, "Sprites/sHeartFull.png");
	sHeartEmpty = new Sprite(16, 16, "Sprites/sHeartEmpty.png");

	sGround			   = new Sprite(48, 96, "Sprites/sGround.png");
	sGroundAlert	   = new Sprite(48, 96, "Sprites/sGroundAlert.png");
	sGroundHighlighted = new Sprite(48, 96, "Sprites/sGroundHighlighted.png");
	
	bgSky = new Sprite(1350, 540, "Sprites/bgSky.png");
	bgCredits = new Sprite(960, 540, "Sprites/bgCredits.png");
	bgHighscores = new Sprite(960, 540, "Sprites/bgHighscores.png");
	
	int width = 9, inc = 7;
	sHP[0] = sNoone;
	for (int i = 1; i <= 3; ++i) {
		sHP[i] = new Sprite(width, 9, "Sprites/sHP"+std::to_string(long long(i))+".png");
		width += inc;
	}
}

void draw_sprite_ext(int x, int y, Sprite* spr, int xscale, int yscale) {
	if (xscale == 1) iShowImage(x, y, spr->get_width(), spr->get_height(), spr->get_tex());
		else if (xscale == -1) iShowImage(x+spr->get_width(), y, -spr->get_width(), spr->get_height(), spr->get_tex());
}

void draw_sprite(int x, int y, Sprite* spr) {
	draw_sprite_ext(x, y, spr, 1, 1);
}
