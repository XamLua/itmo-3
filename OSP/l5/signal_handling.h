#ifndef __SIG_H_H__
#define __SIG_H_H__

void handle_hup(int num);
void handle_int(int num);
void handle_term(int num);
void handle_usr1(int num);
void handle_usr2(int num);

#endif
