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
	int size, count;
public:
	task()
	{
		U = 0.0;
		condU = 0.0;
		value = 0.0;
		count = 0;
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
	timeStore.push_back(*pi);
}

void task::schedSecond()
{
	//sort - use a heap or a priority queue
	sortTask();
	cout << "Task set sorted" << endl;
	cin.ignore();

	//reference table
	//pi			The task for which the w(t) ias being calculated
	//ci			The execution time of the task in consideration
	//timeStore		This is a vector storing the scheduling points


	//calculate the wi(t) for every task
	vector<float>::iterator pi, ci;

	//This is the outermost loop. This decides which tak is being currently visited
	for (pi = p.begin(), ci=c.begin(); pi != p.end(); ++pi, ++ci)
	{
		//this T is the time of the current viewed task. This will be passed to calcTime
		float T = *pi;
		vector<float>::iterator ti;
		int flag = 0;

		//cout << "Considering task #" << T;
		//Now this two information (The current time and the iterator pointing to the current task)
		//are required for the calculation of the scheduling points.
		//These scheduling points are valid for the calculation of the time demand function of the task in question
		timeStore.clear();
		calcTime(T, pi);


		//Now this loop traverses the scheduling points and at every iteration sees whether the condition is satisfied or not
		for (auto time = timeStore.rbegin(); time != timeStore.rend(); ++time)
		{

			value = *ci;
			count++;
			for (auto pj = pi, cj = ci; pj != p.begin(); --pj, --cj)
			{
				value += (*cj)*ceil((*time) / (*pj));
				if (value > *time)
				{
					cout << "The task set is not scheduable" << endl;
					cout << "Unscheduable task: (" << (*pi) << ", " << (*ci) << ")" << endl;
					cout << "Number of iterations: " << count;
					cin.ignore();
					exit(0);
				}
			}
			//cout << "\n\t" << "Value: " << (value) << endl;
		}
	}

	cout << "Scheduable Task set" << endl;
	cin.ignore();
}

int main()
{
	task T;
	T.getInput();
	T.schedFirst();
	cin.ignore();
	return 0;
}

