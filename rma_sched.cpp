#include "stdafx.h"
#include<iostream>
#include<vector>
#include<cmath>
#include<queue>
#include<sstream>
#include<fstream>
#include<ctime>
#include<map>
#define MAX_PERIOD 1000
#define BUFF_SIZE 10
#define NO_OF_PROCESSORS 4
#define NO_OF_TASKS 200
using namespace std;


class task
{
	vector<float> *c, *p, t, wt, *timeStore, tempC, tempP; //tempC and tempP for taking input, rest are for computation
	float condU;
	long double U;
	double value, ratio;
	void calcTime(float Ti, int m, int processor);
	void summ();
	void schedSecond(int processor);
	void sortTask(int processor);

public:
	void schedFirst(int processor);
	int size, count, m;
	void divide();
	void takeInput();
	void getInput();
	void gen(float ds);
	void clear()
	{
		U = 0.0;
		condU = 0.0;
		value = 0.0;
		count = 0;
		ratio = 9999999.0;
		delete(c);
		delete(p);
		tempP.clear();
		tempC.clear();
		delete(timeStore);
		c = new vector<float>[NO_OF_PROCESSORS];
		p = new vector<float>[NO_OF_PROCESSORS];
		timeStore = new vector<float>[NO_OF_PROCESSORS];
	}
	task()
	{
		U = 0.0;
		condU = 0.0;
		value = 0.0;
		count = 0;
		ratio = 9999999.0;
		c = new vector<float>[NO_OF_PROCESSORS];
		p = new vector<float>[NO_OF_PROCESSORS];
		timeStore = new vector<float>[NO_OF_PROCESSORS];
	}
	
	void debug()
	{
		takeInput();
		divide();
	}
	
};

void startITDA()
{
	fstream fout;
	string filename = "output" + to_string(NO_OF_PROCESSORS) + ".txt";
	fout.open(filename, fstream::out);
	task *T;
	for (int j = 10; j <= NO_OF_TASKS; j += 10)
	{
		T = new task();
		//divide
		(*T).gen(j);
		(*T).takeInput();
		(*T).divide();
		//call schedFirst on each
		(*T).count = 0;
		for (int i = 0; i < NO_OF_PROCESSORS; i++)
		{
			(*T).schedFirst(i);
		}
		cout << "\n\n-------------------\n Total iterations for " << j << ": " << (*T).count << "\n\n";
		fout << j << "," << (*T).count << endl;
		free(T);
	}
	cin.ignore();
	fout.close();
	exit(0);
}

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
	cout << "Reading task-set of size: " << size << "\n-----------------------------" << endl;
	for (int i = 0; i<size; i++)
	{
		fin.getline(buff1, BUFF_SIZE);
		stringstream sts(buff1);

		sts.getline(buff2, 10, ',');
		u = atoi(buff2);
		tempC.push_back(u);

		sts.getline(buff2, 10, ',');
		u = atoi(buff2);
		tempP.push_back(u);
	}
	cout << "Completed reading task-set\n\n";

	tempP.pop_back();
	tempC.pop_back();
	size = tempP.size();
}


/*
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
*/


//start divinding the task set into NO_OF_PROCESSORS
//on each call schedFirst, sortTask, calcTime, schedSecond

void task::divide()
{
	vector<double> p_exec(NO_OF_PROCESSORS, 0.0);
	int ele = 0;
	double tempU = 0.0;
	//work on tempP and allocate one by one
	for (auto i = tempP.begin(), j = tempC.begin(); i != tempP.end(); ++i, ++j)
	{
		tempU = *j / *i;
		ele = distance(begin(p_exec), (min_element(begin(p_exec), end(p_exec)))); //finds the least loaded processor
		p[ele].push_back(*i);
		c[ele].push_back(*j);
		p_exec[ele] += tempU;
		
	}

	//Uncomment to view which tasks are being allocated to which processor
	/*
	for (int i = 0; i < NO_OF_PROCESSORS;i++)
	{
		for (auto it : p[i])
		{
			cout << it << "  ";
		}
		cout << "-------" << p_exec[i];
		cout << endl;
	}
	*/
}
//After divide, modify the other functions so that they work on the input that they take as parameter.
//This can be achieved by giving them the input only what processor they are required to operate on. 



