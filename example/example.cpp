#include "example.h"

const int BUFSIZE = 1024;

TCHAR chReadBuf[BUFSIZE];
BOOL fSuccess;
DWORD cbRead;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\ApriltagPipeIn");

int trackernum = 4;

int main()
{
	std::cout << "Waiting..." << std::endl;

	const int BUFSIZE = 1024;
	std::string pipeName = "\\\\.\\pipe\\ApriltagPipeIn";

	std::istringstream ret;
	std::string word;

	ret = Send(TEXT("numtrackers"));
	ret >> word;
	if (word != "numtrackers")
	{
		std::cout << "Wrong message received!" << std::endl;
		return 24;
	}
	int connected_trackers;
	ret >> connected_trackers;
	for(int i = connected_trackers;i<trackernum;i++)
	{
		ret = Send(TEXT("addtracker"));
		ret >> word;
		if (word != "added")
		{
			std::cout << "Wrong message received!" << std::endl;
			return 25;
		}
	}

	//ret = Send(TEXT("addstation"));

	Sleep(1000);

	//ret = Send(TEXT("updatestation 0 2 1 0 1 0 0 0"));

	clock_t start, end;
	while (true)
	{		
		//for timing our detection
		start = clock();

		ret = Send(TEXT("synctime"));
		double avgtime; int lasttime;
		
		ret >> avgtime;
		ret >> lasttime; 

		Sleep((int)(avgtime - (lasttime % (int)avgtime)));
		//Sleep(30);

		ret = Send(TEXT("getdevicepose 1")); // 0 for HMD, 1 is left controller
		ret >> word;
		if (word != "devicepose")
		{
			std::cout << "Wrong message received!" << " " << word << std::endl;
			continue;
			return 26;
		}

		//first three variables are a position vector
		int idx; double a; double b; double c;

		//second four are rotation quaternion
		double qw; double qx; double qy; double qz;

		//read to our variables
		ret >> idx; ret >> a; ret >> b; ret >> c; ret >> qw; ret >> qx; ret >> qy; ret >> qz;

		std::cout << a << " " << b << " " << c << std::endl;

		SendTracker(0, a, b, c, qw, qx, qy, qz, -1, 0.9);
		SendTracker(1, a - 1, b, c, qw, qx, qy, qz, -1, 0.9);
		SendTracker(2, a, b, c + 1, qw, qx, qy, qz, -1, 0.9);
		SendTracker(3, a, b, c - 1, qw, qx, qy, qz, -1, 0.9);

		//ret = Send(TEXT("updatestation 0 0 0 0 1 0 0 0"));

		end = clock();
		double frameTime = double(end - start) / double(CLOCKS_PER_SEC);

		std::cout << frameTime << double(CLOCKS_PER_SEC) << std::endl;
	}
}

std::istringstream Send(LPTSTR lpszWrite)
{
	fSuccess = CallNamedPipe(
		lpszPipename,        // pipe name 
		lpszWrite,           // message to server 
		(lstrlen(lpszWrite) + 1) * sizeof(TCHAR), // message length 
		chReadBuf,              // buffer to receive reply 
		BUFSIZE * sizeof(TCHAR),  // size of read buffer 
		&cbRead,                // number of bytes read 
		2000);                 // waits for 2 seconds 

	if (fSuccess || GetLastError() == ERROR_MORE_DATA)
	{
		std::cout << chReadBuf << std::endl;
		chReadBuf[cbRead] = '\0'; //add terminating zero
					//convert our buffer to string
		std::string rec = chReadBuf;
		std::istringstream iss(rec);
		// The pipe is closed; no more data can be read. 

		if (!fSuccess)
		{
			printf("\nExtra data in message was lost\n");
		}
		return iss;
	}
	else
	{
		std::cout << GetLastError() << " :(" << std::endl;
		std::string rec = " senderror";
		std::istringstream iss(rec);
		return iss;
	}
}

std::istringstream SendTracker(int id, double a, double b, double c, double qw, double qx, double qy, double qz, double time, double smoothing)
{
	std::string s;
	s = " updatepose " + std::to_string(id) +
		" " + std::to_string(a) +
		" " + std::to_string(b) +
		" " + std::to_string(c) +
		" " + std::to_string(qw) +
		" " + std::to_string(qx) +
		" " + std::to_string(qy) +
		" " + std::to_string(qz) +
		" " + std::to_string(time) +
		" " + std::to_string(smoothing) + "\n";

	//send the string to our driver

	LPTSTR sendstring = (LPTSTR)s.c_str();

	return Send(sendstring);
}