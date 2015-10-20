//teendõ: refresh függvény bool-al réjen vissza , és addig true-val amíg még nem állt meg a mozgatás
//hogy addig lehessen meghívni amig kell


#ifndef MAP_H_
#define MAP_H_

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <time.h>
#include <math.h>
#include <iostream>


#define FPS 30.0  
#define WIDTH 600  //szélesség X
#define HEIGHT 400   //Magasság Y
#define HEIGHT_NON_DESTROYABLE 25
#define AIR 0
#define GROUND 1    //pusztítható, pl ebbõl van a hegy
#define BASE 2     //elpusztíthatatlan alap
#define BACKGND al_map_rgb(178,255,255)
#define GREEN al_map_rgb(154,205,40)
#define RED al_map_rgb(255,0,0)
#define MAGIC_COLOR al_map_rgb(rand()%256,rand()%256,rand()%256)
#define BLACK al_map_rgb(0,0,0)
#define BASE_COLOR al_map_rgb(32,178,170)
#define MINIMUM_GROUND_LEVEL 75
#define ROUND_NUM 10

class Map
	{
	private:
		char field[HEIGHT][WIDTH];    // field [y][x]
		int difficulty;
		int pos_of_peak;
		int round;
		int *heightmap;   //egy -1 végjeles tömböt, ami oszloponkénti magasságokat tartalmazza(a végjel után még el van tárolva a hegycsúcs pozíciója
	public:
		Map();
		~Map();
		void generate();   //hegy legenerálás
		bool refresh();    //frissítés, gravitáció, rombolás, ezt többször kell meghívni, ha true-val tér vissza, akkor még megkell hívni mégegyszer
		void print()const;     //oszloponként rajzolja ki a hegyet
		void destroy(int x, int y, double radius);      //kör alakút rombol
		void destroy(int x1, int y1, int x2, int y2, double thickness);    //téglalap alakút rombol, pl lézer kezdõpont, végpont, vastagság
		const int* heightMap();
		int getdifficulty()const;
		void setdifficulty(int diff);
		int get_round()const;
		void operator++(int);   ///növeli a körök számát
	};




#endif
