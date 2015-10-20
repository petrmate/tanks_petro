#include "weapon.h"
#include <math.h>
#include <time.h>
#define BULLET_IN_AIR 0 
#define HIT_GROUND 1
#define HIT_ENEMY 2
#define HIT_FRIENDLY 3 
#define LASER_TRAVEL 0 
#define LASER_HIT 1 
#define LASER_ENDED 2

const char* single_shot::get_name() const
	{
	return "Single Shot";
	}

weapon::~weapon()
	{
	
	}

single_shot::single_shot()
	{
	status = BULLET_IN_AIR;
	printstatus = 0;
	airtime = 0;
	}
single_shot::~single_shot()
	{


	}
void single_shot::set_speed(double angle, double power)
	{
	speed.x = 3.0 * power * cos(-angle);
	speed.y = -3.0 * power * sin(-angle);
	}
void single_shot::set_position(vektor const p)
	{
	position.x = p.x;
	position.y = p.y;
	}
void single_shot::move(int const *heightMap)
	{
	if (status == BULLET_IN_AIR) //a lövedék mozog
		{
		position.x += speed.x / FPS;
		position.y += speed.y / FPS;
		speed.y += GRAVITY/FPS;                  //gravitáció csökkenti a sebességet
		}

	}
void single_shot::print()
	{
	if (status == BULLET_IN_AIR)//itt még mozog a lövedék
		{
		airtime++;
		al_draw_filled_circle(position.x, position.y, 5, al_map_rgb(0, 0, 120));
		}
	else //itt már becsapódott, és a robbanás rajzolódik ki
		{// az egész robbanás 3 másodpercig tart 
		printstatus++;
		al_draw_filled_circle(position.x, position.y, 5 * printstatus / FPS, al_map_rgb(150 + rand() % 75, 100 + rand() % 50, 12));
		}
	}
bool single_shot::destroy(Map &map, const int *heightmap, Tank& friendly, Tank& enemy)
	{
	if (status == BULLET_IN_AIR)
		{
		if (enemy.getpos().distance(position) < 17)
			{
			status = HIT_ENEMY;
			}
		if (airtime>20 && friendly.getpos().distance(position) < 17)
			{
			status = HIT_FRIENDLY;
			}

		if ((HEIGHT - heightmap[(int)position.x]) - position.y < 2)
			status = HIT_GROUND;

		if (position.x<0 || position.x>WIDTH)
			status = HIT_GROUND;
		}
	if (printstatus == 3 * FPS) //robbantunk a mapból, és sebzünk.
		{
		map.destroy(position.x, position.y, 15);
		double distance_from_enemy = position.distance(enemy.getpos());
		double distance_from_friendly = position.distance(friendly.getpos());
		if (status = HIT_GROUND)
			{
			if (distance_from_enemy >= 0 && distance_from_enemy < 10)
				friendly.addscore(100);
			else
				{
				double damage = -5 * distance_from_enemy + 150;
				if (damage>0 && damage < 100)
					friendly.addscore((int)damage);
				}
			if (distance_from_friendly >= 0 && distance_from_friendly < 10)
				enemy.addscore(100);
			else
				{
				double damage = -5 * distance_from_friendly + 150;
				if (damage>0 && damage < 100)
					enemy.addscore((int)damage);
				}
			}
		if (status = HIT_ENEMY)
			{
			double damage = -5 * distance_from_enemy + 100;
			if (damage>0 && damage < 100)
				friendly.addscore((int)damage);
			}
		if (status = HIT_FRIENDLY)
			{
			double damage = -5 * distance_from_friendly + 100;
			if (damage>0 && damage < 100)
				enemy.addscore((int)damage);
			}
		return false;
		}
	return true;
	}
void single_shot::calculate_angle_and_power(const int *heightmap, int difficulty, vektor position_of_enemy, Tank &friendly)
	{
	srand(time(NULL));
	/*double height_left = 0.0;
	for (int i = 10; i != 0; i--)
		{
		height_left += heightmap[currentX - i];
		}
	height_left /= 10;

	vektor toucher;
	toucher = vektor(-5.5, friendly.getpos().y - height_left);

	double min_shoot_angle = vektor(1,0) > toucher;*/


   	vektor pos_of_canon = friendly.getposition_of_end_of_canon();
	int base_x = pos_of_canon.x;
	int currentX = base_x - 25;
	int base_y = HEIGHT - pos_of_canon.y;
	int pos_of_peak = heightmap[WIDTH + 1];
	double tangent;
	tangent = (heightmap[currentX] - base_y) / (double)(base_x - currentX);
	for (currentX--; currentX > 0; currentX--)//kiszámoljuk azt a maximális szöget, amivel még ellehet lõni
		{
		double aktual_tangent = (heightmap[currentX] - base_y) / (double)(base_x - currentX);
		if (aktual_tangent > tangent)
			tangent = aktual_tangent;
		}
	double max_shoot_angle = -ALLEGRO_PI + atan(tangent) + 0.4 + 0.05*(rand() % 8);
	double x = pos_of_canon.x - position_of_enemy.x;
	double y = pos_of_canon.y - position_of_enemy.y;
	double angle = max_shoot_angle;
	if (angle > -ALLEGRO_PI / 2)
		angle = -1.9;
	double force = sqrt((-GRAVITY*x*x) / (2 * cos(angle)*cos(angle)*(y - tan(angle)*x))) / 3.0;

	double probability = rand() % 100 + 1;
	
	force += (rand() % 20) / 20.0;

	if (probability < (70 * cos(difficulty / (ALLEGRO_PI * 20))) + 10)//nehézségi szintbõl csinál  hibázási valószínüséget
		{
		if (rand() % 2)
			{//túl sok lesz az erõ
			force += (rand() % 40) / 10.0 + 1;
			}
		else	//túl kevés lesz az erõs
			{
			force -= (rand() % 40) / 10.0 + 1;
			}
		}

	friendly.set_angle(-angle + friendly.get_angle_from_base() + friendly.get_angle());
	set_speed(angle, force);

	set_position(pos_of_canon);
	}

