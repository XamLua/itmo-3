/*

Программа для расчёта неэкспоненциальных РСеМО. Результаты не сильно впечатляют, но т.к. большинство 
формул это апроксимации, так и должно быть. 
Особенно плохие результаты при детермированных входных потоках, лучше всего при экспонентах.
Если время ожидания или длина очереди отрицательные, то это значит что они нулевые, т.к. коэффициент
в формуле Кимуры может быть отрицательным. 

*/

#include "network.hpp"
#include <stdio.h>
#include <math.h>

long double prob_matrix[NODE_COUNT][NODE_COUNT][CLASS_COUNT]; //Матрица вероятностей переходов

long double cv_matrix[NODE_COUNT][NODE_COUNT][CLASS_COUNT]; //Матрица вспомогательных коэффициентов вариации (???)

struct Node nodes[NODE_COUNT]; //Узлы СеМО (в том числе фиктивный входной)

int main(int argc, char const *argv[])
{
	init_nodes();
	init_probs();
	init_cvs();
	init_arrival_rates();
	init_util();

	//Сам процесс декомпозиции, число итераций взято с запасом
	for (int i = 0; i < 100; ++i)
	{
		iter_merge_chill();
		iter_flow_chill();
		iter_split();
	}

	calc_kimura();

	print_results();

	return 0;
}
 
//Декомпозиция по Whitt
void iter_merge()
{
	long double temp;
	//Для каждого узла
	for (int i = 1; i < NODE_COUNT; i++)
	{
		//Для каждого класса заявок 
		for (int z = 0; z < CLASS_COUNT; ++z)
		{
			temp = 0;
			for (int j = 1; j < NODE_COUNT; ++j)
			{
				temp+=cv_matrix[j][i][z]*nodes[j].lam_i[z]*prob_matrix[j][i][z];
			}

			temp+=cv_matrix[0][i][z]*nodes[0].lam_i[z]*prob_matrix[0][i][z];

			temp = nodes[i].lam_i[z] == 0 ? 0 : temp/nodes[i].lam_i[z];
			nodes[i].c_a_i[z] = temp;
		}

		//Находим среднее значение
		nodes[i].c_a = 0;
		for (int z = 0; z < CLASS_COUNT; ++z)
			nodes[i].c_a+=nodes[i].c_a_i[z]*nodes[i].lam_i[z];
		nodes[i].c_a /= nodes[i].lam; 

	}
}

void iter_flow()
{
	long double temp;
	//Для каждого узла
	for (int i = 1; i < NODE_COUNT; ++i)
	{
		temp = 1;
		temp+= (nodes[i].p*nodes[i].p*(nodes[i].c_b - 1.0))/sqrt(nodes[i].m);
		temp+= (1.0 - nodes[i].p*nodes[i].p) * (nodes[i].c_a - 1.0);
		nodes[i].c_d = temp;
	}
}

void iter_split()
{
	//Пересчитать вспомогательные коэффициенты вариации
	for (int i = 1; i < NODE_COUNT; ++i)
	{
		for (int j = 1; j < NODE_COUNT; ++j)
		{
			for (int z = 0; z < CLASS_COUNT; ++z)
			{
				cv_matrix[i][j][z] = 1.0 + prob_matrix[i][j][z]*(nodes[i].c_d - 1.0);
			}
		}
	}
}

void init_util()
{
	//Подсчёт загрузок
	for (int i = 0; i < NODE_COUNT; ++i)
		for (int j = 0; j < CLASS_COUNT; ++j)
			nodes[i].p_i[j] = nodes[i].lam_i[j]/(nodes[i].m*nodes[i].u); 

	for (int i = 0; i < NODE_COUNT; ++i)
		for (int j = 0; j < CLASS_COUNT; ++j)
			nodes[i].p += nodes[i].p_i[j];
}

