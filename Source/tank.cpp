#include "tank.h"
#include "weapon.h"
Tank::~Tank()
	{
	al_destroy_bitmap(body);
	al_destroy_bitmap(canon);
	for (int i = 0; i < ROUND_NUM; i++)
		{
		delete weapons[i];
		}

	}
Tank::Tank(bool direction_of_tank_is_right)
	{
	this->direction_of_tank_is_right = direction_of_tank_is_right;
	if (direction_of_tank_is_right)
		angle_of_canon = 0.15*ALLEGRO_PI;
	else
		angle_of_canon = 0.85*ALLEGRO_PI;
	int score = 0;
	body = NULL;
	canon = NULL;
	//itt most random legeneráljuk, hogy melyik fegyverbõl milyen valószínûséggel legyen az inventoryba
	for (int i = 0; i < 10; i++)
		{
		int random = rand() % 100;
		if (random >= 60)     //40% az esély, hogy lézer kapunk
			weapons[i] = new laser();
		else if (random < 60)
			weapons[i] = new single_shot();

		}
	benzin = 100;
	shoot_force = 50;
	}
void Tank::rotateCanon(double angle) //pozitív ha balra forgatjuk
	{
	angle_of_canon += angle;
	if (angle_of_canon > 2 * ALLEGRO_PI)
		angle_of_canon -= ALLEGRO_PI * 2;
	if (angle_of_canon < 0)
		angle_of_canon += ALLEGRO_PI * 2;

	}

void Tank::setpos(double px, double py)
	{
	position.x=px;
	position.y=py;
	}

void Tank::move(const int *heightmap, bool right)   //ha a right = true akkor jobbra megyünk
	{
	minus_benzinlevel();
	if (benzin > 0)
		{
		double currentX = position.x;
		if (right && currentX < WIDTH - 10) //jobra megyünk
			{
			if (heightmap[(int)currentX]>heightmap[(int)currentX + 1]){}
			else if (heightmap[(int)currentX] - heightmap[(int)currentX + 1] < -5)   //ha nagyobb a szintkülömbség mint 10 pixel akkor azon már nemtud felmenni
				return;
			position.x = currentX + 0.50;
			position.y = HEIGHT - heightmap[(int)currentX + 1];
		return;



			}
		else if (currentX > 10)    //balra megyünk
			{
			if (heightmap[(int)currentX] - heightmap[(int)currentX - 1] < -5)   //ha nagyobb a szintkülömbség mint 10 pixel akkor azon már nemtud felmenni
				return;
			position.x = currentX - 0.50;
			position.y = HEIGHT - heightmap[(int)currentX - 1];
			return;
			}
		}
	}
bool Tank::gravity(const int *heightmap)
	{
	int currentX = (int)position.x;
	int currentY = (int)position.y;
	if (currentY < HEIGHT-heightmap[currentX])
		{
		position.y = currentY + 1;
		return true;
		}
	return false;
	}
void Tank::makeBody(ALLEGRO_COLOR color_body)
	{
	ALLEGRO_COLOR wheel = al_map_rgb(61, 61, 61);//szürke
	ALLEGRO_COLOR canon_base = al_map_rgb(0, 0, 0);
	body = al_create_bitmap(30, 30);
	al_set_target_bitmap(body);
	al_clear_to_color(al_map_rgb(128, 128, 128));
	al_draw_filled_rounded_rectangle(0, 22, 29, 30, 4, 4, color_body);
	al_draw_filled_rounded_rectangle(6, 15, 23, 22, 4, 4, canon_base);
	al_draw_filled_circle(5, 26, 3, wheel);
	al_draw_filled_circle(15, 26, 3, wheel);
	al_draw_filled_circle(25, 26, 3, wheel);
	al_convert_mask_to_alpha(body, al_map_rgb(128, 128, 128));   //alfára konvertáljuk a szürke hátteret
	canon = al_create_bitmap(21, 7);
	al_set_target_bitmap(canon); // ágyú generálás
	al_clear_to_color(al_map_rgb(128, 128, 128));
	al_draw_line(0, 4, 21, 4, canon_base, 3);
	al_convert_mask_to_alpha(canon, al_map_rgb(128, 128, 128));   //alfára konvertáljuk a szürke hátteret
	}
