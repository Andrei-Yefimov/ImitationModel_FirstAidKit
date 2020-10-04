//#include "simlib.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <list>
//
//#define EVENT_ARRIVAL 1			// Тип события для поступления работы в систему
//#define EVENT_DEPARTURE 2		// Тип события для ухода работы с определенного рабочего места. 
//#define EVENT_END_SIMULATION 3	// Тип события для завершения моделирования
//#define STREAM_INTERARRIVAL 1	// Поток случайных чисел для интервалов времени между поступлениями работ
//#define STREAM_DESEASE_TYPE 2	// Поток случайных чисел для типов работ
//#define STREAM_SERVICE 3		// Поток случайных чисел для времени обслуживания
//#define DESEASE_TYPES 3			// Лёгкое заболевание \ среднее \ тяжёлое
//#define SAMPST_DELAYS 1			// переменная функции sampst для задержек в очереди(очередях). 
//
//struct doctor {
//	float probToDetermDiagn;
//	float probSendToAnother;
//	float probOfReTaking;
//};
//
//struct patient {
//	float diseaseType;
//	float timeToNext;
//	bool isIll;
//};
//
//int i, j, 
//	min_doctors = 3, max_doctors = 15, num_doctors = min_doctors,
//	desease_type;
//float mean_interarrival = 1.0,
//	mean_service = 4.5,
//	length_doctor_works = 8.0,
//	length_simulation = 365.0,
//	prob_distrib_desease_type[26];
//
//struct doctor* doctors;
//struct patient* patients;
//std::list<int> queue;
//
//FILE* infile, * outfile;
//bool NextSet(int* a, int n, int m);
//void Print(int *a, int n);
//void getCnk(int n, int k);
//void arrive(void);
//void depart(int cabinet);
//void report(void);
//
//int main() {
//	srand(time(NULL));
//	infile = fopen("in.txt", "r");
//	outfile = fopen("out.txt", "a");
//	int n = 4, m = 2, c;
//
//	/**/
//	fprintf(outfile, "Поликлиника с несколькими врачами\n");
//	fprintf(outfile, "Число врачей %d - %d\n", min_doctors, max_doctors);
//	fprintf(outfile, "Среднее время между прибытиями %.3f мин\n",
//		mean_interarrival);
//	fprintf(outfile, "Среднее время обслуживания %.3f мин\n",
//		mean_service);
//	fprintf(outfile, "Врачи работают до %.3f ч\n",
//		length_doctor_works);
//
//	for(num_doctors = min_doctors; num_doctors <= max_doctors; ++num_doctors){
//		init_simlib();
//		maxatr = 4;
//		event_schedule(expon(mean_interarrival, STREAM_INTERARRIVAL), EVENT_ARRIVAL);
//		event_schedule(60 * length_doctor_works, EVENT_END_SIMULATION);
//		timing();
//		for(c = 0; c < 10; c++) {
//			while(list_size[LIST_EVENT] != 0){
//				timing();
//				switch(next_event_type){
//				case EVENT_ARRIVAL:
//					arrive();
//					break;
//				case EVENT_DEPARTURE:
//					depart((int) transfer[3]); 
//					break;
//				case EVENT_END_SIMULATION:
//					event_cancel(EVENT_ARRIVAL);
//					break;
//				}
//			}
//		}
//		report();
//	}
//	/*
//
//	for(; n >= m; m++){
//		getCnk(n, m);
//	}
//*/
//	fclose(infile);
//	fclose(outfile);
//	system("pause");
//	return 0;
//}
//
//void arrive(void) {
//	int cabinet = rand() % num_doctors;
//	event_schedule(sim_time + expon(mean_interarrival,
//		STREAM_INTERARRIVAL), EVENT_ARRIVAL);
//	
//	// Если кабинет свободен, начинаем обслуживание прибывшего пациента
//	if(list_size[num_doctors + cabinet] == 0){
//		// Эта касса свободна, поэтому задержка клиента равна 0. 
//		sampst(0.0, SAMPST_DELAYS);
//
//		// Переводим сабинет в состояние занятости 
//		// (атрибуты в этом случае значения не имеют). 
//		list_file(FIRST, num_doctors + cabinet);
//
//		// Планируем завершение обслуживания
//		transfer[3] = cabinet; /* Определяем третий атрибут для записи
//								события типа 2 в списке событий перед
//								вызовом функции event_schedule. */
//		event_schedule(sim_time + expon(mean_service,
//				STREAM_SERVICE), EVENT_DEPARTURE);
//			//  Возвращаен управление основной функции. 
//		return ;
//	}
//
//	//  Возвращаен управление основной функции. 
//	// (выбираем крайнюю левую из одинаковых коротких очередей). 
//	int shortest_length = list_size[1];
//	int shortest_queue = 1;
//	for (cabinet = 2; cabinet <= num_doctors; ++cabinet)
//		if (list_size[cabinet] < shortest_length) {
//			shortest_length = list_size[cabinet];
//			shortest_queue = cabinet;
//		}
//
//	//  Определяем клиента в конец крайней левой
//	// очереди из числа самых коротких очередей
//	transfer[1] = sim_time;
//	list_file(LAST, shortest_queue);
//}
//
//void depart(int cabinet){
//	if(list_size[cabinet] == 0)
//		list_remove(FIRST, num_doctors + cabinet);
//	else {
//		list_remove(FIRST, cabinet);
//		sampst(sim_time - transfer[1], SAMPST_DELAYS);
//		transfer[3] = cabinet; /* определяем перед вызовом event_shedule*/
//		event_schedule(sim_time + expon(mean_service,
//				STREAM_SERVICE), EVENT_DEPARTURE);
//	}
//}
//
//void report() {
//	int cabinet;
//	float avg_num_in_queue = 0.0;
//	for(cabinet = 1; cabinet <= num_doctors; ++cabinet)
//		avg_num_in_queue += filest(cabinet);
//	fprintf(outfile, "\n\nС %d врачами, среднее число пациентов в очереди = %.3f",
//		num_doctors, avg_num_in_queue);
//	fprintf(outfile, "\n\nЗадержки в очереди, мин:\n");
//	out_sampst(outfile,  SAMPST_DELAYS, SAMPST_DELAYS);
//}
//
//void init(float determDiagn, float toAnother, float reTaking) {
//	for(i = 0; i < num_doctors; i++){
//		doctors[i].probToDetermDiagn = determDiagn;
//		doctors[i].probSendToAnother = toAnother;
//		doctors[i].probOfReTaking = reTaking;
//	}
//
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
//	for (i = 0; i < n; i++){
//		printf("%d ", a[i]);
//		// printf(".");
//	}
//}
//
//void getCnk(int n, int k){
//	int* a = new int[n];
//	for (i = 0; i < n; i++)
//		a[i] = i + 1;
//	Print(a, k);
//	if(n >= k){
//		while (NextSet(a, n, k)){
//			Print(a, k);
//			// printf(".");
//		}
//	}
//}