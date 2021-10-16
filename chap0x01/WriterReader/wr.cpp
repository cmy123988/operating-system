#include <windows.h>
#include <stdio.h>
#include <time.h>

DWORD dwID;

struct ThreadInfo
{
	int id;
	char type;
	double s;
	double d;
};
ThreadInfo threads[100];//�߳�����
HANDLE hThread[100];
HANDLE mutex, w;
int c = 0;//�Խ��̽��м���
int Reader_Count = 0;//�Զ����̼���

void Read_ThreadInfo();//��ȡ�ļ���Ľ���
void Read_Write();
DWORD WINAPI Reader(LPVOID lpParam);
DWORD WINAPI Writer(LPVOID lpParam);


int main()
{
	Read_ThreadInfo();
	Read_Write();
	return 0;
}


void Read_ThreadInfo()
{
	FILE* fp;
	fp = fopen("in.txt", "r");
	while (fscanf(fp, "%d %c %lf %lf", &threads[c].id,
		&threads[c].type, &threads[c].s, &threads[c].d) != EOF) {
		c++;
	}
	fclose(fp);
}

void Read_Write()
{
	mutex = CreateSemaphore(NULL, 1, 3, ("mutex"));//�������ڶԼ�����read_count�����Ļ����ź���
	w = CreateSemaphore(NULL, 1, 3, ("w"));//����һ���Ƿ�����д���ź���
	for (int i = 0; i < c; ++i) {
		if (threads[i].type == 'W') {
			hThread[i] = CreateThread(NULL, 0, Writer, &threads[i],0, &dwID);//����д�߽��̣�����ֵΪ�������̵߳ľ��
		}
		else {
			hThread[i] = CreateThread(NULL, 0, Reader, &threads[i], 0, &dwID);//�������߽��̣�����ֵΪ�������̵߳ľ��
		}
	}
	WaitForMultipleObjects(c, hThread, true, INFINITE); //��ָ����ʱ���ڵȴ��������Ϊ����״̬��
}

DWORD WINAPI Reader(LPVOID lpParam)
{
	ThreadInfo* arg = (ThreadInfo*)lpParam;

	while (true) {
		Sleep(arg->s * 1000);
		printf("�߳� %d �����!\n", arg->id);
		WaitForSingleObject(mutex, INFINITE);//��ָ����ʱ���ڵȴ�mutex�ź�������Ϊ����״̬
		if (Reader_Count == 0) {
			WaitForSingleObject(w, INFINITE);//��ָ����ʱ���ڵȴ�w�ź�������Ϊ����״̬��ռ��w�źţ���ֹ��ʱд�߽���
		}
		Reader_Count++;
		ReleaseSemaphore(mutex, 1, NULL);//�ͷ�mutex�ź���
		printf("�߳� %d ���ڶ�!\n", arg->id);
		Sleep(arg->d * 1000);
		printf("�߳� %d ������!\n", arg->id);
		WaitForSingleObject(mutex, INFINITE);
		Reader_Count--;
		if (Reader_Count == 0) {//������еĶ��߽��̶�������
			ReleaseSemaphore(w, 1, NULL);//�ͷ�w����д���ź���
		}
		ReleaseSemaphore(mutex, 1, NULL);
	}


}
DWORD WINAPI Writer(LPVOID lpParam)
{
	ThreadInfo* arg = (ThreadInfo*)lpParam;

	while (true) {
	Sleep(arg->s * 1000);
	printf("�߳� %d ����д!\n", arg->id);
	WaitForSingleObject(w , INFINITE);
	printf("�߳� %d ����д!\n", arg->id);
	Sleep(arg->d * 1000);
	printf("�߳� %d д����!\n", arg->id);
	ReleaseSemaphore(w , 1, NULL);
	}

}

