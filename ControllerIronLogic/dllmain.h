#pragma once

#ifdef CONTROLLERIRONLOGIC_EXPORTS
#define CONTROLLERIRONLOGIC_EXPORTS __declspec(dllexport)
#else
#define CONTROLLERIRONLOGIC_EXPORTS __declspec(dllimport)
#endif
#include <boost/algorithm/string.hpp>
#include "ZGuardClassControler.h"

//#define profiling

ZGuardClassController* Controler;

//New
//Проинициализировать конвертер
extern "C" CONTROLLERIRONLOGIC_EXPORTS bool DllInitConverter(char* PortName) {
	std::string _PortName(PortName);
	std::wstring stemp = std::wstring(_PortName.begin(), _PortName.end());
	LPCWSTR _CvtPortName = stemp.c_str();
	Controler = new ZGuardClassController(_CvtPortName);
	Controler->Init();
	return true;
};

//New
//Сменить активный контролер
extern "C" CONTROLLERIRONLOGIC_EXPORTS bool DllChangeContextController(int indexController) {
	#ifdef profiling
		clock_t t0 = clock();
	#endif		
	Controler->ChangeContext(indexController);
	//cout << "!!!!!!!!!!!!!!DllChangeContextController = " << indexController << "!!!!!!!!!!!!!!!!!!!!!!!!";
	#ifdef profiling
		clock_t t1 = clock();
		cout << "ChangeContext time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
	#endif
	
	return true;
};

//New
//Сменить активный контролер
extern "C" CONTROLLERIRONLOGIC_EXPORTS void DllClearBank() {
#ifdef profiling
	clock_t t0 = clock();
#endif		
	Controler->ClearAllKey();
	//cout << "!!!!!!!!!!!!!!DllChangeContextController = " << indexController << "!!!!!!!!!!!!!!!!!!!!!!!!";
#ifdef profiling
	clock_t t1 = clock();
	cout << "ClearBank time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
#endif
};

//New
//Прочитать новые событие
extern "C" CONTROLLERIRONLOGIC_EXPORTS int DllDoShowNewEvents(int indexEventInController) {
#ifdef profiling
	clock_t t0 = clock();
#endif
	auto res = Controler->DoShowNewEvents(indexEventInController);
#ifdef profiling
	clock_t t1 = clock();
	cout << "DllDoShowNewEvents time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
#endif
	return res;
};

//New
//Обновить банк ключей
extern "C" CONTROLLERIRONLOGIC_EXPORTS void DllUpdateBankKey(int indexBank) {
#ifdef profiling
	clock_t t0 = clock();
#endif		
	Controler->UpdateBankKey(indexBank);
#ifdef profiling
	clock_t t1 = clock();
	cout << "DllUpdateBankKey time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
#endif
};

//New
//Запросить банк ключей
extern "C" CONTROLLERIRONLOGIC_EXPORTS char* DllGetBankKey() {
#ifdef profiling
	clock_t t0 = clock();
#endif		
	stringstream ss;
	ss << "{\"cards\":[";
	int index = 0;
	for (auto& Keys : *Controler->KeyInController)
	{

		std::string key = readKeyInBufferHex(Keys.rNum);
		boost::to_upper(key);
		ss << "{\"card\":\""<<key<<"\",";
		ss << "\"pos\":" << index << ",";
		ss << "\"flags\":0,";
		ss << "\"Keys.fErased\":" << Keys.fErased << ",";
		if (index == Controler->KeyInController->size()-1) {
			ss << "\"tz\":255}";
		}
		else {
			ss << "\"tz\":255},";
		}
		++index;
			
	};
	ss << "]}";
	std::string returnValue(ss.str());
	if(returnValue.length() > 0) {
		char* data = new char[ss.str().size() + 1];
		std::copy(returnValue.begin(), returnValue.end(), data);
		data[returnValue.size()] = '\0';
		#ifdef profiling
				clock_t t1 = clock();
				cout << "DllGetBankKey time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
		#endif
		return data;
	}
	else {
		//std::cout << "{DLLINFO} Не было еще событий.." << std::endl;
		return nullptr;
	}
};


//New
//Запросить индексы удалёных ключей
extern "C" CONTROLLERIRONLOGIC_EXPORTS char* DllGetDeleteIndexKey() {
#ifdef profiling
	clock_t t0 = clock();
#endif		
	stringstream ss;
	ss << "{\"cards\":[";
	int index = 0;
	for (auto& _IndexKey : *Controler->EmptyKeyIndexInController)
	{

		std::string key = readKeyInBufferHex(Controler->KeyInController->at(_IndexKey).rNum);
		boost::to_upper(key);
		ss << "{\"card\":\"" << key << "\",";
		ss << "\"pos\":"<< _IndexKey <<",";
		ss << "\"flags\":0,";
		if (index == Controler->EmptyKeyIndexInController->size() - 1) {
			ss << "\"tz\":255}";
		}
		else {
			ss << "\"tz\":255},";
		}
		++index;

	};
	ss << "]}";
	std::string returnValue(ss.str());
	if (returnValue.length() > 0) {
		char* data = new char[ss.str().size() + 1];
		std::copy(returnValue.begin(), returnValue.end(), data);
		data[returnValue.size()] = '\0';
#ifdef profiling
		clock_t t1 = clock();
		cout << "DllGetDeleteIndexKey time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
#endif
		return data;
	}
	else {
		//std::cout << "{DLLINFO} Не было еще событий.." << std::endl;
		return nullptr;
	}
};