void Tank::print(const int *heightMap)    //elforgatja a tankot a pálya érintõjére, az ágyucsõ szögét a tank testéhet viszonyítja
	{                                 //a végén felszabadítja a kapott magasságtérképet
	//elõször kiszámoljuk az érintõt
	int currentX = (int)position.x;
	double height_left = 0.0, height_right = 0.0;
	//elmegyünk a tanktól jobbra és balra, külön külön átlagoljuk a magasságokat
	for (int i = 10; i != 0; i--)
		{
		height_left += heightMap[currentX - i];
		height_right += heightMap[currentX + i]; 
		}
	height_left /= 10;
	height_right /= 10;

	vektor toucher;
	toucher = vektor(10, height_right - height_left);
	vektor base(1, 0);
	angle_body = base > toucher;
	if (height_right > height_left)
		angle_body *= -1;
	double angle_canon = angle_body - angle_of_canon;

	if (angle_canon < -2 * ALLEGRO_PI)
		angle_canon += 2 * ALLEGRO_PI;
	if (angle_canon >0)
		angle_canon -= 2 * ALLEGRO_PI;

	al_draw_rotated_bitmap(body, 15, 30, position.x, position.y + 1, angle_body, NULL);
	//ágyúcsõ rajzolása
	if (direction_of_tank_is_right)
		{
		al_draw_rotated_bitmap(canon, 0, 4, position.x + 12.6 * cos(-1.25 + angle_body), position.y + 12.6 * sin(-1.25 + angle_body), angle_canon, NULL);
		position_of_end_canon = vektor(position.x + 12.6 * cos(-1.25 + angle_body) + 12 * cos(angle_canon), position.y + 12.6 * sin(-1.25 + angle_body) + 12 * sin(angle_canon));
		position_of_canon_base = vektor(position.x + 12.6 * cos(-1.25 + angle_body), position.y + 12.6 * sin(-1.25 + angle_body));

		}
	else
		{
		al_draw_rotated_bitmap(canon, 0, 4, position.x - 12.6 * cos(-1.25 - angle_body), position.y + 12.6 * sin(-1.25 - angle_body), angle_canon, NULL);
		position_of_end_canon = vektor(position.x + 12.6 * cos(-1.25 - angle_body) + 12 * cos(angle_canon), position.y + 12.6 * sin(-1.25 - angle_body) + 12 * sin(angle_canon));
		position_of_canon_base = vektor(position.x + 12.6 * cos(-1.25 - angle_body), position.y + 12.6 * sin(-1.25 - angle_body));
		}
	angle_of_canon_from_base = angle_canon;

	}



vektor Tank::getposition_of_end_of_canon()const
	{
	return position_of_end_canon;
	}






void Tank::set_angle_from_base(double angle)
	{
	angle_of_canon_from_base = angle;
	}
double Tank::get_angle_from_base()const
	{
	return angle_of_canon_from_base;
	}



vektor Tank::getpos()const
	{
	return position;
	}
void Tank::addscore(int plusscore)
	{
	score += plusscore;
	}

double Tank::get_benzinlevel()const
	{
	return benzin;
	}
void Tank::minus_benzinlevel()
	{
	benzin -= 0.25;
	if (benzin < 0)
		benzin = 0;
	}
double Tank::get_shootforce()const
	{
	return shoot_force;
	}
void Tank::plus_shootforce()
	{
	shoot_force++;
	if (shoot_force == 101)
		shoot_force = 100;
	}
void Tank::minus_shootforce()
	{
	shoot_force--;
	if (shoot_force < 0.99)
		shoot_force = 1;
	}
int Tank::getscore()const
	{
	return score;
	}
void Tank::set_shootforce(double force)
	{
	shoot_force = force;
	}

void Tank::set_angle_of_canon(double angle)
	{
	angle_of_canon = angle;
	}

void Tank::set_angle(double angle_parameter)
	{
	angle_of_canon = angle_parameter;
	}
double Tank::get_angle()const
	{
	return angle_of_canon;
	}

weapon* Tank::operator[](int i) const
	{
	if (i<0 || i>ROUND_NUM)
		return NULL;
	return weapons[i];
	}
vektor Tank::get_position_of_canon_base()const
	{
	return position_of_canon_base;
	}



double Tank::get_angle_body()const
	{
	return angle_body;
	}