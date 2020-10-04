#include "simlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <math.h>
#include <time.h>
#include <list>
#include <time.h>
#include <vector>
#include <algorithm>

using namespace std;

#define EVENT_ARRIVAL 1				// Тип события для поступления работы в систему
#define EVENT_DEPARTURE 2			// Тип события для ухода работы с определенного рабочего места. 
#define EVENT_END_SIMULATION 3		// Тип события для завершения моделирования
#define STREAM_INTERARRIVAL 1		// Поток случайных чисел для интервалов времени между поступлениями работ
#define STREAM_DESEASE_TYPE 2		// Поток случайных чисел для типов работ
#define STREAM_SERVICE 3			// Поток случайных чисел для времени обслуживания
#define DESEASE_TYPES 3				// Лёгкое заболевание \ среднее \ тяжёлое
#define SAMPST_DELAYS 1				// переменная функции sampst для задержек в очереди(очередях). 

struct doctor {
	float probToDetermDiagn;
	float probSendToAnother;
	float probOfReTaking;
	vector<int> sendToDoctor;
	vector<float> busy;
	vector<int> queue_length;
};

struct patient {
	int index;
	int deseaseType;
	float incomingTime;
	float departureTime;
	float timeToNext;
	int doctor;
	bool istIll;
	int visits;
};

struct next_event {
	float time;
	int event_type;
};

int min_doctors = 2, max_doctors = 15,
	num_doctors = min_doctors,
	desease_type,
	visit,
	*cabinet_busy;
static int total_patients = 0;
static int depart1 = 0;
static int depart2 = 0;
static float mean_sim_time = 0;
static float buf = 0;
int arrives = 0, departures = 0;
const int num_desease_types = 3;
float mean_interarrival_ = 10.0,
	mean_service[num_desease_types],
	length_doctor_works = 24.0,
	length_simulation = 2.0,
	prob_distrib_desease_type[DESEASE_TYPES],
	buffer[50];

struct patient patient_ = {};
struct doctor* doctors;
struct patient* patients;
vector<struct patient> patients_cured;			// "вылеченные" пациенты 
vector<vector<struct patient>> doctor_queue;	// очередь на сегодня
vector<struct patient> nextDay_doctor_queue;	// очередь на последующие дни
vector<vector<int>> connections;				// перенаправления
vector<struct next_event> event_list;			// список событий
vector<vector<int>> doctor_queue_delays;		// задержки в очереди к доктору
struct patient *patientsInService;

FILE* infile, * outfile;

void arrive(int new_job);
void depart(void);
void report(void);
bool compareEvents(struct next_event event1, struct next_event event2);

bool NextSet(int *a, int n, int m)
{
	int k = m;
	for (int i = k - 1; i >= 0; --i)
		if (a[i] < n - k + i + 1) 
		{
			++a[i];
			for (int j = i + 1; j < k; ++j)
				a[j] = a[j - 1] + 1;
			return true;
		}
	return false;
}

void Print(int *a, int n) 
{
	int i;
	static int num = 1; 
	//printf("\n%d: ",num++);
	std::vector<int> var;
	for (i = 0; i < n; i++){
		//printf("%d ", a[i]);
		// printf(".");
		var.push_back(a[i]);
	}
	connections.push_back(var);
}

void getCnk(int n, int k){
	int* a = new int[n];
	for (int i = 0; i < n; i++)
		a[i] = i+1;
	Print(a, k);
	if(n >= k){
		while (NextSet(a, n, k)){
			Print(a, k);
		}
	}
	for(int i = 0; i < connections.size(); i++){
		printf("\n");		
		for(int j =  0; j < connections[i].size(); j++){
			printf("%d ", connections[i][j]);
		}
	}	
}

long double fact(int N)
{
    if(N < 0) // если пользователь ввел отрицательное число
        return 0; // возвращаем ноль
    if (N == 0) // если пользователь ввел ноль,
        return 1; // возвращаем факториал от нуля - не удивляетесь, но это 1 =)
    else // Во всех остальных случаях
        return N * fact(N - 1); // делаем рекурсию.
}

