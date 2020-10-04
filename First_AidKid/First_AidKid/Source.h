//#include "simlib.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <list>
//
//#define EVENT_ARRIVAL 1			// ��� ������� ��� ����������� ������ � �������
//#define EVENT_DEPARTURE 2		// ��� ������� ��� ����� ������ � ������������� �������� �����. 
//#define EVENT_END_SIMULATION 3	// ��� ������� ��� ���������� �������������
//#define STREAM_INTERARRIVAL 1	// ����� ��������� ����� ��� ���������� ������� ����� ������������� �����
//#define STREAM_DESEASE_TYPE 2	// ����� ��������� ����� ��� ����� �����
//#define STREAM_SERVICE 3		// ����� ��������� ����� ��� ������� ������������
//#define DESEASE_TYPES 3			// ˸���� ����������� \ ������� \ ������
//#define SAMPST_DELAYS 1			// ���������� ������� sampst ��� �������� � �������(��������). 
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
//	fprintf(outfile, "����������� � ����������� �������\n");
//	fprintf(outfile, "����� ������ %d - %d\n", min_doctors, max_doctors);
//	fprintf(outfile, "������� ����� ����� ���������� %.3f ���\n",
//		mean_interarrival);
//	fprintf(outfile, "������� ����� ������������ %.3f ���\n",
//		mean_service);
//	fprintf(outfile, "����� �������� �� %.3f �\n",
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
//	// ���� ������� ��������, �������� ������������ ���������� ��������
//	if(list_size[num_doctors + cabinet] == 0){
//		// ��� ����� ��������, ������� �������� ������� ����� 0. 
//		sampst(0.0, SAMPST_DELAYS);
//
//		// ��������� ������� � ��������� ��������� 
//		// (�������� � ���� ������ �������� �� �����). 
//		list_file(FIRST, num_doctors + cabinet);
//
//		// ��������� ���������� ������������
//		transfer[3] = cabinet; /* ���������� ������ ������� ��� ������
//								������� ���� 2 � ������ ������� �����
//								������� ������� event_schedule. */
//		event_schedule(sim_time + expon(mean_service,
//				STREAM_SERVICE), EVENT_DEPARTURE);
//			//  ���������� ���������� �������� �������. 
//		return ;
//	}
//
//	//  ���������� ���������� �������� �������. 
//	// (�������� ������� ����� �� ���������� �������� ��������). 
//	int shortest_length = list_size[1];
//	int shortest_queue = 1;
//	for (cabinet = 2; cabinet <= num_doctors; ++cabinet)
//		if (list_size[cabinet] < shortest_length) {
//			shortest_length = list_size[cabinet];
//			shortest_queue = cabinet;
//		}
//
//	//  ���������� ������� � ����� ������� �����
//	// ������� �� ����� ����� �������� ��������
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
//		transfer[3] = cabinet; /* ���������� ����� ������� event_shedule*/
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
//	fprintf(outfile, "\n\n� %d �������, ������� ����� ��������� � ������� = %.3f",
//		num_doctors, avg_num_in_queue);
//	fprintf(outfile, "\n\n�������� � �������, ���:\n");
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