#include "stdafx.h"
#include<iostream>
#include<vector>
#include<cmath>
#include<queue>
using namespace std;


class task
{
	vector<float> c, p, t, wt, timeStore;
	float U, condU;
	double value;
	int size;
public:
	task()
	{
		U = 0.0;
		condU = 0.0;
		value = 0.0;
	}
	void calcTime(float Ti, vector<float>::iterator j);
	void getInput();
	void summ();
	void schedFirst();
	void schedSecond();
	void sortTask();
};

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
	for (pi = p.begin(), ci = c.begin(), ti = t.begin(); pi != p.end(); ++pi, ++ci, ++ti)
	{
		U = U + ((*ci) / (*pi));
	}
	cout << U << endl;
	condU = size * (pow(2.0, (1.0 / size)) - 1.0);
	cout << condU << endl;
	if (U <= condU)
	{
		cout << "The current taskset is scheduable" << endl;
		exit(0);
	}
	else
		cout << "The current taskset might not be scheduable" << endl;

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

void task::calcTime(float Ti, vector<float>::iterator j)
{
	vector<float>::iterator pi;
	for (pi = p.begin(); pi != j; ++pi)
	{
		for (int i = 1; i <= floor(Ti / (*pi)); i++)
		{
			timeStore.push_back((float)(i*(*pi)));
		}
	}
}

void task::schedSecond()
{
	//sort - use a heap or a priority queue
	sortTask();

	//reference table
	//pi			The task for which the w(t) ias being calculated
	//ci			The execution time of the task in consideration
	//timeStore		This is a vector storing the scheduling points


	//calculate the wi(t) for every task
	vector<float>::iterator pi, ci;
	//This is the outermost loop. This decides which tak is being currently visited
	for (pi = p.begin(), ci=c.begin(); pi != p.end(); ++pi, ++ci)
	{
		float T = *pi;
		vector<float>::iterator ti;
		int flag = 0;

		//Now this two information are required for the calculation of the scheduling points.
		//These scheduling points are valid for the calculation of the time demand function of the task in question
		calcTime(T, pi);

		//Now this loop traverses the scheduling points and at every iteration sees whether the condition is satisfied or not
		for (vector<float>::iterator time = timeStore.begin(); time != timeStore.end(); ++time)
		{
			vector<float>::iterator cj, pj;
			value = 0.0;
			for (pj = p.begin(), cj = c.begin(); pj != pi; ++pj, ++cj)
			{
				value += (*cj)*ceil((*time) / (*pi));
			}
			if (value > *time)
			{
				cout << "Unscheduable task: (" << (*pi) << ", " << (*ci) << ")" << endl;
				exit(0);
			}
		}
	}
}

int main()
{
	task T;
	T.getInput();
	T.schedFirst();
	return 0;
}