void main() {
	srand(time(0));

	infile = fopen("in.txt", "r");
	outfile = fopen("out.txt", "w");

	// read info from file
	for (int i = 0; i < num_desease_types; i++)
		fscanf(infile, "%f", &mean_service[i]);
	for (int i = 0; i < num_desease_types; i++)
		fscanf(infile, "%f", &prob_distrib_desease_type[i]);

	fprintf(outfile, "*** Имитационная модель ММО на примере работы поликлиники ***");
	//
	// изменение кол-ва врачей
	//

	// init doctors info
	doctors = new struct doctor[num_doctors];
	for(int i = 0; i < num_doctors; i++){
		doctors[i].probToDetermDiagn = 0.5;
		doctors[i].probOfReTaking = 0.3;
		doctors[i].probSendToAnother = 0.2;
	}
	
	//
	// изменение параметров у врачей (3 цикла)
	//
	for(int Cnk = 2; Cnk <= num_doctors; Cnk++){
		// init doctor connections
		connections.clear();
		getCnk(num_doctors, 2);

		// simulation cicle  connections.size() 0
		for(int i = 0; i < connections.size(); i++){
			// int size = connections[i].size();
			for(int j = 1; j < connections[i].size(); j++) {
				// aprintf("\n%d - i; %d - j; %d - connections[i].size()\n", i, j, 2);
			
				// one simulation cicle

				// setting doctor connections
				if(doctors[connections[i][j-1]-1].sendToDoctor.begin() != doctors[connections[i][j-1]-1].sendToDoctor.end()){
					if(find(doctors[connections[i][j-1]-1].sendToDoctor.begin(),
						doctors[connections[i][j-1]-1].sendToDoctor.end(), connections[i][j]-1) ==
						doctors[connections[i][j-1]-1].sendToDoctor.end()){
							doctors[connections[i][j-1]-1].sendToDoctor.push_back(connections[i][j]-1);
							if(rand()%2==0)
								doctors[connections[i][j]-1].sendToDoctor.push_back(connections[i][j-1]-1);
					}
				} else {
					doctors[connections[i][j-1]-1].sendToDoctor.push_back(connections[i][j]-1);
							if(rand()%2==0)
								doctors[connections[i][j]-1].sendToDoctor.push_back(connections[i][j-1]-1);
				}

				// Записываем заголовок отчета и входные параметры.
				fprintf(outfile, "\n\n*****************************************************");
				fprintf(outfile, "\nМодель поликлиники");
				fprintf(outfile, "\nЧисло врачей %d", num_doctors);
				fprintf(outfile, "\nЧисло типов заболеваний %d",
					num_desease_types);
				fprintf(outfile, "\nСреднее время приёма пациентов \n с данными типами заболеваний		");
				for (int k = 0; k < num_desease_types; k++)
					fprintf(outfile, "%.3f ", mean_service[k]);
				fprintf(outfile, "\nВероятности определения правильного диагноза врачами :\n\t\t");
				for (int k = 0; k < num_doctors; k++)
					fprintf(outfile, "%.3f ", doctors[k].probToDetermDiagn);
				fprintf(outfile, "\nВероятности перенаправить пациента на лечение к другому врачу :\n\t\t");
				for (int k = 0; k < num_doctors; k++)
					fprintf(outfile, "%.3f ", doctors[k].probSendToAnother);
				fprintf(outfile, "\n# врача | к какому врачу может перенаправить\n");
				for (int k = 0; k < num_doctors; k++){
					fprintf(outfile, "\n\t%d\t|", k+1);
					for(int l = 0; l < doctors[k].sendToDoctor.size(); l++)
						fprintf(outfile, "\t%d", doctors[k].sendToDoctor[l]+1);
				}
				fprintf(outfile, "\n\nВероятности повторного приёма :\n\t\t");
				for (int k = 0; k < num_doctors; k++)
					fprintf(outfile, "%.3f ", doctors[k].probOfReTaking);
				fprintf(outfile, "\nСреднее время между поcтуплением пациентов %.2f минут(а|ы)\n\n",
					mean_interarrival_);				
				fprintf(outfile, "Длительность моделируемого периода %.1f %.1f-часовых дней\n\n",
					length_simulation, length_doctor_works);

				// запуск имитации
				sim_time = 0;
				visit = 0;
				desease_type = 0;
				patients_cured.clear();
				doctor_queue.clear();
				event_list.clear();
				nextDay_doctor_queue.clear();

				patientsInService = new struct patient[num_doctors];
				cabinet_busy = new int[num_doctors];
				for(int x = 0; x < num_doctors ; x++){
					cabinet_busy[x] = 0;
					vector<int> vec;
					vector<struct patient> pat;
					vector<struct patient> pat1;
					doctor_queue_delays.push_back(vec);
					doctor_queue.push_back(pat);
				}

				struct next_event first = {0.0, EVENT_ARRIVAL};
				struct next_event last = {60.0 * length_doctor_works * length_simulation, EVENT_END_SIMULATION};
				event_list.push_back(first);
				event_list.push_back(last);

				float zz = 0;
				for(int k = 0; k < 1000000; k++)
					zz += expon(mean_interarrival_, STREAM_INTERARRIVAL);
				printf("%f\n", zz/1000000.0);

				printf("\n** start **\n");
				// printf("%d\n", event_list.size());

				do  {
					sort(event_list.begin(), event_list.end(), compareEvents);
					next_event_type = event_list[0].event_type;
					sim_time = event_list[0].time;
					event_list.erase(event_list.begin());
					bool flag;

					switch (next_event_type) {
					case EVENT_ARRIVAL:
						flag = false;
						for(int k = 0; k < nextDay_doctor_queue.size(); k++)
							if(nextDay_doctor_queue[k].incomingTime == sim_time){
								flag = true;
								patient_ = nextDay_doctor_queue[k];
								k = nextDay_doctor_queue.size();
							}
						if(flag)
							arrive(1);
						else{
							arrives++;
							arrive(0);
						}
						break;
					case EVENT_DEPARTURE:
						departures++;
						depart();
						break;
					case EVENT_END_SIMULATION:
						printf("############");
						report();
						break;
					}
				} while(next_event_type != EVENT_END_SIMULATION);
			}
		}
		if(total_patients != 0)
			printf("%f - mean_sim_time\n", mean_sim_time/(float)total_patients);
	}
	fclose(infile);
	fclose(outfile);

	system("pause");
}

