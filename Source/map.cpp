#include "map.h"


Map::Map() // field [y][x]
	{
	for (int i = 0; i < HEIGHT; i++)
	    for (int j = 0; j < WIDTH; j++)
		   field[i][j] = AIR;
	generate();
	difficulty = 40;
	round = 1;
	heightmap = NULL;
	}


Map::~Map()
	{
	delete[] heightmap;
	}

void Map::generate()// field [y][x]
	{
	for (int i = 0; i < HEIGHT; i++)
	     for (int j = 0; j < WIDTH; j++)
			 field[i][j] = AIR;
	for (int i = HEIGHT - HEIGHT_NON_DESTROYABLE; i < HEIGHT; i++)
		{
		for (int k = 0; k < WIDTH; k++)
			{
			field[i][k] = BASE;
			}
		}
	
	//hegy cs�cspontj�nak a random gener�l�sa
	srand(time(NULL));
	int hy = 60 + rand() % 15;    		//      74 < x < 150
	int hx = 200 + rand() % 200;		//      199 < y <401
	pos_of_peak = hx;
	for (int i = 399 - HEIGHT_NON_DESTROYABLE; i>hy; i--)       // field [y][x]
		{
		field[i][hx] = GROUND;
		}
	

	//most a hx-t�l jobbra �s balra elkezdj�k felt�lteni a fieldet, random ,hogy cs�kken-e a magass�g, de n�vekedni nemtud, �s a MINIMUM_GROUND_LEVEL al� nem mehet
	//balra
	int aktmag=hy;
	bool toolow = false;
	for (int i = hx - 1; i >= 0; i--)
		{
		if (!(rand() % 4) && !toolow)
			aktmag += rand() % 3 + 1;    //n�veljuk aktmagot, de ezzel cs�kken a pont magass�ga , mert az orig� a bal fels� sarokban van, �s y lefel� cs�kken
		if (!(rand() % 4) && toolow)
			aktmag -= rand() % 3 + 1;
		if (aktmag > HEIGHT - HEIGHT_NON_DESTROYABLE - MINIMUM_GROUND_LEVEL)
			toolow = true;
		for (int k = 399 - HEIGHT_NON_DESTROYABLE; k>aktmag; k--)       // field [y][x]
			{
			field[k][i] = GROUND;
			}
		}
	//jobbra
	aktmag = hy;
	toolow = false;
	for (int i = hx + 1; i < WIDTH; i++)
		{
		if (!(rand() % 4) && !toolow)
			aktmag += rand() % 3 + 1;     //n�veljuk aktmagot, de ezzel cs�kken a pont magass�ga , mert az orig� a bal fels� sarokban van, �s y lefel� cs�kken
		if (!(rand() % 4) && toolow)
			aktmag -= rand() % 3 + 1;
		if (aktmag > HEIGHT - HEIGHT_NON_DESTROYABLE - MINIMUM_GROUND_LEVEL)
			toolow = true;

		for (int k = 399 - HEIGHT_NON_DESTROYABLE; k>aktmag; k--)       // field [y][x]
			{
			field[k][i] = GROUND;
			}
		}




	}
bool Map::refresh()    //oszlponk�nt halandunk, egy oszlopban alulr�l indulunk, addig megy�nk, am�g nem tal�lunk AIR-t, ha tal�ltunk akkor egy pixellel lejjebb tesz�nk mindent
	{                    //amikot lejjebb rakosgatjuk a pixeleket, akkor ha tal�lunk k�zt�k nem AIR-t akkor m�g kell k�s�bb pakolnunk, ez�rt true-t adunk vissza
	int i;
	bool need_more_refresh = false;
	for(i = 0; i < WIDTH; i++)
		{
		int k;
		for (k = HEIGHT - 1; k >= 0; k--)
			{
			if (field[k][i] == AIR)    //field[y][x]
				break;
			}
		for (;  k > 0; k--)
			{
			if (field[k][i])
				need_more_refresh = true;
			field[k][i] = field[k - 1][i];
			}
		field[k][i] = AIR;
		}
	return need_more_refresh;
	}
