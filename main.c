#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT_MAP 5000
#define WIDTH_MAP 10000

#define MOVE_UP 'w'
#define MOVE_DOWN 's'
#define MOVE_LEFT 'a'
#define MOVE_RIGHT 'd'

#define TURN_UP 'W'
#define TURN_DOWN 'S'
#define TURN_LEFT 'A'
#define TURN_RIGHT 'D'

#define SET_BLOCK '\n' //enter
#define DELETE_BLOCK 127
#define SELECT_BLOCK 'e'
#define BLOCK_SOLID '1'
#define BLOCK_TR_FG '2'
#define BLOCK_TR_BG '3'

#define QUIT 'q'



struct _coords {
	int th, tw;
	int mh, mw;
	int pty, ptx;
	int pmy, pmx;
	int my, mx;
	int diry, dirx;
		
};

struct _block {
	char sym;
	char tr;
};

struct _map {
	char **sym;
	char **tr;	
};

struct _coords player_move (int diry, int dirx, int rot, struct _coords coords, struct _map map){
	if (!rot){
		coords.diry = diry;
		coords.dirx = dirx;
	}else if (map.tr[coords.pmy + diry][coords.pmx + dirx] != '#'\
	&& (coords.my < 0 || diry == 1 || dirx != 0)\
	&& (coords.mx < 0 || dirx == 1 || diry != 0)\
	&& (coords.my + coords.mh > coords.th || diry == -1 || dirx != 0)\
	&& (coords.mx + coords.mw > coords.tw || dirx == -1 || diry != 0)) {
		coords.pmy += diry;
		coords.pmx += dirx;
		coords.my -= diry;
		coords.mx -= dirx;
	} 
	
	return coords;
}

struct _map set_block (struct _block curr_block, struct _coords coords, struct _map map){
	map.sym[coords.pmy + coords.diry][coords.pmx + coords.dirx] = curr_block.sym;
	map.tr[coords.pmy + coords.diry][coords.pmx + coords.dirx] = curr_block.tr;
	return map;
}

struct _map delete_block (struct _coords coords, struct _map map){
	map.sym[coords.pmy + coords.diry][coords.pmx + coords.dirx] = ' ';
	map.tr[coords.pmy + coords.diry][coords.pmx + coords.dirx] = '_';
	return map;
}

void draw_map (struct _coords coords, struct _map map){
	
		for (int i = -coords.my; i < -coords.my + coords.th; i++)
			for (int j = -coords.mx; j < -coords.mx + coords.tw; j++) 
				if(map.tr[i][j] == '_') 
					mvaddch(coords.my + i, coords.mx + j, map.sym[i][j]);

		mvaddch(coords.pty, coords.ptx, 'O');
		mvaddch(coords.pty + coords.diry, coords.ptx + coords.dirx, '+');
		
		for (int i = -coords.my; i < -coords.my + coords.th; i++)
			for (int j = -coords.mx; j < -coords.mx + coords.tw; j++) 
				if(map.tr[i][j] == '#' || map.tr[i][j] == '-') 
					mvaddch(coords.my + i, coords.mx + j, map.sym[i][j]);
}

struct _map generate_map (struct _coords coords, struct _map map){
	map.sym = (char**)malloc(sizeof(char*) * coords.mh);
	for (int i = 0; i < coords.mh; i++)
		map.sym[i] = (char*)malloc(sizeof(char*) * coords.mw);
	
	map.tr = (char**)malloc(sizeof(char*) * coords.mh);
	for (int i = 0; i < coords.mh; i++)
	    map.tr[i] = (char*)malloc(sizeof(char*) * coords.mw);
	for (int i = 0; i < coords.mh; i++)
		for (int j = 0; j < coords.mw; j++){
			map.sym[i][j] = ' ';
			map.tr[i][j] = '_';
		}
		
	for (int i = 0; i < coords.mh; i++)
		for (int j = 0; j < coords.mw; j++)
			if (map.tr[i][j] == '_' && !(rand() % 12)) map.sym[i][j] = '^';
	return map;
		
}

void debug (struct _coords coords){
	int n = 0;
	mvprintw(n++, 0, "Map Y: %i", coords.my);
	mvprintw(n++, 0, "Map X: %i", coords.mx);
	mvprintw(n++, 0, "Player on Map Y: %i", coords.pmy);
	mvprintw(n++, 0, "Player on Map X: %i", coords.pmx);
	mvprintw(n++, 0, "Player on Terminal Y: %i", coords.pty);
	mvprintw(n++, 0, "Player on Terminal X: %i", coords.ptx);
	}


int main ()
{
	initscr();
	halfdelay(1);
	curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
		
	
	struct _coords coords;

	getmaxyx(stdscr, coords.th, coords.tw);

	coords.mh = HEIGHT_MAP + coords.th / 2;
	coords.mw = WIDTH_MAP + coords.tw / 2;
	struct _map map;
	
	struct _block curr_block;
	curr_block.sym = '#';
	curr_block.tr = '#';

	coords.pty = coords.th / 2;
	coords.ptx = coords.tw / 2;

	coords.pmy = coords.mh / 2;
	coords.pmx = coords.mw / 2;

	coords.my = coords.pty - coords.pmy;
	coords.mx = coords.ptx - coords.pmx;

	coords.diry = 0;
	coords.dirx = 1;
	
	char ch = 0;
	
	printf("Generating map, please wait...\n");

	srand(time(NULL));
	map = generate_map(coords, map);

	
	int run = 1;
	while(run){

		ch = getch();
	
		switch(ch){
			case QUIT: run = 0;
			break;
			case SET_BLOCK: map = set_block(curr_block, coords, map);
			break;
			case DELETE_BLOCK: map = delete_block(coords, map);
			break;
			case SELECT_BLOCK: curr_block.sym = getchar();
			break;
			case BLOCK_SOLID: curr_block.tr = '#';
			break;
			case BLOCK_TR_FG: curr_block.tr = '-';
			break;
			case BLOCK_TR_BG: curr_block.tr = '_';
			break;

			case MOVE_UP: coords = player_move(-1, 0, 1, coords, map);
			break;
			case MOVE_DOWN: coords = player_move(1, 0, 1, coords, map);
			break;
			case MOVE_LEFT: coords = player_move(0, -1, 1, coords, map);
			break;
			case MOVE_RIGHT: coords = player_move(0, 1, 1, coords, map);
			break;
			case TURN_UP: coords = player_move(-1, 0, 0, coords, map);
			break;
			case TURN_DOWN: coords = player_move(1, 0, 0, coords, map);
			break;
			case TURN_LEFT: coords = player_move(0, -1, 0, coords, map);
			break;                                             
			case TURN_RIGHT: coords = player_move(0, 1, 0, coords, map);
			break;


		}
		
		draw_map(coords, map);
		debug(coords);		
		
	}



	for(int i = 0; i < coords.mh; i++){
		free(map.sym[i]);
		free(map.tr[i]);
		}
	free(map.sym);
	free(map.tr);
	endwin();
	return 0;
}