void init_arrival_rates()
{
	//Интенсивности входных потоков узлов (коэффициенты передач должны быть предподсчитаны).
	nodes[0].lam_i[0] = 1.0/6.646;
	nodes[0].lam_i[1] = 1.0/18.0;
	nodes[1].lam_i[0] = nodes[0].lam_i[0] * 1.0;
	nodes[1].lam_i[1] = 0;
	nodes[2].lam_i[0] = nodes[0].lam_i[0] * 0.2;
	nodes[2].lam_i[1] = nodes[0].lam_i[1] * 0.2;
	nodes[3].lam_i[0] = nodes[0].lam_i[0] * 1.052631579;
	nodes[3].lam_i[1] = nodes[0].lam_i[1] * 1.052631579;
	nodes[4].lam_i[0] = nodes[0].lam_i[0] * 0.1052631579;
	nodes[4].lam_i[1] = nodes[0].lam_i[1] * 0.1;
	nodes[5].lam_i[0] = nodes[0].lam_i[0] * 1.0;
	nodes[5].lam_i[1] = 0;

	for (int i = 0; i < NODE_COUNT; ++i)
		for (int j = 0; j < CLASS_COUNT; ++j)
			nodes[i].lam += nodes[i].lam_i[j];
}

void init_cvs()
{
	//Начальные значения
	for (int i = 0; i < NODE_COUNT; ++i)
		for (int j = 0; j < NODE_COUNT; ++j)
			for (int z = 0; z < CLASS_COUNT; ++z)
				if (i == 0)
					cv_matrix[i][j][z] = nodes[0].c_d;
				else
					cv_matrix[i][j][z] = 1;
}

void init_probs()
{
	//Матрица вероятностей переходов
	
	//Заполнение нулями
	for (int i = 0; i <NODE_COUNT ; ++i)
		for (int j = 0; j < NODE_COUNT; ++j)
			for (int z = 0; z < CLASS_COUNT; ++z)
				prob_matrix[i][j][z] = 0;

	//Внести вероятности для СеМО
	prob_matrix[0][1][0] = 0.8;
	prob_matrix[0][2][0] = 0.2;
	prob_matrix[2][1][0] = 1;
	prob_matrix[1][3][0] = 1;
	prob_matrix[3][3][0] = 0.05;
	prob_matrix[3][4][0] = 0.1;
	prob_matrix[3][5][0] = 0.85;
	prob_matrix[4][5][0] = 1;

	prob_matrix[0][2][1] = 0.2;
	prob_matrix[0][3][1] = 1;
	prob_matrix[0][4][1] = 0.1;
	prob_matrix[3][3][1] = 0.05;
}

void init_nodes()
{
	//Входной узел (фиктивный)
	nodes[0].c_d = 1;

	//Регистрация
	nodes[1].m = 12;
	nodes[1].u = 1.0/60.0;
	nodes[1].c_b = (40.0/(sqrt(3)*120.0))*40.0/(sqrt(3)*120.0);

	//Упаковка
	nodes[2].m = 10;
	nodes[2].u = 1.0/60.0;
	nodes[2].c_b = 40.0/(sqrt(3.0)*120.0)*40.0/(sqrt(3.0)*120.0);

	//Досмотр
	nodes[3].m = 24;
	nodes[3].u = 1.0/100.0;
	nodes[3].c_b = 60.0/(sqrt(3.0)*200.0)*60.0/(sqrt(3.0)*200.0);

	//Дьюти фри
	nodes[4].m = 2;
	nodes[4].u = 1.0/60.0;
	nodes[4].c_b = 40.0/(sqrt(3.0)*120.0)*40.0/(sqrt(3.0)*120.0);

	//Выход на посадку
	nodes[5].m = 4;
	nodes[5].u = 1.0/20.0;
	nodes[5].c_b = 20.0/(sqrt(3.0)*40.0)*20.0/(sqrt(3.0)*40.0);
}

long double factorial(long double n)
{
  return (n == 1 || n == 0) ? 1.0 : factorial(n - 1.0) * n;
}