void Map::print() const
	{
	ALLEGRO_COLOR mountain = GREEN;
	// v1.0 kb 0,22sec  pixelenk�nt rajzoljuk ki
	//for (int i = 0; i < HEIGHT; i++)
	//	{
	//	for (int k = 0; k < WIDTH; k++)
	//		if (field[i][k] == GROUND)  // field [y][x]
	//			al_draw_pixel(k + 1, i + 1, GREEN);
	//	}
	//v2.0 0,06 sec oszloponk�nt rajzoljuk ki, �gy hogy egy vonalat rajzolunk, majd ut�na f�ljebb m�g megvizsg�ljuk hogy ott van-e, mert rombol�sn�l lehet olyan
	for (int i = 0; i < WIDTH; i++)           // field [y][x]
		{
		int startline = -1, endline = -1;   //y �rt�kek
		int k;
		bool alja = true;
		for (k = HEIGHT - 1; k >= 0; k--)
			{
			if (field[k][i] == GROUND && alja)
				{
				startline = k;
				alja = false;
				}
			if (field[k][i] == AIR)
				{
				endline = k - 1;
				if (startline != -1 && endline != -1)
					al_draw_line(i + 1, startline + 1, i + 1, endline + 1, mountain, 1);
				break;
				}
			}

		//ha robban�s van, akkor GROUND ut�n AIR j�n, majd megint GROUND
		while (k > 0)
			{

			k--;
			alja = true;
			startline = -1, endline = -1;
			for (; k >= 0; k--)
				{
				if (field[k][i] == GROUND && alja)
					{
					startline = k;
					alja = false;
					}
				if (!alja && field[k][i] == AIR)
					{
					endline = k - 1;
					if (startline != -1 && endline != -1)
						al_draw_line(i + 1, startline + 1, i + 1, endline + 1, mountain, 1);
					break;
					}
				}
			}
	

		}



	al_draw_filled_rectangle(0, HEIGHT - HEIGHT_NON_DESTROYABLE, WIDTH, HEIGHT, BASE_COLOR);
	}


void Map::destroy(int x, int y, double radius)   //csak a k�z�ppont radius+10 sugar� n�gyzet�ben vizsg�l�dunk, nem kell az eg�sz p�ly�t v�gigp�szt�zni
	{
	int kezdoY = y - radius - 10;
	if (y - radius - 10 < 0)
		kezdoY = 0;
	for (int i = kezdoY; i < HEIGHT && i < y + radius + 10; i++)
		{
		int kezdoX = x - radius - 10;
		if (x - radius - 10 < 0)
			kezdoX = 0;
		for (int k = kezdoX; k < WIDTH && k < x + radius + 10; k++)
			{
			if (field[i][k] != BASE && sqrt((double)(k - x)*(k - x) + (i - y)*(i - y)) <= radius)
				field[i][k] = AIR;
			}
		}
	}
void Map::destroy(int x1, int y1, int x2, int y2, double thickness)
	{
	double m = (double)(y2 - y1) / (x2 - x1);  // y=mx+b
	double b = y1 - m*x1;

	if (x1 > x2)
		{
		int temp = x1;
		x1 = x2;
		x2 = temp;
		}

	for (int i = 0; i < HEIGHT; i++)
		{
		for (int k = 0; k < WIDTH; k++)
			{
			if (field[i][k] != BASE)
				{
				if (abs(m*k + b - i) <= thickness && k >= x1 && k <= x2)
					field[i][k] = AIR;
				}
			}

		}

	}
const int* Map::heightMap()
	{
	delete[] heightmap;
	heightmap = new int[WIDTH + 2];
	heightmap[WIDTH] = -1;//lez�rjuk minusz egyel, de m�g ez ut�n elhelyezz�k a hegycs�cs poz�ci�j�t 
	for (int i = 0; i < WIDTH; i++)
		{
		int k;
		for (k = 0; k < HEIGHT && field[k][i] == AIR; k++)
			{}
		heightmap[i] = HEIGHT - k;
		}
	heightmap[WIDTH + 1] = pos_of_peak;
	return heightmap;
	}
int Map::getdifficulty()const
	{
	return difficulty;
	}
void Map::setdifficulty(int diff)
	{
	difficulty = diff;
	}

int Map::get_round()const
	{
	return round;
	}
void Map::operator++(int)
	{
	round++;
	}