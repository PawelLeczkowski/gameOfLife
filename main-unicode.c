#include <pcl.h>
#include <time.h>

#define WIDTH 30
#define HEIGHT 20

int main(void) {
	struct Console* console = start();
	struct UnicodeScreen* ascii = initunicode(console);

	setforegroundcolorunicode(ascii, 0xFF,0x69, 0xB4);

	wchar_t plane[HEIGHT][WIDTH];

	int response = 0;
	while (response != 'y' && response != 'n') {
		clearunicode(ascii);
		setstringunicode(ascii, L"Only glider? y/n");
		refreshunicode(console, ascii);
		response = getcharacter(console);
	}

	srand(time(NULL));
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			plane[i][j] = L'.';
			int r = rand() % 2;
			if (r == 0 && response == 'n') {
				plane[i][j] = L'#';
			}
		}
	}

	if (response == 'y') {
		// glider
		plane[1][0] = L'#';
		plane[2][1] = L'#';
		plane[0][2] = L'#';
		plane[1][2] = L'#';
		plane[2][2] = L'#';
	}

	setinputblock(console, FALSE);
	hidecursorunicode(ascii);

	int generations = 0;
	int menusize = 2;
	int sleep = 100;

	while (1) {
		clearunicode(ascii);

		setstringformattedunicode(ascii, L"Generation: %d | speed: %d | space => enter insert mode", generations, sleep);
		setstringunicode(ascii, L"\nesc => exit");

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
				clearunicode(ascii);
				setstringformattedunicode(ascii, L"Generation: %d | space => exit view mode", generations);
				setstringunicode(ascii, L"\narrows => movement | r => new draw | n => next step | e => place cell");

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
									plane[i][j] = L'.';
								}
								else {
									plane[i][j] = L'#';
								}
							}
						}
						break;
					}
					case 'n': {
						char new[HEIGHT][WIDTH];
						for (int i = 0; i < HEIGHT; ++i) {
							for (int j = 0; j < WIDTH; ++j) {
								new[i][j] = L'.';
								int neighbours = 0;

								for (int k = -1; k <= 1; ++k) {
									for (int l = -1; l <= 1; ++l) {
										if (k == 0 && l == 0)
											continue;

										int row = (i + k + HEIGHT) % HEIGHT;
										int col = (j + l + WIDTH) % WIDTH;

										if (plane[row][col] == L'#') {
											neighbours++;
										}
									}
								}

								if (plane[i][j] == L'.' && neighbours == 3) {
									new[i][j] = L'#';
								}
								else if (plane[i][j] == L'#' && (neighbours == 2 || neighbours == 3)) {
									new[i][j] = L'#';
								}
							}
						}

						memcpy(plane, new, WIDTH * HEIGHT * sizeof(wchar_t));
						break;
					}
					case 'e': {
						plane[posrow][poscol] = L'#';
						break;
					}
					default: break;
				}

				set2darrayunicode(ascii, (wchar_t*)plane, 0 + menusize, 0, WIDTH, HEIGHT);

				setcharcursorunicode(ascii, L"@", posrow + menusize, poscol);

				refreshunicode(console, ascii);
			}
		}

		wchar_t new[HEIGHT][WIDTH];
		for (int i = 0; i < HEIGHT; ++i) {
			for (int j = 0; j < WIDTH; ++j) {
				new[i][j] = L'.';
				int neighbours = 0;

				for (int k = -1; k <= 1; ++k) {
					for (int l = -1; l <= 1; ++l) {
						if (k == 0 && l == 0)
							continue;

						int row = (i + k + HEIGHT) % HEIGHT;
						int col = (j + l + WIDTH) % WIDTH;

						if (plane[row][col] == L'#') {
							neighbours++;
						}
					}
				}

				if ((plane[i][j] == L'.' && neighbours == 3) || (plane[i][j] == L'#' && (neighbours == 2 || neighbours == 3))) {
					new[i][j] = L'#';
				}
			}
		}

		memcpy(plane, new, WIDTH * HEIGHT * sizeof(wchar_t));

		set2darrayunicode(ascii, (wchar_t*)plane, 0 + menusize, 0, WIDTH, HEIGHT);

		generations++;
		refreshunicode(console, ascii);
		_sleep(sleep);
	}

	end(console);

	system("pause");
	return 0;
}