void print_results()
{
	for (int i = 1; i < NODE_COUNT; ++i)
	{
		printf("-----------Узел #%d-----------\n", i);
		printf("w0 = %Lf\n", nodes[i].w_i[0]); //Среднее время ожидания класса 1
		printf("w1 = %Lf\n", nodes[i].w_i[1]); //Среднее время ожидания класса 2
		printf("W = %Lf\n", nodes[i].W); //Среднее время ожидания по классам
		//printf("G1 = %Lf\n", nodes[i].G_i[0]); //Коэффициент для KLB формулы класса 1
		//printf("G2 = %Lf\n", nodes[i].G_i[1]); //Коэффициент для KLB формулы класса 2
		printf("q = %Lf\n", nodes[i].q); //Средняя длина очереди в узле
		printf("p = %Lf\n", nodes[i].p); //Загрузка
		printf("p0 = %Lf\n", nodes[i].p_i[0]); //Загрузка класса 1
		printf("p1 = %Lf\n", nodes[i].p_i[1]); //Загрузка класса 2
		//printf("lam = %Lf\n", nodes[i].lam); //Интенсивность входного (и выходного) потока
		//printf("u = %Lf\n", nodes[i].u); // Интенсивность обслуживания
		//printf("m = %Lf\n", nodes[i].m); // Число приборов
		printf("p_i = %Lf\n", nodes[i].P_i);
		printf("Pm = %Lf\n", nodes[i].Pm);
		printf("c_a = %Lf\n", nodes[i].c_a); //Коэффициент вариации входного потока
		//printf("c_b = %Lf\n", nodes[i].c_b); //Коэффициент вариации интенсивности обслуживания
		printf("c_d = %Lf\n", nodes[i].c_d); //Коэффициет вариации выходного потока
	}
}

//Подсчёт характеристик по Кимуре
void calc_kimura()
{
	for (int i = 1; i < NODE_COUNT; ++i)
	{
		//Посчитать pi
		nodes[i].P_i = 0;
		for (int k = 0; k < nodes[i].m; ++k)
			nodes[i].P_i += pow(nodes[i].m*nodes[i].p, (long double) k) / ((long double) factorial(k));
		nodes[i].P_i += pow(nodes[i].m*nodes[i].p, nodes[i].m) / ((long double) factorial(nodes[i].m)) /(1.0-nodes[i].p);
		nodes[i].P_i = 1.0/nodes[i].P_i;

		//Посчитать Pm
		nodes[i].Pm = pow(nodes[i].m*nodes[i].p, nodes[i].m) / ((long double)factorial(nodes[i].m)*(1.0-nodes[i].p)) * nodes[i].P_i;
		
		//Посчитать средние длины очередей для классов
		for (int z = 0; z < CLASS_COUNT; ++z)
		{
			//Для M/M/m
			nodes[i].q_i[z] = nodes[i].p_i[z]/(1.0-nodes[i].p)*nodes[i].Pm;

			nodes[i].q_i[z] /= 2;

			nodes[i].q_i[z] *= nodes[i].c_a_i[z] + nodes[i].c_b;

			long double temp = (1.0 - nodes[i].c_a_i[z]);

			temp /= 1.0 - 4.0*coop(nodes[i].m, nodes[i].p);

			temp *= exp(-2.0/3.0*(1.0-nodes[i].p)/nodes[i].p);

			temp += (1.0 - nodes[i].c_b) /
					(1.0 + coop(nodes[i].m, nodes[i].p));

			temp += nodes[i].c_a_i[z] + nodes[i].c_b - 1.0;

			nodes[i].q_i[z] /= temp;

		}

		//Посчитать среднее по всем классам
		nodes[i].q = 0;
		for (int z = 0; z < CLASS_COUNT; ++z)
			nodes[i].q += nodes[i].q_i[z];

		//Посчитать время ожидания по классам и среднее
		for (int z = 0; z < CLASS_COUNT; ++z)
			nodes[i].w_i[z] = nodes[i].lam_i[z] == 0 ? 0 : nodes[i].q_i[z]/nodes[i].lam_i[z];

		nodes[i].W = 0;
		for (int z = 0; z < CLASS_COUNT; ++z)
			nodes[i].W += nodes[i].lam_i[z]/nodes[i].lam * nodes[i].w_i[z];
	}
}

//Коэффициент кооперации для формулы Кимуры
long double coop(long double m, long double p)
{
	double temp = 1;

	temp *= (1.0 - p);

	temp *= (m - 1.0);

	temp *= sqrt(4.0 + 5.0*m) - 2.0;

	temp /= 16.0*m*p;

	return temp;
}