float getRandExp(){
	float res = expon(mean_interarrival_, STREAM_INTERARRIVAL);
	/*
	if(!(res >= 1.0)) 
		return getRandExp();
		*/
	return res;
}

bool compareEvents(struct next_event event1, struct next_event event2){
	return event1.time < event2.time;
}

void arrive(int visit){
	// printf("** arrive ** - %f\n", sim_time);
	int cabinet, desease = -1;

	if(visit == 0){
		total_patients++;
		float time = getRandExp();
		struct next_event arrive = {sim_time + time, EVENT_ARRIVAL};
		mean_sim_time += time;
		event_list.push_back(arrive);

		for(int j = 0; j < DESEASE_TYPES; j++)
			if(((float)(rand()%101)/(float)100.0) <= prob_distrib_desease_type[j])
				desease = j;
		patient_.deseaseType = desease != -1 ? desease : 0;
		patient_.visits = 1;
		patient_.doctor = rand()%num_doctors;
		patient_.incomingTime = sim_time;
		patient_.index = total_patients;

		cabinet = patient_.doctor;
		desease_type = patient_.deseaseType;
	} else {
		int i = 0;
		for(i = 0; i < nextDay_doctor_queue.size(); i++)
			if(nextDay_doctor_queue[i].incomingTime == sim_time)
				break;

		cabinet = patient_.doctor;
		desease_type = patient_.deseaseType;

		nextDay_doctor_queue.erase(nextDay_doctor_queue.begin()+i);
	}

	if(cabinet_busy[cabinet] > 0){
		doctor_queue[cabinet].push_back(patient_);
	} 
	else {
		float delay = 0.0;
		doctor_queue_delays[cabinet].push_back(delay);
		cabinet_busy[cabinet]++;

		patient_.departureTime = sim_time + expon(mean_service[desease_type], STREAM_SERVICE);
		struct next_event departure = {patient_.departureTime, EVENT_DEPARTURE};

		patientsInService[cabinet] = patient_;
		event_list.push_back(departure);
	}
}

