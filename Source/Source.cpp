
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <string.h>
#include <fstream>     //ha valami hiba lép fel inícializáláskor, akkor egy fájlba írjuk a logot
#include "map.h"
#include "tank.h"                
#include "weapon.h"
struct status
	{
	bool up;
	bool down;
	bool left;
	bool right;
	bool maprefresh;
	bool gravity;
	bool fire;
	bool under_fire;//ha egy true akkor lövedék még a levegõben van, ekkor minden irányítást letiltunk
	bool powerplus;
	bool powerminus;
	bool enemyturn;
	bool waiting_for_enemy_move;
	bool exit;
	bool ended;
	bool new_game;
	};

void print_texts(ALLEGRO_FONT *font, Tank const &player, Tank const &enemy, Map const &map)
	{
	int round = map.get_round();
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
	al_draw_textf(font, black, 10, 10, NULL, "Player:%3d", player.getscore());
	al_draw_textf(font, black, WIDTH - 100, 10, NULL, "Enemy:%3d", enemy.getscore());
	al_draw_textf(font, black, 0, HEIGHT - 25, NULL, "Fuel:%3.0lf %% ", player.get_benzinlevel());
	al_draw_textf(font, black, 100, HEIGHT - 25, NULL, "Angle:%3.0lf *", -57.295779*player.get_angle_from_base());
	al_draw_textf(font, black, 210, HEIGHT - 25, NULL, "Power:%3.0lf", player.get_shootforce());
	if (round <= ROUND_NUM)
		al_draw_textf(font, black, WIDTH / 2, 10, ALLEGRO_ALIGN_CENTRE, "Round %2d.", map.get_round());
	else
		al_draw_textf(font, black, WIDTH / 2, 10, ALLEGRO_ALIGN_CENTRE, "Round %2d.", ROUND_NUM);

	if (round <= ROUND_NUM)
		al_draw_textf(font, black, 350, HEIGHT - 25, NULL, "Weapon: %s", player[round - 1]->get_name());
	else
		al_draw_textf(font, black, 350, HEIGHT - 25, NULL, "Weapon: %s", "No weapon left");

	}
