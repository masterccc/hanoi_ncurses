#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define DBG_X 20

#define RCOLS (COLS - DBG_X)
#define DISKS 7
#define TOWER_SIZE (DISKS+1)
#define TOUR_X(n) ( (RCOLS  / 4) * n)

#define KEY_TO_ZERO 107
#define KEY_QUIT 113
#define KEY_K 107
#define KEY_L 108
#define KEY_M 109
#define KEY_MRESTART 114
#define KEY_MUNDO 106


void dbg_msg(char *msg, WINDOW *dbg_win){

  static int padding = 2;
  mvwprintw(dbg_win, padding, 1, "%s", msg);
  padding++;
}

void drawTour( WINDOW *window, int **tower ){

  int ecart = RCOLS / 4 ;
  int i, s, hauteur;
  int *tmp;
  /* dessine les tours */
  for(i = 0; i < TOWER_SIZE ; i++){
    mvwprintw(window, LINES-2-i, ecart, "|");
    mvwprintw(window, LINES-2-i, 2 * ecart, "|");
    mvwprintw(window, LINES-2-i, 3 * ecart, "|");
  }

  /* dessine les disques */
  for(i = 0 ; i < 3 ; i++)
    for(hauteur=0, tmp = tower[i]; *tmp && hauteur < tower[3][i];tmp++, hauteur++)
      for(s=0; s<*tmp; s++)
	mvwprintw( window, LINES-2-hauteur, (TOUR_X((i+1))+s)-(*tmp/2) ,"=", *tmp); 
}

void clearTower(WINDOW *w){
  int x,y;
  for( y = LINES-DISKS-1 ; y < LINES-1 ; y++){
    for( x=2; x < RCOLS-1 ; x++ )
      mvwprintw( w, y, x, " ");
  }
}

void chsize(int **tower, int index, int val){
  tower[3][index] = tower[3][index] + val ;
}

void dbg_update( WINDOW *dbg, int **tower){
  int i,j;

  
  for(i=1;i<=3;i++){
    wmove(dbg, 2 + i, 1);
    for(j=1;j<DBG_X-1;j++){
      wprintw(dbg, " ");
    }
  }
  
  for( i = 0; i < 3 ; i++){
    wmove(dbg, i + 3, 2 );
    wprintw(dbg, "[");
    for(j = 0; j < tower[3][i] ; j++){
      wprintw(dbg, " %d", tower[i][j]);
    }
    wprintw(dbg, " ]");
  }
 
}
int swap( int src, int dest, int **tower, WINDOW *dbg){
  if( tower[3][src] <= 0 ) return -1;
 
  if(tower[3][dest] > 0){
    if( tower[src][tower[3][src]-1] > tower[dest][tower[3][dest]-1]) return -1;
  }
    tower[dest][tower[3][dest]] = tower[src][tower[3][src]-1];
    tower[src][tower[3][src]-1] = 0;
    chsize(tower, src, -1);
    chsize(tower, dest, 1);  
    
}


void hanoi(int disks, int src, int dest, int inter, int **tower, WINDOW *dbg, WINDOW *main){
  if(disks){
    hanoi(disks-1, src, inter, dest, tower, dbg, main);
    swap(src,dest, tower, dbg);
    clearTower(main);
    drawTour(main, tower);
    wrefresh(main);
    system("sleep 0.1");
    hanoi(disks-1, inter, dest, src, tower,dbg, main);
  }
}
int main(void) {

  WINDOW *main_win, *dbg;

  /* Création tour */
  int **tower, i, nb;
  tower = calloc( DISKS+1, sizeof(int*));
  for( i = 0; i < DISKS ; i++) tower[i] = calloc(1, sizeof(int));
  tower[3] = calloc( 3, sizeof(int)); 
  /* Rangement des disques sur la premiere tour*/
  for( nb= 0, i = (DISKS*2)-1; i > 0; i-=2, nb++) tower[0][nb] = i;

  tower[3][0] = DISKS;
  tower[3][1] = 0;
  tower[3][2] = 0;
  
  initscr();
  noecho();
  curs_set(0);
  main_win = subwin(stdscr, LINES, RCOLS, 0, DBG_X);
  box(main_win, ACS_VLINE, ACS_HLINE);

  dbg = subwin( stdscr, LINES, DBG_X, 0,0);
  box(dbg, ACS_VLINE, ACS_HLINE);
  
  mvwprintw(main_win, 1, (COLS/2)-2, "HANOI", LINES, RCOLS);
  mvwprintw(main_win, 2, 2, "q:quitter");
  mvwprintw(main_win, 3, 2, "k/l/m:selection tour 1,2 ou 3");

  attron(A_STANDOUT);
  mvwprintw(dbg, 1, 2, "[DEBUG INFO]");
  attroff(A_STANDOUT);

  drawTour(main_win, tower);
  dbg_update( dbg, tower);
  
  wrefresh(dbg);
  wrefresh(main_win);

  hanoi(DISKS, 0, 2, 1, tower, dbg, main_win);
  
  clearTower(main_win);
  drawTour(main_win, tower);
  wrefresh(main_win);
  getch();
  endwin();
    
  //free(main_win);
  return 0;
}


