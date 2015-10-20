//teend�: refresh f�ggv�ny bool-al r�jen vissza , �s addig true-val am�g m�g nem �llt meg a mozgat�s
//hogy addig lehessen megh�vni amig kell


#ifndef MAP_H_
#define MAP_H_

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <time.h>
#include <math.h>
#include <iostream>


#define FPS 30.0  
#define WIDTH 600  //sz�less�g X
#define HEIGHT 400   //Magass�g Y
#define HEIGHT_NON_DESTROYABLE 25
#define AIR 0
#define GROUND 1    //puszt�that�, pl ebb�l van a hegy
#define BASE 2     //elpuszt�thatatlan alap
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
		int *heightmap;   //egy -1 v�gjeles t�mb�t, ami oszloponk�nti magass�gokat tartalmazza(a v�gjel ut�n m�g el van t�rolva a hegycs�cs poz�ci�ja
	public:
		Map();
		~Map();
		void generate();   //hegy legener�l�s
		bool refresh();    //friss�t�s, gravit�ci�, rombol�s, ezt t�bbsz�r kell megh�vni, ha true-val t�r vissza, akkor m�g megkell h�vni m�gegyszer
		void print()const;     //oszloponk�nt rajzolja ki a hegyet
		void destroy(int x, int y, double radius);      //k�r alak�t rombol
		void destroy(int x1, int y1, int x2, int y2, double thickness);    //t�glalap alak�t rombol, pl l�zer kezd�pont, v�gpont, vastags�g
		const int* heightMap();
		int getdifficulty()const;
		void setdifficulty(int diff);
		int get_round()const;
		void operator++(int);   ///n�veli a k�r�k sz�m�t
	};




#endif