bool menu(ALLEGRO_EVENT_QUEUE *event_queue, int & difficulty, ALLEGRO_COLOR &player_color, ALLEGRO_COLOR &enemy_color, ALLEGRO_DISPLAY *display,
	ALLEGRO_FONT *font_big, ALLEGRO_FONT *font_small, ALLEGRO_FONT *font_tiny)// többlépcsõs menü, itt lehet majd nehézséget, színet, meg a többi apróságot állítani
	{//kezdõ menü,színháromszögek, és nehézségi szint 
	player_color = MAGIC_COLOR;
	enemy_color = MAGIC_COLOR;
	ALLEGRO_EVENT ev;
	bool stay = true;
	vektor red(185, 250), green(300, 50), blue(415, 250);//a háromszög sarkai a színek "origói"
	vektor aktpos;
	bool new_game = false, help = false;
	//kezdõképernyõ(játéknév, verzió, és loading
	char *loading[4] = { "    ", ".   ", "..  ", "... " };
	char *aktload = loading[0];
	int aktloadingcounter = 0;
	for (int i = 0; i < 5 * FPS; i++) //Loading bar
		{
		al_clear_to_color(BACKGND);
		al_draw_textf(font_big, BLACK, WIDTH / 2, 14, ALLEGRO_ALIGN_CENTRE, "%s", "Tanks v1.0");
		al_draw_textf(font_small, BLACK, WIDTH / 2 + 40, 40, ALLEGRO_ALIGN_CENTER, "%s", "by Petro /Y9LQB9/");
		al_draw_filled_rectangle(WIDTH / 4, 3 * HEIGHT / 4, WIDTH / 4 + 2 * i, 3 * HEIGHT / 4 + 50, al_map_rgb(50, 200, 75));
		al_draw_rectangle(WIDTH / 4, 3 * HEIGHT / 4, 3 * WIDTH / 4, 3 * HEIGHT / 4 + 50, BLACK, 3);
		al_draw_textf(font_small, BLACK, WIDTH / 2, 3 * HEIGHT / 4 + 58, ALLEGRO_ALIGN_CENTER, "%3.0lf%s", 1 + 100 * i / (5 * FPS), "%");
		if (!(i % 10))
			{
			aktloadingcounter++;
			aktloadingcounter %= 4;
			aktload = loading[aktloadingcounter];
			}


		al_draw_textf(font_big, BLACK, WIDTH / 2, 3 * HEIGHT / 4 + 10, ALLEGRO_ALIGN_CENTRE, "%s%s", "Loading", aktload);
		al_flip_display();
		al_rest(1 / FPS);
		}

	//menu:új játék, leírás, kilépés
	al_clear_to_color(BACKGND);
	al_draw_textf(font_big, BLACK, WIDTH / 2, 14, ALLEGRO_ALIGN_CENTRE, "%s", "Tanks v1.0");
	al_draw_textf(font_big, BLACK, WIDTH / 2, 100, ALLEGRO_ALIGN_CENTRE, "%s", "New Game");
	al_draw_textf(font_big, BLACK, WIDTH / 2, 140, ALLEGRO_ALIGN_CENTRE, "%s", "Help");
	al_draw_textf(font_big, BLACK, WIDTH / 2, 180, ALLEGRO_ALIGN_CENTRE, "%s", "Quit");
	al_draw_textf(font_small, BLACK, WIDTH / 2 + 40, 40, ALLEGRO_ALIGN_CENTER, "%s", "by Petro /Y9LQB9/");
	al_draw_rectangle(WIDTH / 2 - 28, 180, WIDTH / 2 + 35, 210, BLACK, 3);
	al_draw_rectangle(WIDTH / 2 - 70, 100, WIDTH / 2 + 70, 130, BLACK, 3);
	al_draw_rectangle(WIDTH / 2 - 40, 140, WIDTH / 2 + 40, 170, BLACK, 3);
	al_flip_display();
	while (1)
		{
		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
			int x = ev.mouse.x;
			int y = ev.mouse.y;
			if (x > WIDTH / 2 - 70 && x < WIDTH / 2 + 70 && y>100 && y < 130) //new game
				{
				new_game = true;
				break;
				}
			if (x > WIDTH / 2 - 28 && x < WIDTH / 2 + 35 && y>180 && y < 210)//exit
				{
				return false;
				}
			if (x > WIDTH / 2 - 40 && x < WIDTH / 2 + 40 && y>140 && y < 170)//help
				{
				help = true;
				break;
				}
			}
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			return false;
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
			return false;
			}
		}
	if (help)
		{
		al_clear_to_color(BACKGND);
		al_draw_textf(font_big, BLACK, WIDTH / 2, 14, ALLEGRO_ALIGN_CENTRE, "%s", "Tanks v1.0");
		al_draw_textf(font_small, BLACK, WIDTH / 2 + 40, 40, ALLEGRO_ALIGN_CENTER, "%s", "by Petro /Y9LQB9/");
		al_draw_textf(font_big, BLACK, WIDTH / 2, 80, ALLEGRO_ALIGN_CENTER, "%s", "/Controls/");
		al_draw_textf(font_big, BLACK, WIDTH / 2, 120, ALLEGRO_ALIGN_CENTER, "%s", "Move: A,D");
		al_draw_textf(font_big, BLACK, WIDTH / 2, 160, ALLEGRO_ALIGN_CENTER, "%s", "Rotate canon: W,S");
		al_draw_textf(font_big, BLACK, WIDTH / 2, 200, ALLEGRO_ALIGN_CENTER, "%s", "Fire Power: Q,E");
		al_draw_textf(font_big, BLACK, WIDTH / 2, 240, ALLEGRO_ALIGN_CENTER, "%s", "Fire: Space");
		al_draw_textf(font_big, BLACK, WIDTH / 2, 280, ALLEGRO_ALIGN_CENTER, "%s", "Quit: Esc");
		al_draw_textf(font_big, BLACK, WIDTH / 4, 320, ALLEGRO_ALIGN_CENTRE, "%s", "New Game");
		al_draw_textf(font_big, BLACK, 3 * WIDTH / 4, 320, ALLEGRO_ALIGN_CENTRE, "%s", "Quit");
		al_draw_rectangle(WIDTH / 4 - 70, 320, WIDTH / 4 + 70, 350, BLACK, 3);
		al_draw_rectangle(3 * WIDTH / 4 - 30, 320, 3 * WIDTH / 4 + 35, 350, BLACK, 3);



		al_flip_display();
		while (2)
			{
			al_wait_for_event(event_queue, &ev);
			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
				{
				int x = ev.mouse.x;
				int y = ev.mouse.y;
				if (x > WIDTH / 4 - 70 && x < WIDTH / 4 + 70 && y>320 && y < 350) //new game
					{
					new_game = true;
					break;
					}
				if (x >3 * WIDTH / 4 - 30 && x < 3 * WIDTH / 4 + 35 && y>320 && y < 350)//exit
					{
					return false;
					}
				}
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				return false;
			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				{
				return false;
				}
			}
		}

	//saját szín választó

	
	for (int m = 0; m < 2; m++)
		{

		al_clear_to_color(BACKGND);
		for (int i = 0; i < HEIGHT; i++)//i-vel y irányba, k-val x irányban járjuk be a képernyõt
			{
			for (int k = 0; k < WIDTH; k++)
				{
				if (-1.73913*k + 571.74 < i && 1.73913*k - 471.73 < i && i < 250)//háromszög egyenlõtlenségrendszer.
					{
					aktpos.x = k;
					aktpos.y = i;
					al_draw_pixel(k + 1, i + 1, al_map_rgb(255 - 1.1*aktpos.distance(red), 255 - 1.1*aktpos.distance(green), 255 - 1.1*aktpos.distance(blue)));
					}
				}
			}

		al_draw_textf(font_big, BLACK, WIDTH / 2, 14, ALLEGRO_ALIGN_CENTRE, "%s", "Pick your color, and hit next!");
		al_draw_textf(font_big, BLACK, WIDTH * 5 / 6, HEIGHT - 50, ALLEGRO_ALIGN_CENTRE, "%s", "NEXT->");
		al_draw_textf(font_small, BLACK, WIDTH * 1 / 6, HEIGHT - 100, ALLEGRO_ALIGN_CENTRE, "%s", "Current color: ");
		al_draw_filled_rectangle(50, HEIGHT - 80, 150, HEIGHT - 30, player_color);
		al_draw_rectangle(450, 345, 550, 385, BLACK, 4);
		al_flip_display();
		}
	while (1)
		{

		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
			if (ev.mouse.button == 1) //ball klikk
				{
				int x = ev.mouse.x;
				int y = ev.mouse.y;

				if (-1.73913*x + 571.74 < y && 1.73913*x - 471.73 < y && y < 250)//háromszög egyenlõtlenségrendszer.
					{//a háromszögön belülre kattintottunk
					aktpos.x = x;
					aktpos.y = y;
					player_color = al_map_rgb(255 - 1.1*aktpos.distance(red), 255 - 1.1*aktpos.distance(green), 255 - 1.1*aktpos.distance(blue));
					}
				else
				if (x>450 && x<550 && y>345 && y < 385)
					break;


				al_draw_filled_rectangle(50, HEIGHT - 80, 150, HEIGHT - 30, player_color);
				al_flip_display();
				}
			}

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
			if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				return false;
			}
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
			return false;
			}

		}



	//ellenfél színválasztó

		for (int m = 0; m < 2; m++)
			{

			al_clear_to_color(BACKGND);
			for (int i = 0; i < HEIGHT; i++)//i-vel y irányba, k-val x irányban járjuk be a képernyõt
				{
				for (int k = 0; k < WIDTH; k++)
					{
					if (-1.73913*k + 571.74 < i && 1.73913*k - 471.73 < i && i < 250)//háromszög egyenlõtlenségrendszer.
						{
						aktpos.x = k;
						aktpos.y = i;
						al_draw_pixel(k + 1, i + 1, al_map_rgb(255 - 1.1*aktpos.distance(red), 255 - 1.1*aktpos.distance(green), 255 - 1.1*aktpos.distance(blue)));
						}
					}
				}

			al_draw_textf(font_big, BLACK, WIDTH / 2, 14, ALLEGRO_ALIGN_CENTRE, "%s", "Pick the enemy's color, and hit next!");
			al_draw_textf(font_big, BLACK, WIDTH * 5 / 6, HEIGHT - 50, ALLEGRO_ALIGN_CENTRE, "%s", "NEXT->");
			al_draw_textf(font_small, BLACK, WIDTH * 1 / 6, HEIGHT - 100, ALLEGRO_ALIGN_CENTRE, "%s", "Current color: ");
			al_draw_filled_rectangle(50, HEIGHT - 80, 150, HEIGHT - 30, enemy_color);
			al_draw_rectangle(450, 345, 550, 385, BLACK, 4);
			al_flip_display();
			}
		while (1)
			{
		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
			if (ev.mouse.button == 1)
				{
				int x = ev.mouse.x;
				int y = ev.mouse.y;

				if (-1.73913*x + 571.74 < y && 1.73913*x - 471.73 < y && y < 250)//háromszög egyenlõtlenségrendszer.
					{
					aktpos.x = x;
					aktpos.y = y;
					enemy_color = al_map_rgb(255 - 1.1*aktpos.distance(red), 255 - 1.1*aktpos.distance(green), 255 - 1.1*aktpos.distance(blue));
					}
				else if (x>450 && x<550 && y>345 && y < 385) //next gomb helye
					break;


				al_draw_filled_rectangle(50, HEIGHT - 80, 150, HEIGHT - 30, enemy_color);
				al_flip_display();
				}
			}

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
			if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				return false;
			}
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
			return false;
			}
		}
	//nehézségi szint állító
	char *difficulty_name[10] =
		{
		"Very Low",
		"Low",
		"Weak",
		"Easy",
		"Medium",
		"Hard",
		"Ultra Violence",
		"Nightmare",
		"Impossible",
		"Chuck Norris",
		};
	al_clear_to_color(BACKGND);
	al_draw_textf(font_big, BLACK, WIDTH / 2, 14, ALLEGRO_ALIGN_CENTRE, "%s", "Choose a suitable difficulty level and hit next!");
	al_draw_textf(font_big, BLACK, WIDTH * 5 / 6, HEIGHT - 50, ALLEGRO_ALIGN_CENTRE, "%s", "NEXT->");
	al_draw_filled_triangle(WIDTH / 2 - 25, 120, WIDTH / 2 - 25, 180, WIDTH / 2 - 100, 150, BLACK);
	al_draw_filled_triangle(WIDTH / 2 + 25, 120, WIDTH / 2 + 25, 180, WIDTH / 2 + 100, 150, BLACK);
	al_flip_display();
	int difficulty_level = 5;
	al_draw_textf(font_big, BLACK, WIDTH / 2 - 5, 150 - 15, ALLEGRO_ALIGN_CENTER, "%2d", difficulty_level);
	while (1)
		{

		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
			int x = ev.mouse.x;
			int y = ev.mouse.y;

			if (ev.mouse.button == 1)
				{
				if (x > WIDTH / 2 + 25 && x < WIDTH / 2 + 100 && y<180 && y>120 && difficulty_level<10)//növelõ gomb
					{
					difficulty_level++;
					}
				if (x < WIDTH / 2 - 25 && x>WIDTH / 2 - 100 && y<180 && y>120 && difficulty_level>1)//csökkentõgomb
					{
					difficulty_level--;
					}
				if (x > 450 && x<550 && y>345 && y < 385) //Next gomb
					break;
				}
			}
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			return false;
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
			return false;
			}
		al_clear_to_color(BACKGND);
		al_draw_textf(font_big, BLACK, WIDTH / 2, 14, ALLEGRO_ALIGN_CENTRE, "%s", "Choose a suitable difficulty level and hit next!");
		al_draw_textf(font_big, BLACK, WIDTH * 5 / 6, HEIGHT - 50, ALLEGRO_ALIGN_CENTRE, "%s", "NEXT->");
		al_draw_textf(font_big, BLACK, WIDTH / 2, 210, ALLEGRO_ALIGN_CENTER, "%s", difficulty_name[difficulty_level - 1]);
		al_draw_filled_triangle(WIDTH / 2 - 25, 120, WIDTH / 2 - 25, 180, WIDTH / 2 - 100, 150, BLACK);
		al_draw_filled_triangle(WIDTH / 2 + 25, 120, WIDTH / 2 + 25, 180, WIDTH / 2 + 100, 150, BLACK);
		al_draw_rectangle(450, 345, 550, 385, BLACK, 4);
		al_draw_textf(font_big, BLACK, WIDTH / 2 - 5, 150 - 15, ALLEGRO_ALIGN_CENTER, "%2d", difficulty_level);
		al_flip_display();
		}





	difficulty = difficulty_level * 10;
	return true;
	}