void task::schedFirst(int processor)
{
	vector<float>::iterator pi, ci, ti;
	U = 0.0;
	for (pi = p[processor].begin(), ci = c[processor].begin(); pi != p[processor].end(); ++pi, ++ci)
	{
		U = U + ( (*ci) / (*pi) );
	}
	//cout << "Scheduability bound\n---------------------------\n";
	//cout << "Calculated Utilization: " << U << endl;
	//condU = size * (pow(2.0, (1.0 / size)) - 1.0);
	//cout << "Limiting Utilization: " << condU << endl;
	//cin.ignore();
	if (U <= condU)
	{
		cout << "The current taskset is scheduable at processor#" << processor << endl;
		//cin.ignore();
		
	}
	else
	{
		//cout << "The current taskset might/might not be scheduable.\n" << endl;
		schedSecond(processor);
		//cin.ignore();
	}
}

void task::sortTask(int processor)
{
	vector<float>::iterator pi, ci, temp;
	float t;
	for (pi = p[processor].begin(); pi != p[processor].end(); ++pi)
	{
		float min = *pi;
		for (temp = pi; temp != p[processor].end(); temp++)
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
void task::calcTime(float Ti, int m, int processor)
{
	int i;
	for (i = 0; i <= m; i++)
	{
		for (int k = 1; k <= floor(Ti / (p[processor][i])); k++)
		{
			timeStore[processor].push_back((float)(k*(p[processor][i])));
		}
	}
	timeStore[processor].push_back(p[processor][i]);
}


void task::schedSecond(int processor)
{
	cout << "Time Demand Analysis for processor#" << processor << "\n---------------------------\n";
	fstream fout;
	fout.open("plot_this.txt", fstream::out);
	//sort - use a heap or a priority queue
	sortTask(processor);
	cout << "Task set sorted" << endl;
	//cin.ignore();

	//reference table
	//pi			The task for which the w(t) ias being calculated
	//ci			The execution time of the task in consideration
	//timeStore		This is a vector storing the scheduling points


	//calculate the wi(t) for every task

	//This is the outermost loop. This decides which tak is being currently visited
	//Now the decision has to be made so as to reduce the iteration. This for loop cannot proceed serially.
	float volatile b_time = 0;
	int size = p[processor].size();
	m = ((int)(size / 2)) % (size-1);
	int o_count = 0;
	int flag = 1;
	double max_ratio = 0.0;
	while (ratio > 1 && o_count < size)
	{
		o_count++;
		auto *pi = &(p[processor][m]);
		auto *ci = &(c[processor][m]);
		//this T is the time of the current viewed task. This will be passed to calcTime
		float T = *pi;
		vector<float>::iterator ti;
		int flag = 1;

		//cout << "Considering task #" << T << endl;
		//cin.ignore();
		//Now this two information (The current time and the iterator pointing to the current task)
		//are required for the calculation of the scheduling points.
		//These scheduling points are valid for the calculation of the time demand function of the task in question
		timeStore[processor].clear();
		calcTime(T, m, processor);

		float volatile chk_time = 0;
		float volatile chk_tm = 0;

		//Now this loop traverses the scheduling points and at every iteration sees whether the condition is satisfied or not
		for (auto time = timeStore[processor].rbegin(); time != timeStore[processor].rend(); ++time)
		{
			value = *ci;
			count++;
			for (int pj = m, cj = m; pj >= 0; --pj, --cj)
			{
				if (flag)
				{
					max_ratio = (c[processor][0])*ceil((*time) / (p[processor][0])) / *time;
					max_ratio = 1/max_ratio;
					//cout << "Max ratio: " << max_ratio;
					flag = 0;
				}
				value += (c[processor][cj])*ceil((*time) / (p[processor][pj]));
				if (value > *time)
				{
					cout << "The task set is not scheduable at processor#" << processor << endl;
					cout << "Unscheduable task: (" << (*ci) << ", " << (*pi) << ")" << endl;
					//cout << "Number of iterations: " << count;
					return;
				}
			}
			chk_time = value;
			chk_tm = *time;
			//cout << "\n\t" << "Value: " << (value) << endl;
		}
		ratio = chk_tm / chk_time;
		//Calculates the new value that is the next task in the sorted list.
		m = m + floor(ratio*size/max_ratio);
		//cout <<"| Ratio: " << chk_tm / chk_time << "| m: " << m;
		//fout << chk_tm / chk_time << endl;
	}

	//cout << "Scheduable Task set" << endl;
	//cout << "Number of iterations: " << count;
}


//This function generates random task sets
void task::gen(float ds)
{
	ofstream fout;
	srand(std::time(0));
	fout.open("task_set.txt");
	fout << (int)ds << endl;
	float chk = (1.0*NO_OF_PROCESSORS / ds);
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
	task T;
	//T.debug();
	startITDA();
	cin.ignore();
	return 0;
}

