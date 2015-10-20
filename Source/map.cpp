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
	
	//hegy csúcspontjának a random generálása
	srand(time(NULL));
	int hy = 60 + rand() % 15;    		//      74 < x < 150
	int hx = 200 + rand() % 200;		//      199 < y <401
	pos_of_peak = hx;
	for (int i = 399 - HEIGHT_NON_DESTROYABLE; i>hy; i--)       // field [y][x]
		{
		field[i][hx] = GROUND;
		}
	

	//most a hx-tõl jobbra és balra elkezdjük feltölteni a fieldet, random ,hogy csökken-e a magasság, de növekedni nemtud, és a MINIMUM_GROUND_LEVEL alá nem mehet
	//balra
	int aktmag=hy;
	bool toolow = false;
	for (int i = hx - 1; i >= 0; i--)
		{
		if (!(rand() % 4) && !toolow)
			aktmag += rand() % 3 + 1;    //növeljuk aktmagot, de ezzel csökken a pont magassága , mert az origó a bal felsõ sarokban van, és y lefelé csökken
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
			aktmag += rand() % 3 + 1;     //növeljuk aktmagot, de ezzel csökken a pont magassága , mert az origó a bal felsõ sarokban van, és y lefelé csökken
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
bool Map::refresh()    //oszlponként halandunk, egy oszlopban alulról indulunk, addig megyünk, amíg nem találunk AIR-t, ha találtunk akkor egy pixellel lejjebb teszünk mindent
	{                    //amikot lejjebb rakosgatjuk a pixeleket, akkor ha találunk köztük nem AIR-t akkor még kell késõbb pakolnunk, ezért true-t adunk vissza
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
	// v1.0 kb 0,22sec  pixelenként rajzoljuk ki
	//for (int i = 0; i < HEIGHT; i++)
	//	{
	//	for (int k = 0; k < WIDTH; k++)
	//		if (field[i][k] == GROUND)  // field [y][x]
	//			al_draw_pixel(k + 1, i + 1, GREEN);
	//	}
	//v2.0 0,06 sec oszloponként rajzoljuk ki, úgy hogy egy vonalat rajzolunk, majd utána följebb még megvizsgáljuk hogy ott van-e, mert rombolásnál lehet olyan
	for (int i = 0; i < WIDTH; i++)           // field [y][x]
		{
		int startline = -1, endline = -1;   //y értékek
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

		//ha robbanás van, akkor GROUND után AIR jön, majd megint GROUND
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


void Map::destroy(int x, int y, double radius)   //csak a középpont radius+10 sugarú négyzetében vizsgálódunk, nem kell az egész pályát végigpásztázni
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
	heightmap[WIDTH] = -1;//lezárjuk minusz egyel, de még ez után elhelyezzük a hegycsúcs pozícióját 
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