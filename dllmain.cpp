
#include "pch.h"
#include "CSerialPort/CSerialPort.h"
#include "CSerialPort/CSerialPort.c"
#include "SimpleIni/SimpleIni.h"
#include "SimpleIni/ConvertUTF.h"
#include "Wchar_conv/Wchar_conv.h"
#include "Wchar_conv/Wchar_conv.cpp"
#include "timercpp.h"
#include <iostream>

	using namespace std;
	PORT port;
	HANDLE mainth;
	HANDLE systh;

	float val[10000];
	float prevval[10000];
	float sysval[25];
	float prevsysval[25];
	string strval[99];
	string prevstrval[99];
	bool trigger[999];
	bool abortthread=false;
	int serialport;
	int baudrate;
	int sl_autosend;
	float valsave[10000][1];
	unsigned short maxLvar = 0;
	unsigned short maxStvar = 0;
	unsigned short maxSvar = 0;


	void czekaj(int iMilisekundy)
	{
		clock_t koniec = clock() + iMilisekundy * CLOCKS_PER_SEC / 1000.0;
		while (clock() < koniec) continue;

	}

	DWORD WINAPI MainThread(LPVOID lpParam)
	{
		SendData(port, ("START:" + to_string(47) + ":" + to_string(11) + '\n').c_str());

		while (!abortthread) {
			for (int x = 0; x <= maxLvar; x++) {
				if (strval[x] != prevstrval[x]) {
					SendData(port, ("$V:" + to_string(x) + ":" + strval[x] + '\n').c_str());
					prevstrval[x] = strval[x];
				}
			}

			for (int x = 0; x < 10000; x++) {
				if (val[x] != prevval[x]) {
					SendData(port, ("LV:" + to_string(x) + ":" + to_string(val[x]) + '\n').c_str());
					prevval[x] = val[x];
				}
			}
			
		}
		return(0);
	}


	void loadconfigfile() {
		CSimpleIniA ini;
		ini.SetUnicode();
		SI_Error rc = ini.LoadFile(".\\plugins\\Omsi2Komsi.opl");
		if (rc >= 0) {
			serialport = atoi(ini.GetValue("omsi2komsi", "COMx"));
			baudrate = atoi(ini.GetValue("omsikomsi", "BAUD_RATE"));
		}


	}

	std::string encode(const std::wstring& wstr, int codePage = GetACP())
	{
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(codePage, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(codePage, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	__declspec(dllexport) void __stdcall PluginStart(void* aOwner)
	{
		//AllocConsole();
		//freopen("CONOUT$", "w", stdout);

		// int msgbox = MessageBox(0, "PluginStart", "Debug", MB_OK);

		loadconfigfile();
		cout << serialport << endl;
		if (serialport != 0) {
			cout << serialport << endl;
			port = OpenPort(serialport);
			SetPortBoudRate(port, baudrate);
			mainth = CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
		}
	}


	__declspec(dllexport) void __stdcall AccessVariable(unsigned short varindex, float* value, bool* write)
	{
		if (maxLvar < varindex) {
			maxLvar = varindex;
			}
		if (valsave[varindex][1] == 1) {
			valsave[varindex][1] = 0;
			*write = 1;
			*value = valsave[varindex][0];
		}
			val[varindex] = round( (float)*value);
			//*write = 0;
	}

	__declspec(dllexport) void __stdcall AccessStringVariable(unsigned short varindex, wchar_t* value, bool* write)
	{
		if (maxStvar < varindex) {
			maxStvar = varindex;
		}
			strval[varindex] = encode(value);
	}

	__declspec(dllexport) void __stdcall AccessTrigger(unsigned short triggerindex, bool* active)
	{
			*active = trigger[triggerindex];	
	}

	__declspec(dllexport) void __stdcall AccessSystemVariable(unsigned short varindex, float* value)
	{
		if (maxSvar < varindex) {
			maxSvar = varindex;
		}
		sysval[varindex] = (float)*value;
	}

	__declspec(dllexport) void __stdcall PluginFinalize()
	{
		abortthread = true;
		WaitForSingleObject(mainth, 2000);
		CloseHandle(mainth);
		ClosePort(port);
	}