laser::laser()
	{
	printstatus = 0;
	status = LASER_TRAVEL;
	}
laser::~laser()
	{
	
	}
void laser::set_speed(double angle, double power)
	{
	speed.x = 100.0 *  cos(-angle);
	speed.y = -100.0 *  sin(-angle); 
	}
void laser::set_position(vektor const p)
	{
	start_position = p;
	end_position = p;
	}
void laser::move(int const *heightMap)
	{
	if (end_position.x<0 || end_position.x>WIDTH || end_position.y < 0 || end_position.y>HEIGHT - HEIGHT_NON_DESTROYABLE)
		{
		status = LASER_HIT;
		}
	if (status == LASER_TRAVEL)
		{
		end_position.x += speed.x / FPS;
		end_position.y += speed.y / FPS;
		}
	}
	
void laser::print()
	{
	if (status == LASER_TRAVEL)
		{
		al_draw_line(start_position.x, start_position.y, end_position.x, end_position.y, al_map_rgb(0, 0, 240), 5);
		al_draw_filled_circle(end_position.x, end_position.y, 2.5, al_map_rgb(0, 0, 240));
		}
	if (status == LASER_HIT)
		{
		al_draw_line(start_position.x, start_position.y, end_position.x, end_position.y, al_map_rgb(0, 0, 240), 6 + 3 * sin(printstatus / 3));
		al_draw_filled_circle(end_position.x, end_position.y, 3 + 1.5 * sin(printstatus / 3), al_map_rgb(0, 0, 240));
		al_draw_line(start_position.x, start_position.y, end_position.x, end_position.y, MAGIC_COLOR, 3 + 3 * sin(printstatus / 3));
		}
	
	}
bool laser::destroy(Map &map, const  int *heightmap, Tank& friendly, Tank& enemy)
	{
	if (end_position.y < enemy.getpos().y + tan(enemy.get_angle_body())*(end_position.x - enemy.getpos().x)
		                  && enemy.getpos().distance(end_position) < 15.0)
		{
		status = LASER_HIT;
		}
	if (status == LASER_TRAVEL)
		{
		return true;
		}
	if (printstatus == (int)3 * FPS)
		{
		status = LASER_ENDED;
		int radius = 5 + rand() % 3;
		map.destroy(start_position.x, start_position.y, end_position.x, end_position.y, radius);
		map.destroy(end_position.x, end_position.y, radius / 2);
		   //ezzel csak az ellenfelet lehet sebezni
		double distance = end_position.distance(enemy.getpos());
		if (distance < 15)
			friendly.addscore((int )(-5.3333*distance + 150));
		return false;
		}
	if (status == LASER_HIT)
		{
		printstatus++;
		return true;
		}

	return true;
	}
void laser::calculate_angle_and_power(const int *heightmap, int difficulty, vektor position_of_enemy, Tank &friendly)
	{
	vektor position_of_friendly = friendly.getpos();
	double angle = vektor(1.0, 0.0) > (position_of_enemy + friendly.get_position_of_canon_base() * (-1));//két vektor hajlásszöge
	if (position_of_enemy.y < position_of_friendly.y)
		friendly.set_angle(angle + friendly.get_angle_from_base() + friendly.get_angle());
	else
		friendly.set_angle(-angle + friendly.get_angle_from_base() + friendly.get_angle());
	friendly.print(heightmap);
	angle = vektor(1.0, 0.0) > (position_of_enemy + friendly.getposition_of_end_of_canon() * (-1));//két vektor hajlásszöge


	double probability = rand() % 100 + 1;

	angle += 0.0772664626*((rand() % 20) / 20);//random-ba ad max 5 fokot

	if (probability < (70 * cos(difficulty / (ALLEGRO_PI * 20))) + 10)//nehézségi szintbõl csinál  hibázási valószínüséget(negyes szinusz peridódus)
		{
		if (rand() % 2)
			{
			angle += 0.0772664626*(0.6+(rand() % 20) / 20);
			}
		else	
			{
			angle += 0.0772664626*(0.6 + (rand() % 20) / 20);
			}
		}


	if (position_of_enemy.y < position_of_friendly.y)
		set_speed(-angle, 0);
	else
		set_speed(angle, 0);
	set_position(friendly.getposition_of_end_of_canon());

	}


const char* laser::get_name() const
	{
	return "Laser";
	}
