#ifndef __NETWORK__
#define __NETWORK__

#define NODE_COUNT 6

#define CLASS_COUNT 2

struct Node
{
	long double f_i[CLASS_COUNT];
	long double G_i[CLASS_COUNT];
	long double Pm;
	long double P_i;
	long double lam_i[CLASS_COUNT]; //Интенсивность входного (и выходного) потока
	long double lam; //Интенсивность входного (и выходного) потока
	long double c_a_i[CLASS_COUNT]; //Коэффициент вариации входного потока
	long double c_a; //Коэффициент вариации входного потока
	long double m; //Число приборов
	long double u; //Интенсивность обслуживания
	long double c_b; //Коэффициент вариации интенсивности обслуживания
	long double w; //Среднее время ожидания
	long double w_i[CLASS_COUNT]; //Среднее время ожидания
	long double W;
	long double q;
	long double q_i[CLASS_COUNT];
	long double p_i[CLASS_COUNT]; //Загрузка
	long double p; //Загрузка
	long double c_d; //Коэффициет вариации выходного потока
};

void print_results();

void init_probs();

void init_cvs();

void init_nodes();

void init_arrival_rates();

void init_util();

void iter_merge();

void iter_flow();

void iter_merge_chill();

void iter_flow_chill();

void iter_split();

void calc_measures_klb();

void calc_kimura();

long double coop(long double m, long double p);

long double factorial(long double n);
#endif