void servise(int cabinet){
	// Определяем диагноз и то, выздоровеет ли пациент вследствие этого диагноза
	if(patientsInService[cabinet].istIll != NULL)
		patientsInService[cabinet].istIll = patientsInService[cabinet].istIll  &&
			( ((float)(rand()%101)/(float)100.0) <= doctors[cabinet].probToDetermDiagn);
	else
		patientsInService[cabinet].istIll = ( ((float)(rand()%101)/(float)100.0) <= doctors[cabinet].probToDetermDiagn);


	if(patientsInService[cabinet].visits <= 5){
		// повторный приём у врача 
		if(( ((float)(rand()%101)/(float)100.0) <= doctors[cabinet].probOfReTaking)){
			float time = getRandExp();
			float t = sim_time + time + (float)((rand()%1+1) * 24.0 * 60);
			mean_sim_time += time;

			patientsInService[cabinet].incomingTime = t;
			patientsInService[cabinet].doctor = cabinet;
			patientsInService[cabinet].visits += 1;
			//patientsInService[cabinet].departureTime = 
			//	patientsInService[cabinet].incomingTime + expon(mean_service[desease_type], STREAM_SERVICE);

			nextDay_doctor_queue.push_back(patientsInService[cabinet]);

			struct next_event arrive = {t, EVENT_ARRIVAL};
			//struct next_event departure = {patientsInService[cabinet].departureTime, EVENT_DEPARTURE};
			event_list.push_back(arrive);
			//event_list.push_back(departure);
		}
		else if(( ((float)(rand()%101)/(float)100.0) <= doctors[cabinet].probSendToAnother)){
			float time = getRandExp();
			float t = sim_time + time + (float)((rand()%1+1) * 24.0 * 60);
			mean_sim_time += time;
			patientsInService[cabinet].incomingTime = t;
			if(doctors[cabinet].sendToDoctor.size() != 0)
				patientsInService[cabinet].doctor = doctors[cabinet].sendToDoctor[(rand() % doctors[cabinet].sendToDoctor.size())];
			patientsInService[cabinet].visits += 1;
			//patientsInService[cabinet].departureTime = 
			//	patientsInService[cabinet].incomingTime + expon(mean_service[desease_type], STREAM_SERVICE);

			nextDay_doctor_queue.push_back(patientsInService[cabinet]);
			struct next_event arrive = {t, EVENT_ARRIVAL};
			//struct next_event departure = {patientsInService[cabinet].departureTime, EVENT_DEPARTURE};
			event_list.push_back(arrive);
			//event_list.push_back(departure);
		}
		else patients_cured.push_back(patientsInService[cabinet]);
	} else patients_cured.push_back(patientsInService[cabinet]);

	struct patient pat = {};
	patientsInService[cabinet] = pat;
}

void depart(void){
	// printf("** depart ** - %f\n", sim_time);
	int cabinet = -1, desease_type = -1, vst = -1;
	float last_depart;
	
	for(int k = 0; k < num_doctors; k++)
		if(patientsInService[k].departureTime == sim_time){
			desease_type = patientsInService[k].deseaseType;
			vst = patientsInService[k].visits;
			cabinet = patientsInService[k].doctor;
		}

	if(cabinet == -1 || desease_type == -1 || vst == -1)
		printf("-1...\n");

	doctors[cabinet].busy.push_back(patientsInService[cabinet].departureTime - patientsInService[cabinet].incomingTime);		// busy
	last_depart = patientsInService[cabinet].departureTime;			 // for delay detecting
	
	if(patientsInService[cabinet].visits > 0 && patientsInService[cabinet].departureTime == sim_time){
		servise(cabinet);
	}

	if (doctor_queue[cabinet].size() == 0) {
		depart1++;
		cabinet_busy[cabinet]--;
	}
	else{
		depart2++;

		float delay = last_depart - doctor_queue[cabinet][0].incomingTime;
		doctor_queue_delays[cabinet].push_back(delay);
		doctors[cabinet].queue_length.push_back(doctor_queue[cabinet].size());

		float time = sim_time + expon(mean_service[desease_type], STREAM_SERVICE);
		struct next_event departure = {time, EVENT_DEPARTURE};
		event_list.push_back(departure);

		doctor_queue[cabinet][0].incomingTime = sim_time;
		doctor_queue[cabinet][0].departureTime = time;

		patientsInService[cabinet] = doctor_queue[cabinet][0];
		doctor_queue[cabinet].erase(doctor_queue[cabinet].begin());
	}
}

