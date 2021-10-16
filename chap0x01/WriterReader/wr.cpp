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
ThreadInfo threads[100];//线程数组
HANDLE hThread[100];
HANDLE mutex, w;
int c = 0;//对进程进行计数
int Reader_Count = 0;//对读进程计数

void Read_ThreadInfo();//获取文件里的进程
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
	mutex = CreateSemaphore(NULL, 1, 3, ("mutex"));//创建用于对计数器read_count操作的互斥信号量
	w = CreateSemaphore(NULL, 1, 3, ("w"));//创建一个是否允许写的信号量
	for (int i = 0; i < c; ++i) {
		if (threads[i].type == 'W') {
			hThread[i] = CreateThread(NULL, 0, Writer, &threads[i],0, &dwID);//创建写者进程，返回值为所创建线程的句柄
		}
		else {
			hThread[i] = CreateThread(NULL, 0, Reader, &threads[i], 0, &dwID);//创建读者进程，返回值为所创建线程的句柄
		}
	}
	WaitForMultipleObjects(c, hThread, true, INFINITE); //在指定的时间内等待多个对象为可用状态；
}

DWORD WINAPI Reader(LPVOID lpParam)
{
	ThreadInfo* arg = (ThreadInfo*)lpParam;

	while (true) {
		Sleep(arg->s * 1000);
		printf("线程 %d 请求读!\n", arg->id);
		WaitForSingleObject(mutex, INFINITE);//在指定的时间内等待mutex信号量对象为可用状态
		if (Reader_Count == 0) {
			WaitForSingleObject(w, INFINITE);//在指定的时间内等待w信号量对象为可用状态，占用w信号，防止读时写者进入
		}
		Reader_Count++;
		ReleaseSemaphore(mutex, 1, NULL);//释放mutex信号量
		printf("线程 %d 正在读!\n", arg->id);
		Sleep(arg->d * 1000);
		printf("线程 %d 读完了!\n", arg->id);
		WaitForSingleObject(mutex, INFINITE);
		Reader_Count--;
		if (Reader_Count == 0) {//如果所有的读者进程都结束了
			ReleaseSemaphore(w, 1, NULL);//释放w允许写的信号量
		}
		ReleaseSemaphore(mutex, 1, NULL);
	}


}
DWORD WINAPI Writer(LPVOID lpParam)
{
	ThreadInfo* arg = (ThreadInfo*)lpParam;

	while (true) {
	Sleep(arg->s * 1000);
	printf("线程 %d 请求写!\n", arg->id);
	WaitForSingleObject(w , INFINITE);
	printf("线程 %d 正在写!\n", arg->id);
	Sleep(arg->d * 1000);
	printf("线程 %d 写完了!\n", arg->id);
	ReleaseSemaphore(w , 1, NULL);
	}

}