//New
//Открыть текущую дверь
extern "C" CONTROLLERIRONLOGIC_EXPORTS void DllOpenDoor(int sideLock) {
#ifdef profiling
	clock_t t0 = clock();
#endif		
	Controler->OpenDoor(sideLock);
#ifdef profiling
	clock_t t1 = clock();
	cout << "DllOpenDoor time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
#endif
};

//New
//Добавить новую карточку в выбраный контролер
extern "C" CONTROLLERIRONLOGIC_EXPORTS void DllAddCart(char* stringCart) {
#ifdef profiling
	clock_t t0 = clock();
#endif
	cout <<"stringCart => "<< &stringCart << endl;
	std::string key(stringCart);
	cout <<"\n\n{DLLINFO} DllAddCart(" << key<<")\n";
	Controler->AddCart(key);
#ifdef profiling
	clock_t t1 = clock();
	cout << "DllAddCart time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
#endif
};

//New
//Добавить новую карточку в выбраный контролер
extern "C" CONTROLLERIRONLOGIC_EXPORTS void DllAddCartWithIndex(char* stringCart,int* IndexArr,int Size) {
#ifdef profiling
	clock_t t0 = clock();
#endif
	cout << "stringCart => " << &stringCart << endl;
	std::string key(stringCart);
	cout << "\n\n{DLLINFO} DllAddCart(" << key << ")\n";
	vector<int>IndexVector;
	for (int i = 0; i < Size; i++) {
		cout << IndexArr[i]<<endl;
		IndexVector.emplace_back(IndexArr[i]);
	}
	Controler->AddCart(key,IndexVector);
#ifdef profiling
	clock_t t1 = clock();
	cout << "DllAddCart time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
#endif
};


//New
//Удалить карточку в выбраный контролер
extern "C" CONTROLLERIRONLOGIC_EXPORTS int* DllDeleteCard(char* stringCart) {
#ifdef profiling
	clock_t t0 = clock();
#endif
	cout << "stringCart => " << &stringCart << endl;
	std::string key(stringCart);
	cout << "\n\n{DLLINFO} DeleteCard(" << key << ")\n";
	auto vector = Controler->DeleteCard(key);
	int* data;
	if (!vector.empty()) {
		data = new int[vector.size()];
		std::copy(vector.begin(), vector.end(), data);	
	}
	else {
		//Ключа такого нет возвращаем -1
		data = new int[1];
		data[0] = -1;
	}
	
#ifdef profiling
	clock_t t1 = clock();
	cout << "DllDeleteCard time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
#endif
	return data;
};

//New
//Cделал хуйню (первичная инициализация позже переделаю мамой клянусь)
extern "C" CONTROLLERIRONLOGIC_EXPORTS int  DllDoCtrEventsMenu(int _CtrAddr, int LastEventIndexInController) {
#ifdef profiling
	clock_t t0 = clock();
#endif
	auto res = Controler->DoCtrEventsMenu(_CtrAddr, LastEventIndexInController);
#ifdef profiling
	clock_t t1 = clock();
	cout << "DllDoCtrEventsMenu time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
#endif
	return res;
};

//OLD
//Запросить последний ключ поднесеный к контролеру
extern "C" CONTROLLERIRONLOGIC_EXPORTS const char* DllLastKeyGet() {
#ifdef profiling
	clock_t t0 = clock();
#endif		
	const char data[64]{ *Controler->LastKeyGet().c_str() };
	if (strlen(data) > 0) {
		#ifdef profiling
				clock_t t1 = clock();
				cout << "DllLastKeyGet time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
		#endif
		return data;
	}
	else {
		//std::cout << "{DLLINFO} Не было еще карт.." << std::endl;
		return "";
	}
		
};

//OLD
//Собрать событие в формате json и передать их как строку
extern "C" CONTROLLERIRONLOGIC_EXPORTS char* DllStrControllerEventsJson() {
#ifdef profiling
	clock_t t0 = clock();
#endif		
	std::string returnValue = Controler->LastControllerEventsJson();	
	if (returnValue.length() > 0) {
		char* data = new char[returnValue.size() + 1];
		std::copy(returnValue.begin(), returnValue.end(), data);
		data[returnValue.size()] = '\0';
		#ifdef profiling
				clock_t t1 = clock();
				cout << "DllStrControllerEventsJson time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
		#endif
		return data;
	}
	else {
		//std::cout << "{DLLINFO} Не было еще событий.." << std::endl;
		return nullptr;
	}

};

//OLD
//Собрать все событие в формате json и передать их как строку
extern "C" CONTROLLERIRONLOGIC_EXPORTS char* DllStrAllControllerEventsJson() {
#ifdef profiling
	clock_t t0 = clock();
#endif		
	std::string returnValue = Controler->AllControllerEventsJson();
	if (returnValue.length() > 0) {
		char* data = new char[returnValue.size() + 1];
		std::copy(returnValue.begin(), returnValue.end(), data);
		data[returnValue.size()] = '\0';
		#ifdef profiling
				clock_t t1 = clock();
				cout << "DllStrAllControllerEventsJson time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
		#endif
		return data;
	}
	else {
		//std::cout << "{DLLINFO} Не было еще событий.." << std::endl;
		return nullptr;
	}

};
