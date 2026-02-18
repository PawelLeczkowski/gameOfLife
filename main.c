#include <pcl.h>
#include <time.h>

#define WIDTH 30
#define HEIGHT 20

int main(void) {
	struct Console* console = start();
	struct AsciiScreen* ascii = initascii(console);

	setforegroundcolorascii(ascii, 0xFF,0x69, 0xB4);

	char plane[HEIGHT][WIDTH];

	int response = 0;
	while (response != 'y' && response != 'n') {
		clearascii(ascii);
		setstringascii(ascii, "Only glider? y/n");
		refreshascii(console, ascii);
		response = getcharacter(console);
	}

	srand(time(NULL));
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			plane[i][j] = '.';
			int r = rand() % 2;
			if (r == 0 && response == 'n') {
				plane[i][j] = '#';
			}
		}
	}

	if (response == 'y') {
		// glider
		plane[1][0] = '#';
		plane[2][1] = '#';
		plane[0][2] = '#';
		plane[1][2] = '#';
		plane[2][2] = '#';
	}

	setinputblock(console, FALSE);
	hidecursorascii(ascii);

	int generations = 0;
	int menusize = 2;
	int sleep = 100;

	while (1) {
		clearascii(ascii);

		setstringformattedascii(ascii, "Generation: %d | speed: %d | space => enter insert mode", generations, sleep);
		setstringascii(ascii, "\nesc => exit");

		int input = getcharacter(console);
		if (input == KEY_ESC) {
			break;
		}

		switch (input) {
			case '+': {
				if (sleep == 300)
					break;
				sleep += 10;
				break;
			}
			case '-': {
				if (sleep == 0)
					break;
				sleep -= 10;
				break;
			}
			default: break;
		}

		if (input == ' ') {
			int posrow = 0, poscol = 0;
			while (1) {
				clearascii(ascii);
				setstringformattedascii(ascii, "Generation: %d | space => exit view mode", generations);
				setstringascii(ascii, "\narrows => movement | r => new draw | n => next step | e => place cell");

				input = getcharacter(console);
				if (input == ' ') {
					break;
				}

				switch (input) {
					case KEY_UP: {
						if (posrow == 0)
							break;
						posrow--;
						break;
					}
					case KEY_LEFT: {
						if (poscol == 0)
							break;
						poscol--;
						break;
					}
					case KEY_DOWN: {
						if (posrow == HEIGHT - 1)
							break;
						posrow++;
						break;
					}
					case KEY_RIGHT: {
						if (poscol == WIDTH - 1)
							break;
						poscol++;
						break;
					}
					case 'r': {
						for (int i = 0; i < HEIGHT; i++) {
							for (int j = 0; j < WIDTH; j++) {
								int r = rand() % 2;
								if (r == 0) {
									plane[i][j] = '.';
								}
								else {
									plane[i][j] = '#';
								}
							}
						}
						break;
					}
					case 'n': {
						char new[HEIGHT][WIDTH];
						for (int i = 0; i < HEIGHT; ++i) {
							for (int j = 0; j < WIDTH; ++j) {
								new[i][j] = '.';
								int neighbours = 0;

								for (int k = -1; k <= 1; ++k) {
									for (int l = -1; l <= 1; ++l) {
										if (k == 0 && l == 0)
											continue;

										int row = (i + k + HEIGHT) % HEIGHT;
										int col = (j + l + WIDTH) % WIDTH;

										if (plane[row][col] == '#') {
											neighbours++;
										}
									}
								}

								if (plane[i][j] == '.' && neighbours == 3) {
									new[i][j] = '#';
								}
								else if (plane[i][j] == '#' && (neighbours == 2 || neighbours == 3)) {
									new[i][j] = '#';
								}
							}
						}

						memcpy(plane, new, WIDTH * HEIGHT * sizeof(char));
						break;
					}
					case 'e': {
						plane[posrow][poscol] = '#';
						break;
					}
					default: break;
				}

				set2darrayascii(ascii, (char*)plane, 0 + menusize, 0, WIDTH, HEIGHT);

				setcharcursorascii(ascii, '@', posrow + menusize, poscol);

				refreshascii(console, ascii);
			}
		}

		char new[HEIGHT][WIDTH];
		for (int i = 0; i < HEIGHT; ++i) {
			for (int j = 0; j < WIDTH; ++j) {
				new[i][j] = '.';
				int neighbours = 0;

				for (int k = -1; k <= 1; ++k) {
					for (int l = -1; l <= 1; ++l) {
						if (k == 0 && l == 0)
							continue;

						int row = (i + k + HEIGHT) % HEIGHT;
						int col = (j + l + WIDTH) % WIDTH;

						if (plane[row][col] == '#') {
							neighbours++;
						}
					}
				}

				if ((plane[i][j] == '.' && neighbours == 3) || (plane[i][j] == '#' && (neighbours == 2 || neighbours == 3))) {
					new[i][j] = '#';
				}
			}
		}

		memcpy(plane, new, WIDTH * HEIGHT * sizeof(char));

		set2darrayascii(ascii, (char*)plane, 0 + menusize, 0, WIDTH, HEIGHT);

		generations++;
		refreshascii(console, ascii);
		_sleep(sleep);
	}

	end(console);

	system("pause");
	return 0;
}
