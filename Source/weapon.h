
#ifndef WEAPON_H_
#include "vektor.h"
#include "map.h"
#include "tank.h"
#include <allegro5/allegro_primitives.h>

#define WEAPON_H_
#define GRAVITY 100.0
class weapon    //intef�sz �r�kl�s, heterog�n kollekci�s fegyvert�mb tartozik minden tankhoz
	{
	public:
		virtual ~weapon() = 0;
		virtual void move(int const *heightMap) = 0;
		virtual void print() = 0;
		virtual bool destroy(Map& map, const  int *heightmap, Tank& friendly, Tank& enemy) = 0;
		virtual void set_speed(double angle, double power) = 0;
		virtual void set_position(vektor const p) = 0;
		virtual void calculate_angle_and_power(const int *heightmap, int difficulty, vektor position_of_enemy, Tank &friendly) = 0;
		virtual const char* get_name()const = 0;
	};

class single_shot :public weapon
	{
	private:
		vektor speed;
		vektor position;
		int status;
		int printstatus;
		int airtime;//azzel sz�moljuk az elej�n hogy mennyi ideig van a leveg�ben, az�rt hogy felt�etlkp�nt haszn�ljuk a saj�t magunk sebz�s�n�l
	public:
		const char* get_name()const;
		single_shot();
		~single_shot();
		void set_speed(double angle, double power);
		void set_position(vektor const p);
		void move(int const *heightMap);
		void print();
		bool destroy(Map &map, const int *heightmap, Tank& friendly, Tank& enemy);
		void calculate_angle_and_power(const int *heightmap, int difficulty, vektor position_of_enemy, Tank &friendly);


	};
class laser :public weapon
	{
	private:
		vektor speed;//nagys�g nemfontos, az ir�ny ami l�nyeges
		vektor start_position;
		vektor end_position;
		int status;
		int printstatus;
	public:
		laser();
		~laser();
		void set_speed(double angle, double power);
		void set_position(vektor const p);
		void move(int const *heightMap);
		void print();
		bool destroy(Map &map, const int *heightmap, Tank& friendly, Tank& enemy);
		void calculate_angle_and_power(const int *heightmap, int difficulty, vektor position_of_enemy, Tank &friendly);
		const char* get_name()const;


	};



#endif