int main(void)
	{
	srand(time(NULL));
	//Allegrós dolgok és státusjelzõk inícializálása
	status status = { false, false, false, false, true, true, false, false, false, false, false, false, false, false };
	try{
		if (!al_init())
			throw("Failed to initialize Allegro's core module");
		if (!al_init_primitives_addon())
			throw("Failed to initialize Allegro's drawing module");
		al_init_font_addon();
		if (!al_init_ttf_addon())
			throw("Failed to initialize Allegro's text drawing module");
		if (!al_install_keyboard())
			throw("Failed to initialize Allegro's keyboard module");
		if (!al_install_mouse())
			throw("Failed to initialize Allegro's mouse module");
		}
	catch (const char *error)
		{//hibaüzenet mellé a dátumot is kírjuk
		std::ofstream error_file;
		time_t  timev;
		time(&timev);
		struct tm *time_formatted = localtime(&timev);  //time.h beli C-s struktúra, ami átkonvertájla a a time_t-t rendes idõ formátumra(év,hó, nap, ...)
		error_file.open("errorlogger.txt", std::ios::app);
		error_file << '[' << time_formatted->tm_year + 1900 << '.' << time_formatted->tm_mon + 1 << '.' << time_formatted->tm_mday
			<< ". " << time_formatted->tm_hour << ':' << time_formatted->tm_min << ':' << time_formatted->tm_sec << ']' << std::endl;
		error_file << "Error occured: " << error << std::endl;
		error_file.close();
		status.exit = true;
		exit(-1);
		}
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	event_queue = al_create_event_queue();
	ALLEGRO_DISPLAY *display = NULL;
	display = al_create_display(WIDTH, HEIGHT);
	ALLEGRO_TIMER *timer = NULL;
	timer = al_create_timer(1.0 / FPS);
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	ALLEGRO_FONT *font_10 = al_load_ttf_font("font.ttf", 10, NULL);
	ALLEGRO_FONT *font_16 = al_load_ttf_font("font.ttf", 16, NULL);
	ALLEGRO_FONT *font_24 = al_load_ttf_font("font.ttf", 24, NULL);
	ALLEGRO_EVENT ev;
	//inicializálás vége
	int player_score = 0, enemy_score = 0;
	//beállítások (színek, nehézség, stb.)
	while (!status.exit)//elindul ez a ciklus, ha újra ideér a program, akkor  az azt jelenti, hogy elértük a 10.kört, és restartot nyomtak
		{


		ALLEGRO_COLOR player_color = MAGIC_COLOR, enemy_color = MAGIC_COLOR;
		int difficulty = 30;
		status.exit = !menu(event_queue, difficulty, player_color, enemy_color, display, font_24, font_16, font_10);
		if (status.exit)
			break;
		//

		Map map;
		map.setdifficulty(difficulty);
		Tank player(true);//a player jobbra néz
		Tank enemy(false);//az ellenfél bara

		const int *heightmap = map.heightMap();

		player.makeBody(player_color);
		player.setpos(50, HEIGHT - heightmap[50] - 50);
		enemy.makeBody(enemy_color);
		enemy.setpos(WIDTH - 50, HEIGHT - heightmap[WIDTH - 50] - 50);



		al_set_target_bitmap(al_get_backbuffer(display));
		al_clear_to_color(BACKGND);
		map.print();
		al_flip_display();
		weapon *fegyver = NULL;

		al_start_timer(timer);

		while (1)
			{
			int refreshspeedspoler = 0;
			al_wait_for_event(event_queue, &ev);

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)     //gomblenyomás
				{
				if (ev.keyboard.keycode == ALLEGRO_KEY_D)
					{
					status.right = true;
					}

				else if (ev.keyboard.keycode == ALLEGRO_KEY_A)
					{
					status.left = true;
					}

				else if (ev.keyboard.keycode == ALLEGRO_KEY_W)
					{

					status.up = true;
					}

				else if (ev.keyboard.keycode == ALLEGRO_KEY_S)
					{

					status.down = true;
					}

				else if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
					{
					status.fire = true;
					}
				else if (ev.keyboard.keycode == ALLEGRO_KEY_Q)
					{
					status.powerminus = true;
					}
				else if (ev.keyboard.keycode == ALLEGRO_KEY_E)
					{
					status.powerplus = true;
					}
				else if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					status.exit = true;

				}

			else if (ev.type == ALLEGRO_EVENT_KEY_UP)                   //gombfelengedés
				{
				if (ev.keyboard.keycode == ALLEGRO_KEY_D)
					{
					status.right = false;
					}

				else if (ev.keyboard.keycode == ALLEGRO_KEY_A)
					{

					status.left = false;
					}

				else if (ev.keyboard.keycode == ALLEGRO_KEY_W)
					{

					status.up = false;
					}

				else if (ev.keyboard.keycode == ALLEGRO_KEY_S)
					{

					status.down = false;
					}
				else if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
					{

					status.fire = false;
					}

				else if (ev.keyboard.keycode == ALLEGRO_KEY_Q)
					{
					status.powerminus = false;
					}
				else if (ev.keyboard.keycode == ALLEGRO_KEY_E)
					{
					status.powerplus = false;
					}

				}

			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				{
				status.exit = true;
				}

			if (!(status.ended || status.maprefresh || status.gravity || status.under_fire || status.waiting_for_enemy_move || status.enemyturn)) //player köre
				{
				if (status.left && !status.right)
					{
					player.move(heightmap, false);
					}
				else if (!status.left && status.right)
					{
					player.move(heightmap, true);
					}
				else if (!status.up && status.down)
					{
					player.rotateCanon(-0.013);
					}
				else if (status.up && !status.down)
					{
					player.rotateCanon(+0.013);
					}
				else if (status.fire)
					{
					fegyver = player[map.get_round() - 1];
					fegyver->set_position(player.getposition_of_end_of_canon());
					fegyver->set_speed(player.get_angle_from_base(), player.get_shootforce());
					status.under_fire = true;
					}
				else if (status.powerminus && !status.powerplus)
					{
					player.minus_shootforce();
					}
				else if (!status.powerminus && status.powerplus)
					{
					player.plus_shootforce();
					}
				}


			if (!(status.ended || status.maprefresh || status.gravity || status.under_fire || !status.waiting_for_enemy_move))//enemy köre
				{
				status.waiting_for_enemy_move = false;
				fegyver = enemy[map.get_round() - 1];
				fegyver->calculate_angle_and_power(heightmap, map.getdifficulty(), player.getpos(), enemy);
				status.under_fire = true;
				}





			if (ev.type == ALLEGRO_EVENT_TIMER)
				{
				if (status.maprefresh)
					{
					if (!map.refresh())
						status.maprefresh = false;
					heightmap = map.heightMap();
					}
				if (status.gravity)
					{
					bool is_gravity_needed_1 = player.gravity(heightmap);
					bool is_gravity_needed_2 = enemy.gravity(heightmap);
					if (!(is_gravity_needed_1 || is_gravity_needed_2))
						status.gravity = false;

					}
				if (status.gravity || status.maprefresh)
					{
					heightmap = map.heightMap();
					}
				if (map.get_round() > 10 && !status.maprefresh && !status.gravity)
					status.ended = true;
				al_clear_to_color(BACKGND);
				map.print();
				player.print(heightmap);
				enemy.print(heightmap);
				if (status.under_fire)
					{
					fegyver->print();
					fegyver->move(heightmap);

					bool help_bool;

					if (status.enemyturn)
						help_bool = fegyver->destroy(map, heightmap, enemy, player);
					else
						help_bool = fegyver->destroy(map, heightmap, player, enemy);


					if (!help_bool)
						{
						status.under_fire = false;
						status.enemyturn = !status.enemyturn;
						if (status.enemyturn)
							status.waiting_for_enemy_move = true;
						else
							{
							map++;
							status.waiting_for_enemy_move = false;
							}
						status.maprefresh = true;
						status.gravity = true;
						enemy.move(heightmap, true);
						}



					}
				print_texts(font_16, player, enemy, map);
				al_flip_display();
				}
			if (status.ended)
				{
				player_score = player.getscore();
				enemy_score = enemy.getscore();
				break;
				}
			if (status.exit)
				break;
			}

		//10.kör vége, eredményhírdetés, új játék felajánlása
		if (status.exit)
			break;
		if (player_score == enemy_score)
			al_draw_textf(font_24, BLACK, WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Draw");
		else
			al_draw_textf(font_24, BLACK, WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "You %s!", player_score > enemy_score ? "Win" : "Lose");
		al_draw_textf(font_24, BLACK, WIDTH / 4, 320, ALLEGRO_ALIGN_CENTRE, "%s", "New Game");
		al_draw_textf(font_24, BLACK, 3 * WIDTH / 4, 320, ALLEGRO_ALIGN_CENTRE, "%s", "Quit");
		al_draw_rectangle(WIDTH / 4 - 70, 320, WIDTH / 4 + 70, 350, BLACK, 3);
		al_draw_rectangle(3 * WIDTH / 4 - 30, 320, 3 * WIDTH / 4 + 35, 350, BLACK, 3);

		al_flip_display();

		while (2)
			{
			al_wait_for_event(event_queue, &ev);
			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
				{
				int x = ev.mouse.x;
				int y = ev.mouse.y;
				if (x > WIDTH / 4 - 70 && x < WIDTH / 4 + 70 && y>320 && y < 350) //new game
					{
					status.ended = false;
					status.exit = false;
					status.gravity = true;
					status.maprefresh = true;
					break;
					}
				if (x >3 * WIDTH / 4 - 30 && x < 3 * WIDTH / 4 + 35 && y>320 && y < 350)//exit
					{
					status.exit = true;
					break;
					}
				}
			}
		if (status.exit)
			break;

		}


	al_destroy_font(font_10);
	al_destroy_font(font_16);
	al_destroy_font(font_24);
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_uninstall_keyboard();
	al_uninstall_mouse();
	al_shutdown_font_addon();
	al_shutdown_ttf_addon();
	al_shutdown_primitives_addon();
	return 0;



	}