//Декомпозия по Chylla
void iter_merge_chill()
{
	for (int i = 1; i < NODE_COUNT; ++i)
	{
		//Для каждого класса заявок
		for (int z = 0; z < CLASS_COUNT; ++z)
		{
			nodes[i].c_a_i[z] = 1;

			for (int j = 0; j < NODE_COUNT; ++j)
			{
				if (nodes[i].lam_i[z] != 0)
				{
					nodes[i].c_a_i[z] += nodes[j].lam_i[z]*prob_matrix[j][i][z]/nodes[i].lam_i[z]*
										 (cv_matrix[j][i][z] - 1.0);
				}
			}
		}

		nodes[i].c_a = 1;
		for (int z = 0; z < CLASS_COUNT; ++z)
		{
			nodes[i].c_a += nodes[i].lam_i[z]/nodes[i].lam*(nodes[i].c_a_i[z]-1.0);
		}
	}
}

void iter_flow_chill()
{
	calc_kimura();
	
	for (int i = 1; i < NODE_COUNT; ++i)
	{
		
		nodes[i].c_d = 1;

		nodes[i].c_d += pow(nodes[i].Pm, 2.0)*(nodes[i].c_b - 1.0);

		nodes[i].c_d += (1.0 - pow(nodes[i].Pm, 2.0))*(nodes[i].c_a - 1.0);
	}
}

//Подсчёт характеристик по KLB
void calc_measures_klb()
{
	for (int i = 1; i < NODE_COUNT; ++i)
	{
		//Посчитать pi
		nodes[i].P_i = 0;
		for (int k = 0; k < nodes[i].m; ++k)
			nodes[i].P_i += pow(nodes[i].m*nodes[i].p, (long double) k) / ((long double) factorial(k));

		nodes[i].P_i += pow(nodes[i].m*nodes[i].p, nodes[i].m) / ((long double) factorial(nodes[i].m)) /(1.0-nodes[i].p);
		nodes[i].P_i = 1.0/nodes[i].P_i;

		//Посчитать Pm
		nodes[i].Pm = pow(nodes[i].m*nodes[i].p, nodes[i].m) / ((long double)factorial(nodes[i].m)*(1.0-nodes[i].p)) * nodes[i].P_i;

		for (int z = 0; z < CLASS_COUNT; ++z)
		{
			//Для M/M/m
			nodes[i].q_i[z] = nodes[i].p_i[z]/(1.0-nodes[i].p)*nodes[i].Pm;

			//Для AC
			nodes[i].q_i[z] *= (nodes[i].c_a_i[z] + nodes[i].c_b)/2;

			//Посчитать фактор коррекции KLB
			if (nodes[i].c_a_i[z] > 1.0)
				nodes[i].G_i[z] = exp(-(1.0-nodes[i].p_i[z])*(nodes[i].c_a_i[z]-1.0)/(nodes[i].c_a_i[z] + nodes[i].c_b));
			else
				nodes[i].G_i[z] = exp(-2.0/3.0*(1.0-nodes[i].p_i[z])/nodes[i].p_i[z]*
							 pow((nodes[i].c_a_i[z] - 1.0),2.0)/(nodes[i].c_a_i[z] + nodes[i].c_b));	

			nodes[i].q_i[z] *= nodes[i].G_i[z];
		}

		//Посчитать среднее по всем классам
		nodes[i].q = 0;
		for (int z = 0; z < CLASS_COUNT; ++z)
			nodes[i].q += nodes[i].q_i[z];

		//Посчитать времена ожидания по классам и среднее
		for (int z = 0; z < CLASS_COUNT; ++z)
			nodes[i].w_i[z] = nodes[i].lam_i[z] == 0 ? 0 : nodes[i].q_i[z]/nodes[i].lam_i[z];

		nodes[i].W = 0;
		for (int z = 0; z < CLASS_COUNT; ++z)
			nodes[i].W += nodes[i].lam_i[z]/nodes[i].lam * nodes[i].w_i[z];
	}
}