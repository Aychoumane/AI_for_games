#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <string>
#include "mybt.h"

bt_t B;
int boardwidth = 0;
int boardheight = 0;
bool white_turn = true;

#ifndef VERBOSE_RAND_PLAYER
#define VERBOSE_RAND_PLAYER
bool verbose = true;
bool showboard_at_each_move = true;
#endif

void help() {
  fprintf(stderr, "  quit\n");
  fprintf(stderr, "  echo ON | OFF\n");
  fprintf(stderr, "  help\n");
  fprintf(stderr, "  name <PLAYER_NAME>\n");
  fprintf(stderr, "  newgame <NBCOL> <NBLINE>\n");
  fprintf(stderr, "  genmove\n");
  fprintf(stderr, "  play <L0C0L1C1>\n");
  fprintf(stderr, "  showboard\n");
}
void name() {
  printf("= rand_player\n\n");
}
void newgame() {
  if((boardheight < 1 || boardheight > 10) && (boardwidth < 1 || boardwidth > 10)) {
    fprintf(stderr, "boardsize is %d %d ???\n", boardheight, boardwidth);
    printf("= \n\n");
    return;
  }
  B.init(boardheight, boardwidth);
  white_turn = true;
  if(verbose) fprintf(stderr, "ready to play on %dx%d board\n", boardheight, boardwidth);
  printf("= \n\n");
}
void showboard() {
  B.print_board(stderr);
  printf("= \n\n");
}

//génération d'un mouvement. 
void genmove() {
  //Test et condition de fin de partie. 
  int ret = B.endgame();
  if(ret != EMPTY) {
    fprintf(stderr, "game finished\n");
    if(ret == WHITE) fprintf(stderr, "white player wins\n");
    else fprintf(stderr, "black player wins\n");
    printf("= \n\n");
    return;
  }
  bt_move_t m = B.get_rand_move();
  //Calcul du meilleur coup 
  bt_t temp = B ; 
  int score = 0 ; 
  int score_max = -1 ; 
  int indice_score_max = -1 ; 
  int score_nb_piece ;
  int distance_sum ;  
  //Tour blanc, joue avec une heuristique, et évalue le meilleur mouvement. 
  //Le mouvement 'm' est modifié. 
    if(white_turn)
    {
      for(int i = 0 ; i< temp.nb_moves ; i ++ ){
        //printf("Val de nb moves : %d\n ", temp.nb_moves) ; 
        score_nb_piece = 0;
        distance_sum = 0; 
        score = 0 ;
    
        if(temp.can_play(temp.moves[i])){
          temp.play(temp.moves[i]) ; 
          score_nb_piece =  temp.nb_black_pieces - temp.nb_white_pieces  ;
    
          // Évalue la proximité des pièces à la ligne d'arrivée de l'adversaire.
         
          for (int i = 0; i < temp.nb_white_pieces; i++) {
              distance_sum += temp.nbl - temp.white_pieces[i].line;
          }
          for (int i = 0; i < temp.nb_black_pieces; i++) {
              distance_sum -= temp.black_pieces[i].line;
          }
        }
         
        score = 5* score_nb_piece +  3* distance_sum; 
        if(score > score_max){
          score_max = score ; 
          indice_score_max = i ; 
        }
        //Réinitialisation au tableau de départ 
        temp = B ; 
      }
      m = B.moves[indice_score_max] ; 
      
      printf(" m -> : %d %d %d %d \n", m.line_i, m. col_i , m.line_f , m.col_f); 
      printf(" best -> : %d %d %d %d \n", B.moves[indice_score_max].line_i, B.moves[indice_score_max]. col_i , B.moves[indice_score_max].line_f , B.moves[indice_score_max].col_f);
      B.play(B.moves[indice_score_max]);
    }
    // Tour noir, joue avec rand_move 
    // Le mouvement 'm' n'est pas modifié. 
    else{
      printf("Tour noir\n"); 
      B.play(m);
    }
  // Blabla 
  if(verbose) {
    m.print(stderr, white_turn, B.nbl);
    fprintf(stderr, "\n");
  }
  //Changement de tour et affichage
  white_turn = !white_turn;
  printf("= %s\n\n", m.tostr(B.nbl).c_str());
  B.print_board() ; 
}
void play(char a, char b, char c, char d) {
  bt_move_t m;
  m.line_i = boardheight-(a-'0');
  m.col_i = b-'a';
  m.line_f = boardheight-(c-'0');
  m.col_f = d-'a';
  if(B.can_play(m)) {
    B.play(m);
    if(verbose) {
      m.print(stderr, white_turn, B.nbl);
      fprintf(stderr, "\n");
    }
    white_turn = !white_turn;
  } else {
    fprintf(stderr, "CANT play %d %d %d %d ?\n", m.line_i, m.col_i, m.line_f, m.col_f);
  }
  if(showboard_at_each_move == true ) showboard();
  printf("= \n\n");
}
int main(int _ac, char** _av) {
  bool echo_on = false;
  setbuf(stdout, 0);
  setbuf(stderr, 0);
  if(verbose) fprintf(stderr, "heuris_player started\n");
  char a,b,c,d; // for play cmd
  for (std::string line; std::getline(std::cin, line);) {
    if(verbose) fprintf(stderr, "rand_player receive %s\n", line.c_str());
    if(echo_on) if(verbose) fprintf(stderr, "%s\n", line.c_str());
    if(line.compare("quit") == 0) { printf("= \n\n"); break; }
    else if(line.compare("echo ON") == 0) echo_on = true;
    else if(line.compare("echo OFF") == 0) echo_on = false;
    else if(line.compare("help") == 0) help();
    else if(line.compare("name") == 0) name();
    else if(sscanf(line.c_str(), "newgame %d %d\n", &boardheight, &boardwidth) == 2) newgame();
    else if(line.compare("g") == 0) genmove();
    else if(sscanf(line.c_str(), "play %c%c%c%c\n", &a,&b,&c,&d) == 4) play(a,b,c,d);
    else if(line.compare("showboard") == 0) showboard();
    else if(line.compare(0,2,"//") == 0) ; // just comments
    else fprintf(stderr, "???\n");
    if(echo_on) printf(">");
  }
  if(verbose) fprintf(stderr, "bye.\n");

  return 0;
}
