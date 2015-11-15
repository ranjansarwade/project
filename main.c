/*****************************************************************************
 * Copyright (C) Ranjan Sarwade sarwaderm14.comp@coep.ac.in
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#include <stdio.h>
#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <errno.h>
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4
#define prate 200
#define grate 150
#define srate 100
#define trate 2
#define pf 10
typedef struct node {
	int seat;
	char type;
	struct node *next;
}node;
typedef struct matrix { 
	node **a;
	int n;
}matrix;
float billing(int n, char ch);
int booking(MENU *, int *n, char *ch, matrix *m);
void reciept(char type,int psn, float price,int n, char ch);
void init(matrix *m, int n);
void insertseat(matrix *m, int from, int to, char type);
void insertseat(matrix *m, int from, int to, char type);
void printmatrix(matrix *m, int n);
char *choices[] = {
		"ENGLISH MOVIES",
		"HINDI MOVIES",
		"MARATHI MOVIES",
		"EXIT",
	};
char *english[] = {
		"THE MARTIAN",
		"INTERSTELLAR",
		"STAR WARS",
		"MISSION IMPOSSIBLE V",
		"BACK"
	};
char *hindi[] = {
		"TALWAR",
		"PK",
		"SHAANDAR",
		"BAJIRAO MASTANI",
		"BACK",
	};
char *marathi[] ={
		"COFEE ANI BARACH KAHI",
		"KATYAR KALJAT GHUSLI",
		"ZENDA",
		"ELIZABETH EKADASHI",
		"BACK",
	};
void func(char *name);
int main(int argc, char *argv[]) {
	ITEM **my_items, **e_items, **h_items, **m_items;
	int n, k, c;
	char ch;
	float prize;
	FILE *fp;
	int i1, j1, j;
	matrix m;
	fp = fopen("seats.txt", "r");
	if(fp == NULL) {
		perror("fopen failed:");
		return errno;
	}
	fscanf(fp, "%d", &n);
	init(&m, n);
	/*Code to read seat matrix from file */
	for(i1 = 0; i1 < n; i1++)
		for(j1 = 0; j1 < n;) {
			fscanf(fp, "%c", &ch);
			if(ch == 'p' || ch == 'g' || ch == 's' || ch == 'B'){
				insertseat(&m, i1, j1, ch);
				j1++;
			}
		}
	fclose(fp);
	/*Initialise Curses*/	
	static int pos = 0, pos1 = 0, pos2 = 0, pos3 = 0;
	MENU *my_menu,*e_menu,*h_menu,*m_menu ;
	int n_choices,m_choices,h_choices,e_choices, i;
	initscr();
	start_color();	
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	n_choices = ARRAY_SIZE(choices);
	e_choices = ARRAY_SIZE(english);
	h_choices = ARRAY_SIZE(hindi);
	m_choices = ARRAY_SIZE(marathi);
	my_items = (ITEM**)calloc(n_choices +1, sizeof(ITEM *));
	e_items = (ITEM**)calloc(e_choices +1, sizeof(ITEM *));
	h_items = (ITEM**)calloc(h_choices +1, sizeof(ITEM *));
	m_items = (ITEM**)calloc(m_choices +1, sizeof(ITEM *));
	/*Initialise Menu*/
	for(i = 0;i < n_choices; ++i) {
		my_items[i] = new_item(choices[i], NULL);
		set_item_userptr(my_items[i], func);
	}
	for(i = 0;i < e_choices; ++i) {
		e_items[i] =new_item(english[i], NULL);
		set_item_userptr(e_items[i], func);
		}
	for(i = 0;i < h_choices; ++i) {
		h_items[i] = new_item(hindi[i], NULL);
		set_item_userptr(h_items[i], func);
		}
	for(i = 0;i < m_choices; ++i) {
		m_items[i] =new_item(marathi[i], NULL);
		set_item_userptr(m_items[i], func);
		}
			
	my_items[n_choices] = (ITEM *)NULL;
	e_items[e_choices] = (ITEM *)NULL;
	h_items[h_choices] = (ITEM *)NULL;
	m_items[m_choices] = (ITEM *)NULL;
	my_menu = new_menu((ITEM**)my_items);
	e_menu = new_menu((ITEM**)e_items);
	h_menu = new_menu((ITEM**)h_items);
	m_menu = new_menu((ITEM**)m_items);
	/*Control will reach here when you exit from any language Section*/
	read:
	mvprintw(LINES - 3,0,"Press <Enter> to see the option selected");
	mvprintw(LINES - 2,0,"EXIT to exit and up and down arrow keys to navigate");
	attron(A_BOLD);
	mvprintw(LINES / 2, COLS / 2 - 16, "!!!TICKET BOOKING PORTAL!!!");
	attroff(A_BOLD);
	post_menu(my_menu);
	refresh();
	/*Menu Operations Begin Here*/
	while((c = getch()) != KEY_F(2)) {
		switch(c) {
			case KEY_DOWN:
				menu_driver(my_menu,REQ_DOWN_ITEM);
				if(pos != 3)
					pos++;
				mvprintw(LINES - 4,0, "the key is at %d", pos + 1);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				if(pos != 0)  
					pos--;
				mvprintw(LINES - 4,0, "the key is at %d", pos + 1);
				break;
			case 10 : {
				ITEM *cur;
				void (*p) (char *);
				cur = current_item(my_menu);
				p = item_userptr(cur);
				p((char *)item_name(cur));
				pos_menu_cursor(my_menu);
				if(pos == 0) {
					unpost_menu(my_menu);
					/*Control will reach here if booking is cancelled*/
					read1:
					post_menu(e_menu);
					refresh();
					pos_menu_cursor(e_menu);
					while((c = getch()) != KEY_F(2)) {
						switch(c) { 
							case KEY_DOWN:
								menu_driver(e_menu,REQ_DOWN_ITEM);
								if(pos1 != 4)
								pos1++;
								mvprintw(LINES - 4,0, "the key is at %d", pos1 + 1);
								break;
							case KEY_UP:
								menu_driver(e_menu, REQ_UP_ITEM);
								if(pos1 != 0)  
								pos1--;
								mvprintw(LINES - 4, 0, "the key is at %d", pos1 + 1);
								break;
							case 10:
								 if(pos1 == 4) {
									unpost_menu(e_menu);
									goto read;
								}
								else {
									k = booking(e_menu, &n, &ch, &m);
									if(k == 1) {
										prize = billing(n, ch);
										reciept('e', pos1, prize, n, ch);	
										getch();
										goto read4;
									}
									else/*In case booking is cancelled*/ 
										goto read1;
								}
								break;	
							}	
						}
					}
				else if(pos == 1) {
					unpost_menu(my_menu);
					read2:
					post_menu(h_menu);
					refresh();
					pos_menu_cursor(h_menu);
					while((c = getch()) != KEY_F(2)) {
						switch(c) { 
							case KEY_DOWN:
								menu_driver(h_menu,REQ_DOWN_ITEM);
								if(pos2 != 4)
								pos2++;
								mvprintw(LINES - 4,0, "the key is at %d", pos2 + 1);
								break;
							case KEY_UP:
								menu_driver(h_menu, REQ_UP_ITEM);
								if(pos2 != 0)  
								pos2--;
								mvprintw(LINES - 4, 0, "the key is at %d", pos2 + 1);
								break;
							case 10:
								if(pos2 == 4) {
								unpost_menu(h_menu);
								goto read;
								}
								else {
									k = booking(h_menu, &n, &ch, &m);
									if(k == 1) {
										prize = billing(n, ch);
										reciept('h', pos2, prize, n, ch);
										getch();
										goto read4;
									}
									else 
										goto read2;
								}		
								break;
							
						}
					} 
				}
				else if(pos == 2) {
					unpost_menu(my_menu);
					read3:
					post_menu(m_menu);
					refresh();
					pos_menu_cursor(m_menu);
					while((c = getch()) != KEY_F(2)) {
						switch(c) { 
							case KEY_DOWN:
								menu_driver(m_menu,REQ_DOWN_ITEM);
								if(pos3 != 4)
								pos3++;
								mvprintw(LINES - 4,0, "the key is at %d", pos3 + 1);
								break;
							case KEY_UP:
								menu_driver(m_menu, REQ_UP_ITEM);
								if(pos3 != 0)  
								pos3--;
								mvprintw(LINES - 4, 0, "the key is at %d", pos3 + 1);
								break;
							case 10:
								if(pos3 == 4) {
								unpost_menu(m_menu);
								goto read;
								}
								else {
									k = booking(m_menu, &n, &ch, &m);
									if(k == 1) {
										prize = billing(n, ch);
										reciept('m', pos3, prize, n, ch);
										getch();
										goto read4;
									}
									else 
										goto read3;
								}	
								break;
						}
					}
				}
				else if(pos == 3) {
					unpost_menu(my_menu);
					read4:
					for(i = 0;i < n_choices; ++i)
						free_item(my_items[i]);	
					free_menu(my_menu);
					for(i = 0;i < e_choices; ++i)
						free_item(e_items[i]);	
					free_menu(e_menu);
					for(i = 0;i < h_choices; ++i)
						free_item(h_items[i]);	
					free_menu(h_menu);
					for(i = 0;i < m_choices; ++i)
						free_item(m_items[i]);
					node *tmp;
					fp = fopen("seats.txt","w");
					matrix *o = &m;
					fprintf(fp,"%d\n", o->n);
					for(i = 0; i < o->n; i++)  {
					tmp = o->a[i];
					for(j = 0; j < o->n; j++) {
					fprintf(fp, "%c", tmp->type);
						tmp = tmp->next;
						}
					fprintf(fp, "\n");
					}
					fclose(fp);
					free_menu(m_menu);
					endwin();
					clear();
					exit(0);
				}
				break;
			
			}
			break;
		}
	}
	fclose(fp);
	return 0;
}
void func(char *name) {
	move(20, 0);
	clrtoeol();
	mvprintw(20, 0, "Item selected is: %s", name);
}
/*Reciept Printing Function*/
void reciept(char type, int pos, float prize, int n,char ch) {
	int j;
	printw("RECIEPT :\n");
	if(type == 'e') {
		printw("TYPE: %s\n", choices[0]);
		for(j = 0;j < 5; j++)
			if(pos == j)
				printw("MOVIE: %s\n",english[pos]);
	}
	if(type == 'h') {
		printw("LANGUAGE: %s\n", choices[1]);
		for(j = 0;j < 5; j++)
			if(pos == j)
				printw("MOVIE: %s\n", hindi[pos]);
	}	
	if(type == 'm') {
		printw("LANGUAGE: %s\n", choices[2]);
		for(j = 0;j < 5; j++)
			if(pos == j)
				printw("MOVIE: %s\n", marathi[pos]);
	}
	printw("TICKETS BOOKED =  %d\n", n);
	if(ch == 'p')
		printw("TICKET CLASS : PLATINUM\n");
	else if(ch =='g')
		printw("TICKET CLASS : GOLD\n");
	else if(ch == 's')
		printw("TICKET CLASS: SILVER\n");
	printw("BILL: Rs %f[Inclusive all Taxes and processing fee = 10 Rupees]\n", prize);
	mvprintw(LINES / 2, COLS / 2, "THANK YOU");
	return;
}
/*Booking is done Here*/	
int booking(MENU *s_menu,int *n, char *ch, matrix *m) {
	char c;
	unpost_menu(s_menu);
	initscr();
	echo();
	mvprintw(0, 0, "Type no.of tickets and <enter> and then press\n p - PLATINUM(Rate : 200Rs/head)\n g- GOLD(Rate : 150Rs/head)\n s - SILVER(Rate : 100Rs/head)\n");
	move(LINES/2, COLS/2 - 18);
	printw("How Many Tickets Do You Want??");
	mvscanw(LINES / 2 + 1, COLS / 2 - 18,"%d", n);
	mvprintw(LINES / 2 + 2,COLS / 2 - 18, "what type of seats do you want ??");
	mvscanw(LINES / 2 +3, COLS / 2 -18, "%c", ch);
	clear();
	printmatrix(m, *n);
	mvprintw(0, 0,"Do You Wish To Continue??");
	mvprintw(LINES - 3,0,"Press \n <y> to continue");
	mvprintw(LINES -1 , 0," Any key to go back");
	mvscanw(1,0,"%c", &c);
	refresh();
	clear();
	if(c == 'y') 
		return 1;
	else 
		return 0;
}
/*billing is done here*/
float billing(int n, char ch) {
	int val;
	if (n ==  0)
		return 0;
	float total, tax;
	if(ch == 'p')
		val = n * prate;
	else if(ch == 'g')
		val = n * grate;
	else if(ch == 's')
		val = n * srate;
	tax = val * trate /100 + pf;
	total = val + tax;
	return total;
}
/*Code below will do the seat matrix operations using array of linked lists*/
void init(matrix *m, int n) {
	int i, j;
	node *k, *tmp, *l;
	m->a = (node **)malloc(n * sizeof(node *));	
	m->n = n;
	for(i = 0; i < n; i++) {
		for(j = 0; j < n; j++) {
			k = (node *)malloc(sizeof(node));
			if(j > 0)
				l->next = k;
			k->type = 'O';
			k->seat = j;
			if(j == 0)
				tmp = k;
			if(j == n - 1) 
				k->next = NULL;
			l = k;			
		}
		m->a[i] = tmp;
	}
}
void insertseat(matrix *m, int from, int to, char type) {
	node *tmp;	
	tmp = m->a[from];
	while(tmp->seat != to) 
		tmp = tmp->next;
	tmp->type = type;
}
void printmatrix(matrix *m, int n) {
	int i, j, k;
	node *tmp;
	mvprintw(LINES -6, 10,"Choose seat numbers by entering row(space)Column and enter");
	mvprintw(LINES -5, 0,"P-> Platinum\nG-> Gold\nS->Silver\nB->Booked");
	move(LINES / 2 - 1, COLS / 2);
	printw("  ");
	for(i = 0; i < m->n; i++)
		printw("%d ", i + 1);
	printf("\n");
	for(i = 0; i < m->n; i++)  {
		tmp = m->a[i];
		move(LINES / 2 + i, COLS / 2);
		printw("%d ", i+1);
		for(j = 0; j < m->n; j++) {
			printw("%c ", tmp->type);
			tmp = tmp->next;
		}
		printw("\n");
	}
	mvprintw(0, 10, "Choose %d seats\n", n);
	echo();
	for(k = 0; k < n; k++) {
 		scanw("%d %d", &i, &j);
		insertseat(m, i -1, j - 1, 'B');
	}
	clear();
}
