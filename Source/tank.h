//kell másoló konstruktor, mert a bitmapok dinamikusak



#ifndef TANK_H_
#define TANK_H_
#include "vektor.h"
#include "map.h"
class weapon;
#include <allegro5/allegro.h>
#include <math.h>
#include <allegro5\allegro_primitives.h>
class Tank
	{
	private:
		int score;
		vektor position;
		double angle_of_canon;   //az ágyú állás a tankhopz viszinítva
		double angle_of_canon_from_base;  //az ágyú állás a vízszinhez viszinytva
		ALLEGRO_BITMAP *body;       
		ALLEGRO_BITMAP *canon;       
		weapon* weapons[ROUND_NUM]; // ebbe fogjuk tárolni a fegyvereket heterogén kollekcióval;
		bool direction_of_tank_is_right;
		vektor position_of_end_canon;
		double benzin;
		double shoot_force;
		vektor position_of_canon_base;
		double angle_body;
	public:
		Tank(bool direction_of_tank_is_right = false);
		~Tank();
		void makeBody(ALLEGRO_COLOR color_body); //megadott színû tank készítése, utána meg kell hívni a al_set_target_bitmap(al_get_backbuffer(display)) függvényt
		void move(const int *heightmap, const bool right);     //1 pixellel odébbviszi a tankot
		void print(const int *heightMap);                //kirajzolja az adott tankot, megfelelõ állású csõvel
		void rotateCanon(double angle); //pozitív ha balra forgatjuk
		void setpos(double px, double py);
		vektor getpos()const;
		//ha false-al tér vissza akkor már nincs szükség gravityra
		bool gravity(const int *heightMap);
		void set_angle_from_base(double angle);
		double get_angle_from_base()const;
		void addscore(int plusscore);
		vektor getposition_of_end_of_canon()const;
		double get_benzinlevel()const;
		void minus_benzinlevel();
		double get_shootforce()const;
		void plus_shootforce();
		void minus_shootforce();
		int getscore()const;
		void set_shootforce(double force);
		void set_angle_of_canon(double angle);
		void set_angle(double angle_parameter);
		double get_angle()const;
		double get_angle_body()const;
		weapon* operator[](int i)const;
		vektor get_position_of_canon_base()const;

	};



#endif