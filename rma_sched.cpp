#include "stdafx.h"
#include<iostream>
#include<vector>
#include<cmath>
#include<queue>
#include<sstream>
#include<fstream>
#include<ctime>
#define MAX_PERIOD 1000
#define BUFF_SIZE 10
using namespace std;


class task
{
	vector<float> c, p, t, wt, timeStore;
	float condU;
	long double U;
	double value, ratio;
	int size, count, m;
public:
	task()
	{
		U = 0.0;
		condU = 0.0;
		value = 0.0;
		count = 0;
		ratio = 9999999.0;
	}
	void calcTime(float Ti, int m);
	void takeInput();
	void getInput();
	void summ();
	void schedFirst();
	void schedSecond();
	void sortTask();
};


void task::takeInput()
{
	fstream fin; int rc, u;
	fin.open("task_set.txt", fstream::in);
	stringstream ss;
	char buff1[BUFF_SIZE];
	char buff2[BUFF_SIZE];
	char bkp[BUFF_SIZE];
	fin.getline(buff1, BUFF_SIZE);

	ss << buff1;
	ss.getline(buff2, 10, ',');
	size = atoi(buff2);
	cout << "Reading task-set of size: " << size << endl;
	for (int i = 0; i<size; i++)
	{
		fin.getline(buff1, BUFF_SIZE);
		stringstream sts(buff1);

		sts.getline(buff2, 10, ',');
		u = atoi(buff2);
		c.push_back(u);

		sts.getline(buff2, 10, ',');
		u = atoi(buff2);
		p.push_back(u);
	}
	cout << "Completed reading task-set\n";

	p.pop_back();
	c.pop_back();
	for (int i : p)
	{
		cout << i << " ";
	}
	size = p.size();
	cin.ignore();
}

void task::getInput()
{
	float c, p, t;
	cout << "Enter the number of tasks in the task set: ";
	cin >> size;

	for (int i = 0; i<size; i++)
	{
		cout << "Enter the \'c\',\'p\',\'t\' for task #" << i << ": ";
		cin >> c >> p >> t;
		(this->c).push_back(c);
		(this->p).push_back(p);
		(this->t).push_back(t);
	}
}


void task::schedFirst()
{
	vector<float>::iterator pi, ci, ti;
	cout << "Reaches here";
	for (pi = p.begin(), ci = c.begin(); pi != p.end(); ++pi, ++ci)
	{
		U = U + ( (*ci) / (*pi) );
		cout << "Ci: " << *ci << "Pi: " << *pi << "U: " << U << endl;
	}
	cout << "Reaches here too";
	cout << "Calculated Utilization: " << U << endl;
	condU = size * (pow(2.0, (1.0 / size)) - 1.0);
	cout << "Limiting Utilization: " << condU << endl;
	cin.ignore();
	if (U <= condU)
	{
		cout << "The current taskset is scheduable" << endl;
		cin.ignore();
		
	}
	else
	{
		cout << "The current taskset might not be scheduable" << endl;
		schedSecond();
		cin.ignore();
		exit(0);
	}
}

void task::sortTask()
{
	vector<float>::iterator pi, ci, temp;
	float t;
	for (pi = p.begin(); pi != p.end(); ++pi)
	{
		float min = *pi;
		for (temp = pi; temp != p.end(); temp++)
		{
			if (*temp < min)
			{
				t = *pi;
				*pi = *temp;
				*temp = t;
			}
		}
	}
}


// This can be parallelized az the loop is dependant on the loop element in itself
void task::calcTime(float Ti, int m)
{
	int i;
	for (i = 0; i <= m; i++)
	{
		for (int k = 1; k <= floor(Ti / (p[i])); k++)
		{
			timeStore.push_back((float)(k*(p[i])));
		}
	}
	timeStore.push_back(p[i]);
}


void task::schedSecond()
{
	fstream fout;
	fout.open("plot_this.txt", fstream::out);
	//sort - use a heap or a priority queue
	sortTask();
	cout << "Task set sorted" << endl;
	cin.ignore();

	//reference table
	//pi			The task for which the w(t) ias being calculated
	//ci			The execution time of the task in consideration
	//timeStore		This is a vector storing the scheduling points


	//calculate the wi(t) for every task

	//This is the outermost loop. This decides which tak is being currently visited
	//Now the decision has to be made so as to reduce the iteration. This for loop cannot proceed serially.
	float volatile b_time = 0;
	m = (int)(size / 2);
	while (ratio > 1)
	{
		auto *pi = &p[m];
		auto *ci = &c[m];
		//this T is the time of the current viewed task. This will be passed to calcTime
		float T = *pi;
		vector<float>::iterator ti;
		int flag = 0;

		//cout << "Considering task #" << T << endl;
		//cin.ignore();
		//Now this two information (The current time and the iterator pointing to the current task)
		//are required for the calculation of the scheduling points.
		//These scheduling points are valid for the calculation of the time demand function of the task in question
		timeStore.clear();
		calcTime(T, m);

		float volatile chk_time = 0;
		float volatile chk_tm = 0;
		//Now this loop traverses the scheduling points and at every iteration sees whether the condition is satisfied or not
		for (auto time = timeStore.rbegin(); time != timeStore.rend(); ++time)
		{

			value = *ci;
			count++;
			for (int pj = m, cj = m; pj >= 0; --pj, --cj)
			{
				value += (c[cj])*ceil((*time) / (p[pj]));
				if (value > *time)
				{
					cout << "The task set is not scheduable" << endl;
					cout << "Unscheduable task: (" << (*ci) << ", " << (*pi) << ")" << endl;
					cout << "Number of iterations: " << count;
					cin.ignore();
					exit(0);
				}
			}
			chk_time = value;
			chk_tm = *time;
			//cout << "\n\t" << "Value: " << (value) << endl;
		}
		ratio = chk_tm / chk_time;
		m = m + floor(ratio);
		cout << "(" << (*ci) << "," << (*pi) << "): " << chk_time << " | time: " << chk_tm << "| Ratio: " << chk_tm / chk_time;
		//fout << chk_tm / chk_time << endl;
		//cin.ignore();
	}

	cout << "Scheduable Task set" << endl;
	cout << "Number of iterations: " << count;
	cin.ignore();
}

//This function generates random task sets
void gen(float ds)
{
	ofstream fout;
	srand(std::time(0));
	fout.open("task_set.txt");
	fout << (int)ds << endl;
	float chk = (1.0 / ds);
	for (int i = 1; i < (int)ds; i++)
	{
		float x = 0;
		while (x < MAX_PERIOD/2)
			x = abs(rand() + 10)*abs(rand() + 10) % MAX_PERIOD + 2;

		float y = x;
		while ((y / x)*1000 >= chk*1000)
		{
			y--;
		}
		if (y <= 0)
			y = 1;
		fout << y << "," << x << "," << endl;
	}
	fout.close();
}




int main()
{
	gen(100);
	task T;
	T.takeInput();
	//T.getInput();
	T.schedFirst();
	cin.ignore();
	return 0;
}

