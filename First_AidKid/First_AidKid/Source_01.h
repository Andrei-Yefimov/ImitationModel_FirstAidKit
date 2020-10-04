//
//
//#include "simlib.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <list>
//#include <vector>
//
//using namespace std;
//
//#define EVENT_ARRIVAL 1				// Тип события для поступления работы в систему
//#define EVENT_DEPARTURE 2			// Тип события для ухода работы с определенного рабочего места. 
//#define EVENT_END_SIMULATION 3		// Тип события для завершения моделирования
//#define STREAM_INTERARRIVAL 1		// Поток случайных чисел для интервалов времени между поступлениями работ
//#define STREAM_DESEASE_TYPE 2		// Поток случайных чисел для типов работ
//#define STREAM_SERVICE 3			// Поток случайных чисел для времени обслуживания
//#define STREAM_DETERM_DIAGNOS 4		// Поток сучайных чисел для определения диагноза
//#define STREAM_RETACKING 5			// Поток случайных чисел для определения повторного визита
//#define STREAM_SENDING 6			// Поток случайных чисел для перенаправления к другому врачу
//#define DESEASE_TYPES 3				// Лёгкое заболевание \ среднее \ тяжёлое
//#define SAMPST_DELAYS 1				// переменная функции sampst для задержек в очереди(очередях). 
//
//struct doctor {
//	float probToDetermDiagn;
//	float probSendToAnother;
//	float probOfReTaking;
//	std::vector<int> sentToDoctor;
//};
//
//struct patient {
//	float diseaseType;
//	float departureTime;
//	float timeToNext;
//	bool isIll;
//	int visits;
//};
//
//int i, j, x, y, z,
//	min_doctors = 3, max_doctors = 15,
//	num_doctors = min_doctors,
//	desease_type,
//	visit,
//	*cabinet_busy,
//	*num_doctors_busy,
//	*route;
//const int num_desease_types = 3;
// static int num = 0;
//float mean_interarrival = 1.0,
//	mean_service[num_desease_types],
//	length_doctor_works = 12.0,
//	length_simulation = 365.0,
//	prob_distrib_desease_type[26];
//
//struct doctor* doctors;
//struct patient* patients;
//vector<struct patient> queue;
//vector<vector<int>> connections;
//
//FILE* infile, * outfile;
//
//void arrive(int new_job);
//void depart(void);
// void report(void);
//bool isTotal(int count);
//bool isTotalDeterming(int count);
//bool isTotalReTacking(int count);
//bool isTotalSending(int count);
//void getCnk(int n, int k);
//
//int main(){
//	infile = fopen("in.txt", "r");
//	outfile = fopen("out.txt", "a");
//
//	fprintf(outfile, "Модель поликлиники\n\n");
//
//	for (i = 1; i <= num_desease_types; ++i)
//		fscanf(infile, "%f", &prob_distrib_desease_type[i]);
//	for (i = 1; i <= num_desease_types; ++i)
//		fscanf(infile, "%f", &mean_service[i]);
//
//	/*
//	for(num_doctors = min_doctors; num_doctors < max_doctors; num_doctors++){
//		doctors = new struct doctor[num_doctors];
//		cabinet_busy = new int[num_doctors];
//
//		for(i = 0; i < num_doctors; i++){
//			doctors[i].probToDetermDiagn = 0.5;
//			doctors[i].probOfReTaking = 0.3;
//			doctors[i].probSendToAnother = 0.2;
//		}
//
//		while(!isTotal(num_doctors)){
//			while(!isTotalSending(num_doctors)){
//				int item = rand()%num_doctors;
//				addProbability(item, doctors[i].probSendToAnother, 0.5);
//
//				while(!isTotalReTacking(num_doctors)){
//					int item = rand()%num_doctors;
//					addProbability(item, doctors[i].probOfReTaking, 0.5);
//
//					while(!isTotalDeterming(num_doctors)){
//						int item = rand()%num_doctors;
//						addProbability(item, doctors[i].probToDetermDiagn, 1.00);
//
//						for(x = 1; num_doctors >= x; x++){
//							getCnk(num_doctors, x);
//							
//							for(i = 0; i < connections.size(); i++){
//								for(j =  0; j < connections[i].size()-1; j++){
//									if(x != 1)
//										doctors[connections[i][j]].sentToDoctor.push_back(connections[i][j+1]);
//								}
//
//								 -----
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	*/
//
//	num_doctors = 3;
//	doctors = new struct doctor[num_doctors];
//	cabinet_busy = new int[num_doctors];
//
//	for(i = 0; i < num_doctors; i++){
//		doctors[i].probToDetermDiagn = 0.5;
//		doctors[i].probOfReTaking = 0.3;
//		doctors[i].probSendToAnother = 0.2;
//	}
//	 Залисываем заголовок отчета и входные параметры.
//	fprintf(outfile, "\n\n*****************************************************");
//	fprintf(outfile, "\nМодель поликлиники");
//	fprintf(outfile, "\nЧисло врачей %d", num_doctors);
//	fprintf(outfile, "\nЧисло типов заболеваний %d",
//		num_desease_types);
//	fprintf(outfile, "\nФункция распределения типов заболеваний		");
//	for (i = 0; i < num_desease_types; i++)
//		fprintf(outfile, "%.3f ", prob_distrib_desease_type[i]);
//	fprintf(outfile, "\nСреднее время приёма пациентов \n с данными типами заболеваний		");
//	for (i = 0; i < num_desease_types; i++)
//		fprintf(outfile, "%.3f ", mean_service[i]);
//	fprintf(outfile, "\nВероятности определения правильного диагноза врачами :\n\t\t");
//	for (i = 0; i < num_doctors; i++)
//		fprintf(outfile, "%.3f ", doctors[i].probToDetermDiagn);
//	fprintf(outfile, "\nВероятности перенаправить пациента на лечение к другому врачу :\n\t\t");
//	for (i = 0; i < num_doctors; i++)
//		fprintf(outfile, "%.3f ", doctors[i].probSendToAnother);
//	fprintf(outfile, "\nВероятности повторного приёма :\n\t\t");
//	for (i = 0; i < num_doctors; i++)
//		fprintf(outfile, "%.3f ", doctors[i].probOfReTaking);
//	fprintf(outfile, "\nСреднее время между потуплением пациентов %.2f час\n\n",
//		mean_interarrival);				
//	fprintf(outfile, "Длительность моделируемого периода %.1f %.1f-часовых дней\n\n",
//		length_simulation, length_doctor_works);
//
//
//	 подготовка к симуляции
//
//	 Изначально устанавливаем всех 
//	 врачей в состояние незанятости. 
//	num_doctors_busy = new int[num_doctors];
//	cabinet_busy = new int[num_doctors];
//	for (j = 0; j <= num_doctors; j++){
//		num_doctors_busy[j] = 0;
//		cabinet_busy[j] = 0;
//	}
//
//	init_simlib();
//	maxatr = 4;
//	event_schedule(expon(mean_interarrival, STREAM_INTERARRIVAL), EVENT_ARRIVAL);
//
//	 Планируем завершение моделирования. 
//	 (Необходимо сделать поправку на единицы времени.) 
//	event_schedule(length_doctor_works * length_simulation, EVENT_END_SIMULATION);
//
//	queue.clear();
//
//	do {
//		timing();
//		switch (next_event_type) {
//		case EVENT_ARRIVAL:
//			arrive(1);
//			break;
//		case EVENT_DEPARTURE:
//			depart();
//			break;
//		case EVENT_END_SIMULATION:
//			 report();
//			break;
//		}
//	} while (next_event_type != EVENT_END_SIMULATION);
//
//	fclose(infile);
//	fclose(outfile);
//
//	system("pause");
//	return 0;
//}
//
//bool isTotal(int count){
//	int k;
//	return true && isTotalDeterming(count) && isTotalReTacking(count) && isTotalSending(count);
//}
//
//bool isTotalDeterming(int count){
//	int k;
//	bool result = true;
//	for(k = 0; k < count; k++)
//		result = result && (doctors[k].probToDetermDiagn == 1.00);
//	return result;
//}
//
//bool isTotalReTacking(int count){
//	int k;
//	bool result = true;
//	for(k = 0; k < count; k++)
//		result = result && (doctors[k].probOfReTaking == 0.5);
//	return result;
//}
//
//bool isTotalSending(int count){
//	int k;
//	bool result = true;
//	for(k = 0; k < count; k++)
//		result = result && (doctors[k].probSendToAnother == 0.5);
//	return result;
//}
//
//void addProbability(int item, float field, float maxFieldValue){
//	if(field != maxFieldValue)
//		field += 0.01;
//	else {
//		int item = rand()%num_doctors;
//		addProbability(item, field, maxFieldValue);
//	}
//}
//
//
///*	Функция, обслуживайщая событие поступления новой работы
//	в систему, а также поступление работы на очередное рабочее
//	место ее маршрута, не являющееся событием	*/
//void arrive(int new_patient) {
//	int cabinet;
//	/*	Если это поступление новой работы в систему,
//		генерируем время следующего поступления и определяем тип,
//		а также номер задания поступившей работы	*/
//	if (new_patient == 1) {
//		float time = sim_time + expon(mean_interarrival, STREAM_INTERARRIVAL);
//		event_schedule(time, EVENT_ARRIVAL);
//		desease_type = random_integer(prob_distrib_desease_type, STREAM_DESEASE_TYPE);
//		visit = 1;
//		struct patient new_patient = {desease_type, 0, 0, true, visit};
//		queue.push_back(new_patient);
//	}
//	// Определяем рабочее место на маршруте работы
//	cabinet = rand() % num_doctors;
//
//	// Проверяем, есть ли на этой рабочем месте свободные станки
//	if (cabinet_busy[cabinet] > 0) {
//		/*	Все станки на рабочем месте заняты, поэтому помещаем
//			поступившую работу в конец соответствующей очереди.
//			Обратите внимание, что в записи для каждой работы
//			сохраняштся следующие данные:
//			1) время поступления на данное рабочее место:
//			2) тип работы;
//			3) номер текущего задания.	*/
//		transfer[1] = sim_time;
//		transfer[2] = desease_type;
//		transfer[3] = visit;
//		list_file(LAST, cabinet);
//	}
//	else {
//		/*	Ha этом рабочем месте есть свободный станок,
//			начинаем обслуживание поступившей работы
//			(задержка которой в очереди равна 0).	*/
//		sampst(0.0, cabinet);						// Для доктора
//		sampst(0.0, num_doctors + desease_type);	// Для типа болезни
//		++cabinet_busy[cabinet];
//		timest((float)num_doctors_busy[cabinet], cabinet);
//
//		/*	Планируем завершение обслуживания.
//			Обратите внимание, что два последних атрибута записи
//			события определяются перед вызовом функции event_schedu1e.	*/
//		transfer[3] = desease_type;
//		transfer[4] = visit;
//		transfer[5] = cabinet;
//		event_schedule(sim_time + erlang(2, mean_service[desease_type],
//			STREAM_SERVICE), EVENT_DEPARTURE);
//	}
//}
//
//void depart(void) {
//	int cabinet, desease_type_queue, visit_queue;
//
//	// Определяем рабочее место, которое покидает работа
//	desease_type = transfer[3];
//	visit = transfer[4];
//	cabinet = transfer[5];
//
//	// Проверяем, если ещё работы в очереди на этом рабочем месте
//	if (list_size[cabinet] == 0) {
//		// В очереди на рабочем несте больше нет работ, 
//		// переводим станок на атом рабочем месте в состояние незанятости. 
//		--cabinet_busy[cabinet];
//		timest((float)cabinet_busy[cabinet], cabinet);
//	}
//	else {
//		// В очереди на рабочей месте есть работы,
//		// начинаем обслуживание первой работы в очереди;
//		list_remove(FIRST, cabinet);
//
//		// Вычисляем задержку работы для этого рабочего места
//		sampst(sim_time - transfer[1], cabinet);
//
//		//  Вычисляем ту же задержку для этого типа работы
//		desease_type_queue = transfer[2];
//		visit_queue = transfer[3];
//		sampst(sim_time - transfer[1], num_doctors + desease_type_queue);
//
//		/*	Планируем завершение обслуживания этой работы
//			на данном рабочем месте. Обратите внимание,
//			что два последних атрибута записи события определяются
//			перед вызовом функции event_schedule	*/
//		transfer[3] = desease_type_queue;
//		transfer[4] = visit_queue;
//		float simErl = sim_time + erlang(2, mean_service[desease_type_queue], STREAM_SERVICE);
//		event_schedule(simErl, EVENT_DEPARTURE);
//	}
//	// Если у работы, покинувшей рабочее местоб есть ещё невыполненные
//	// задания, отправляем работ на след. рабочее место её маршрута
//	if (true) { // task < num_tasks[job_type]
//		++visit;
//		arrive(2);
//	}
//}
//
//bool NextSet(int *a, int n, int m)
//{
//	int k = m;
//	for (int i = k - 1; i >= 0; --i)
//		if (a[i] < n - k + i + 1) 
//		{
//			++a[i];
//			for (int j = i + 1; j < k; ++j)
//				a[j] = a[j - 1] + 1;
//			return true;
//		}
//	return false;
//}
//
//void Print(int *a, int n) 
//{
//	int i;
//	static int num = 1; 
//	printf("\n%d: ",num++);
//	vector<int> var;
//	for (i = 0; i < n; i++){
//		printf("%d ", a[i]);
//		// printf(".");
//		var.push_back(a[i]);
//	}
//	connections.push_back(var);
//}
//
//void getCnk(int n, int k){
//	int* a = new int[n];
//	for (i = 0; i < n; i++)
//		a[i] = i + 1;
//	connections.clear();
//	Print(a, k);
//	if(n >= k){
//		while (NextSet(a, n, k)){
//			Print(a, k);
//		}
//	}
//
//	for(i = 0; i < connections.size(); i++){
//		printf("\n");
//		for(j =  0; j < connections[i].size(); j++)
//			printf("%d ", connections[i][j]);
//	}
//}