void report(void){
	float avg_num_in_queue = 0, avg_queue_delays = 0, avg_doctor_queue_delays = 0,
		avg_visits = 0, avg_cured = 0, avg_doctors_busy = 0, avg = 0;

	printf("\n%d - arrives; %d - departures\n", arrives, departures);

	fprintf(outfile, "С %d врачами, среднее число пациентов в очереди :\n", num_doctors);
	for(int cabinet = 0; cabinet < num_doctors; cabinet++) {			// среднее число пациентов в очереди ++
		avg = 0.0;
		avg_num_in_queue = 0.0;
		if(doctors[cabinet].queue_length.size() != 0){
			for(int i = 0; i < doctors[cabinet].queue_length.size(); i++)
				avg += doctors[cabinet].queue_length[i];
			avg_num_in_queue += avg/(float)doctors[cabinet].queue_length.size();
		}
		fprintf(outfile, "%.2f\t", avg_num_in_queue);
	}


	fprintf(outfile, "\n\nСреднее время занятости врачей: ");				// Процент занятости врачей
	for(int x = 0; x < num_doctors; x++){
		avg = 0.0;
		if(doctors[x].busy.size() != 0){
			for(int y = 0; y < doctors[x].busy.size(); y++)
				avg += doctors[x].busy[y];
			avg_doctors_busy += avg/(float)doctors[x].busy.size();
			fprintf(outfile, "%.3f\t", avg/(float)doctors[x].busy.size());
		} else
			fprintf(outfile, "%.3f\t", avg);
	}

	fprintf(outfile, "\n\nОбщее время занятости врачей: ");		// Общее время занятости врачей
	fprintf(outfile, "%.3f", avg_doctors_busy/num_doctors);


	fprintf(outfile, "\n\nЗадержки в очереди, мин:\n");				// Задержки в очереди ++
	for(int x = 0; x < doctor_queue_delays.size(); x++){
		avg = 0.0;
		if(doctor_queue_delays[x].size() != 0)
			for(int y = 0; y < doctor_queue_delays[x].size(); y++)
				avg += doctor_queue_delays[x][y];
		fprintf(outfile, "%.3f\t", avg/(float)doctor_queue_delays[x].size());
		avg_doctor_queue_delays += avg/(float)doctor_queue_delays[x].size();
	} 
	fprintf(outfile, "\n\nОбщая задержка в очереди, мин: ");		// Общая задержка в очереди ++
	fprintf(outfile, "%.3f", avg_doctor_queue_delays/(float)num_doctors);


	fprintf(outfile, "\n\nОбщее кол-во пациентов: %d", total_patients);				// Общее кол-во пациентов
	fprintf(outfile, "\n\nКол-во пациентов, которые выписались из поликлиники: %d",
		patients_cured.size());								// Кол-во пациентов, которые посетили поликлинику
	int patients_in_clinic = 0;
	for(int x = 0; x < num_doctors; x++)
		patients_in_clinic += doctor_queue[x].size();
	// Кол-во пациентов, которые находятся на лечении
	int additional = 0;
	for(int k = 0; k < num_doctors; k++)
		if(patientsInService[k].visits > 0)
			additional++;
	fprintf(outfile, "\n\nКол-во пациентов, которые находятся на лечении: %d", nextDay_doctor_queue.size() + patients_in_clinic + additional);	


	// *****************************
	avg_visits = 0;
	fprintf(outfile, "\n\nСреднее кол-во посещений (выписавшихся): ");		// Среднее кол-во посещений 
	if(patients_cured.size() != 0){												// ('выписавшихся')
		for(int x = 0; x < patients_cured.size(); x++){
			avg_visits += patients_cured[x].visits;
			// printf("%d - patients_cured[%d].visits\n", patients_cured[x].visits, x);
		}
		fprintf(outfile, "%.2f", avg_visits/(float)patients_cured.size());
	} else fprintf(outfile, "%f", avg_visits);

	fprintf(outfile, "\n\nСреднее кол-во посещений (в очереди): ");		// Среднее кол-во посещений 
	for(int x = 0; x < num_doctors; x++){								// (в очереди)
		avg_visits = 0;
		for(int y = 0; y < doctor_queue[x].size(); y++)
			avg_visits += doctor_queue[x][y].visits;
		fprintf(outfile, "%.2f\t", avg_visits/(float)doctor_queue[x].size());
	}

	avg_visits = 0;
	fprintf(outfile, "\n\nСреднее кол-во посещений (назначен повторный приём): ");	// Среднее кол-во посещений 
	for(int x = 0; x < nextDay_doctor_queue.size(); x++)							// (назначен повторный приём)
		avg_visits += nextDay_doctor_queue[x].visits;	
	fprintf(outfile, "%.2f", avg_visits/(float)nextDay_doctor_queue.size());


	fprintf(outfile, "\n\nПроцент пациентов, которые вылечились: ");	// Процент пациентов, которые вылечились
	for(int x = 0; x < patients_cured.size(); x++)
		if(patients_cured[x].istIll == true)
			avg_cured++; 
	fprintf(outfile, "%.2f %", (avg_cured/(float)patients_cured.size())*100.0);


	// *****************************
	int des1 = 0, des2 = 0, des3 = 0;
	fprintf(outfile, "\n\nПроцентное соотношение болезней (выписавшихся): ");	// Процентное соотношение болезней
	for(int x = 0; x < patients_cured.size(); x++){							// (выписавшихся)
		if(patients_cured[x].deseaseType == 0)
			des1++; 
		else if(patients_cured[x].deseaseType == 1)
			des2++; 
		else if(patients_cured[x].deseaseType == 2)
			des3++; 
	}
	fprintf(outfile, "[1] - %.2f%, [2] - %.2f%, [3] - %.2f%,", (des1/(float)patients_cured.size())*100.0,
		(des2/(float)patients_cured.size())*100.0,(des3/(float)patients_cured.size())*100.0);

	des1 = 0, des2 = 0, des3 = 0;
	fprintf(outfile, "\n\nПроцентное соотношение болезней (назначен повторный приём): ");	// Процентное соотношение болезней
	for(int x = 0; x < nextDay_doctor_queue.size(); x++){								// назначен повторный приём
		if(nextDay_doctor_queue[x].deseaseType == 0)
			des1++; 
		else if(nextDay_doctor_queue[x].deseaseType == 1)
			des2++; 
		else if(nextDay_doctor_queue[x].deseaseType == 2)
			des3++; 
	}
	fprintf(outfile, "[1] - %.2f%, [2] - %.2f%, [3] - %.2f%,", (des1/(float)nextDay_doctor_queue.size())*100.0,
		(des2/(float)nextDay_doctor_queue.size())*100.0,(des3/(float)nextDay_doctor_queue.size())*100.0);

	des1 = 0, des2 = 0, des3 = 0;
	int total = 0;
	fprintf(outfile, "\n\nПроцентное соотношение болезней (в очереди): ");	// Процентное соотношение болезней
	for(int x = 0; x < num_doctors; x++){									// (в очереди)
		for(int y = 0; y < doctor_queue[x].size(); y++){
			if(doctor_queue[x][y].deseaseType == 0)
				des1++; 
			else if(doctor_queue[x][y].deseaseType == 1)
				des2++; 
			else if(doctor_queue[x][y].deseaseType == 2)
				des3++; 
		}
		total += doctor_queue[x].size();
	}
	fprintf(outfile, "[1] - %.2f%, [2] - %.2f%, [3] - %.2f%,", (des1/(float)total)*100.0,
		(des2/(float)total)*100.0,(des3/(float)total)*100.0);

}

//