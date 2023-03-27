// CtrEvents.cpp: определяет точку входа для консольного приложения.
//
#include "pch.h"
#include "ZGuardClassControler.h"
#include <ctime>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <thread>
#include <typeinfo>
#include <ctime>

using namespace std;

vector <_ZG_CTR_EVENT> pEventVectorUnkKey;
vector <_ZG_CTR_EVENT> pEventVectorPassEvent;

	bool readFunc(INT nPos, INT nMax, PVOID pUserData) {
		//cout <<"{DLLINFO}Текущий шаг выполнения функции." << nPos << endl;
		//cout <<"{DLLINFO} Всего количество шагов." << nMax << endl;
		return true;
	}

	std::string readKeyInBuffer(Z_KEYNUM& pNum) {
		BYTE key[2];
		key[0] = pNum[1];
		key[1] = pNum[2];
		WORD* keyDword = (WORD*)key;
		//cout << "{DLLINFO} Key:" << WORD(pNum[3]) << "," << *keyDword << endl;
		std::string key_str = to_string(WORD(pNum[3])) + "," + to_string(*keyDword);
		return key_str;
	};

	std::string readKeyInBufferHex(Z_KEYNUM& pNum) {
		BYTE key[2];
		key[0] = pNum[1];
		key[1] = pNum[2];
		WORD* keyDword = (WORD*)key;
		std::stringstream ss;
		ss <<"000000"<< std::hex << WORD(pNum[3]) << *keyDword;
		return ss.str();
	};

	std::string DecodeKeyInBufferHex(BYTE aData[8]) {
		std::stringstream ss;
		ss << std::hex << WORD(aData[7]) << WORD(aData[6]) << WORD(aData[5]) << WORD(aData[4]) << WORD(aData[3]) << WORD(aData[2]);
		return ss.str();
	};

	BOOL TryHexToDec(TCHAR chHex, INT* pDec)
	{
		if ((chHex >= L'0') && (chHex <= L'9'))
			*pDec = chHex - L'0';
		else if ((chHex >= L'a') && (chHex <= L'f'))
			*pDec = chHex - L'a' + 10;
		else if ((chHex >= L'A') && (chHex <= L'F'))
			*pDec = chHex - L'A' + 10;
		else
			return FALSE;
		return TRUE;
	}

	BOOL ParseKeyNum(Z_KEYNUM& rKeyNum, LPCTSTR pszText)
	{
		INT nGroup, nNumber;
		ZeroMemory(&rKeyNum, sizeof(rKeyNum));
		if (_stscanf_s(pszText, TEXT("%d,%d"), &nGroup, &nNumber) == 2)
		{
			rKeyNum[0] = 3;
			*(PWORD)(&rKeyNum[1]) = nNumber;
			rKeyNum[3] = nGroup;
		}
		else
		{
			INT i, j, n, n2;
			i = _tcslen(pszText) - 2;
			j = 1;
			while (i >= 0)
			{
				if (!TryHexToDec(pszText[i], &n) || !TryHexToDec(pszText[i + 1], &n2))
					return FALSE;
				rKeyNum[j] = (n2 & 0xF) | ((n & 0xF) << 4);
				j++;
				if (j > 6)
					break;
				i -= 2;
			}
			rKeyNum[0] = (j - 1);
		}
		return TRUE;
	}

	//Idx			Позиция текущего ключа.
	//Key			Параметры ключа.
	//Pos			Текущий шаг выполнения функции.
	//Max			Всего количество шагов.
	//UserData		Параметр пользователя для Callback - функции.
	bool readAllKeyFunc(INT nIdx, PZG_CTR_KEY pKey, INT nPos, INT nMax, PVOID pUserData) {
		//vector<_ZG_CTR_KEY>* KeyInControler = ((vector<_ZG_CTR_KEY>*)pUserData);
		//vector<int>* EmptyKeyIndexInController = ((vector<int>*)pUserData);
		CartDto* pCartDto = ((CartDto*)pUserData);
		pCartDto->KeyInController->push_back(*pKey);
		cout << "{DLLINFO} |Поз.ключа. => " << nIdx << " | Тек.шаг => " << nPos;
		cout << " | В.шагов => " << nMax;
		cout << " | Ключ.E-marrin => " << readKeyInBuffer(pKey->rNum);
		cout << " | Ключ.Hex => " << readKeyInBufferHex(pKey->rNum);
		if (pKey->fErased){
			pCartDto->EmptyKeyIndexInController->push_back(nIdx);
			cout << " | Флаг.fErased => " << pKey->fErased;
		};
		cout << " | Ключ.E-marrin => " << readKeyInBuffer(pKey->rNum)<<endl;
		return true;
	}

	ZGuardClassController::ZGuardClassController(LPCWSTR _CvtPortName) {
	//Инициализация
		CvtPortName = _CvtPortName;
		oZGL = new CZGuardLoader();
		if (!oZGL->IsLoaded())
		{
			_tprintf(TEXT("Load ZGuard error 0x%X.\n"), oZGL->m_nStatus);
			throw std::exception("Проблемы с dll возможно она отсутствует");
		}
		KeyInController = new vector<_ZG_CTR_KEY>;
		EmptyKeyIndexInController = new vector<int>;
		_PointCartDto = nullptr;

		dto = new	dtoThread{
			g_hEvent,
			g_hCtr,
			g_fThreadActive,
			g_nCtrMaxEvents,
			ControlerEventsJson,
			KeyInController,
			EmptyKeyIndexInController,
			isSendData,
			LastKey};


	};

	ZGuardClassController::~ZGuardClassController() {
		//Инициализация		
		delete oZGL;
		oZGL = nullptr;

		delete KeyInController;
		KeyInController = nullptr;

		delete EmptyKeyIndexInController;
		EmptyKeyIndexInController = nullptr;

		delete _PointCartDto;
		_PointCartDto = nullptr;

		delete dto;
		dto = nullptr;

		if (g_hCtr != NULL)
			ZG_CloseHandle(g_hCtr);
		if (hCvt != NULL)
			ZG_CloseHandle(hCvt);
		if (g_hEvent != NULL)
			CloseHandle(g_hEvent);
		ZG_Finalyze();
		cout << "{DLLINFO} Выполнен деструктор эмулятора\n";
	};

	void ZGuardClassController::ShowEvents(int nStart, int nCount, dtoThread& dto){
			_ZG_CTR_EVENT aEvents[6];
			PZG_CTR_EVENT pEv;
			int i, j, nIdx, nCnt;

			bool isZG_EV_UNKNOWN_KEY = false;

			i = j = nIdx = nCnt = 0;
			while (i < nCount)
			{
				nIdx = (nStart + i) % dto.g_nCtrMaxEvents;
				nCnt = (nCount - i);
				if (nCnt > _countof(aEvents))
					nCnt = _countof(aEvents);

				if ((nIdx + nCnt) > dto.g_nCtrMaxEvents)
					nCnt = (dto.g_nCtrMaxEvents - nIdx);

				if (!CheckZGError(ZG_Ctr_ReadEvents(dto.g_hCtr, nIdx, aEvents, nCnt, NULL, NULL), _T("ZG_Ctr_ReadEvents")))
					return;
				for (j = 0; j < nCnt; j++)
				{
					pEv = &aEvents[j];
					switch (pEv->nType)
					{
					case ZG_EV_PASSAGE:
						break;
					//case ZG_EV_ELECTRO_ON:
					//case ZG_EV_ELECTRO_OFF:
					//{
					//	_ZG_EV_TIME rTime;
					//	ZG_EC_SUB_EV nSubEvent;
					//	DWORD nPowerFlags;
					//	ZG_Ctr_DecodeEcEvent(dto.g_hCtr, pEv->aData, &rTime, &nSubEvent, &nPowerFlags);
					//	//_tprintf(TEXT("{DLLINFO} %.4d. %.2d.%.2d %.2d:%.2d:%.2d %s Sub_event: %s, Power flags: %d\n"),
					//	//	nIdx + j,
					//	//	rTime.nDay, rTime.nMonth,
					//	//	rTime.nHour, rTime.nMinute, rTime.nSecond,
					//	//	EvTypeStrs[pEv->nType],
					//	//	EcSubEvStrs[nSubEvent], nPowerFlags);
					//}
					//break;
					//case ZG_EV_FIRE_STATE:
					//{
					//	_ZG_EV_TIME rTime;
					//	ZG_FIRE_SUB_EV nSubEvent;
					//	DWORD nFireFlags;
					//	ZG_Ctr_DecodeFireEvent(dto.g_hCtr, pEv->aData, &rTime, &nSubEvent, &nFireFlags);
					//	//_tprintf(TEXT("{DLLINFO} %.4d. %.2d.%.2d %.2d:%.2d:%.2d %s Sub_event: %s, Fire flags: %d\n"),
					//	//	nIdx + j,
					//	//	rTime.nDay, rTime.nMonth,
					//	//	rTime.nHour, rTime.nMinute, rTime.nSecond,
					//	//	EvTypeStrs[pEv->nType],
					//	//	FireSubEvStrs[nSubEvent], nFireFlags);
					//}
					//break;
					//case ZG_EV_SECUR_STATE:
					//{
					//	_ZG_EV_TIME rTime;
					//	ZG_SECUR_SUB_EV nSubEvent;
					//	DWORD nSecurFlags;
					//	ZG_Ctr_DecodeSecurEvent(dto.g_hCtr, pEv->aData, &rTime, &nSubEvent, &nSecurFlags);
					//	/*_tprintf(TEXT("{DLLINFO} %.4d. %.2d.%.2d %.2d:%.2d:%.2d %s Sub_event: %s, Security flags: %.2Xh\n"),
					//		nIdx + j,
					//		rTime.nDay, rTime.nMonth,
					//		rTime.nHour, rTime.nMinute, rTime.nSecond,
					//		EvTypeStrs[pEv->nType],
					//		SecurSubEvStrs[nSubEvent], nSecurFlags);*/
					//}
					//break;
					//case ZG_EV_MODE_STATE:
					//{
					//	_ZG_EV_TIME rTime;
					//	ZG_CTR_MODE nMode;
					//	ZG_MODE_SUB_EV nSubEvent;
					//	ZG_Ctr_DecodeModeEvent(dto.g_hCtr, pEv->aData, &rTime, &nMode, &nSubEvent);
					//	/*_tprintf(TEXT("{DLLINFO} %.4d. %.2d.%.2d %.2d:%.2d:%.2d %s Mode: %s, Sub_event: %s\n"),
					//		nIdx + j,
					//		rTime.nDay, rTime.nMonth,
					//		rTime.nHour, rTime.nMinute, rTime.nSecond,
					//		EvTypeStrs[pEv->nType],
					//		ModeStrs[nMode],
					//		ModeSubEvStrs[nSubEvent]);*/
					//}
					//break;
					case ZG_EV_UNKNOWN_KEY:
					{
						Z_KEYNUM rNum;
						ZG_Ctr_DecodeUnkKeyEvent(dto.g_hCtr, pEv->aData, &rNum);
						/*
						std::cout << "{DLLINFO} ZG_EV_UNKNOWN_KEY: " << std::endl;
						_tprintf(TEXT("{DLLINFO} %.4d. Key \"%s\"\n"),nIdx + j,ZKeyNumToStr(rNum, g_fProximity).c_str());
						std::cout << "{DLLINFO} HEX: "<< readKeyInBufferHex(rNum) << std::endl;
						*/

						///////////////////////////////

						_ZG_EV_TIME rTime;
						ZG_CTR_DIRECT nDirect;
						INT nKeyIdx, nKeyBank;
						ZG_Ctr_DecodePassEvent(dto.g_hCtr, pEv->aData, &rTime, &nDirect, &nKeyIdx, &nKeyBank);

						//Формируем время текущее
						std::time_t current_time = std::time(nullptr);
						std::tm local_time;
						localtime_s(&local_time, &current_time);
						stringstream time;
						time << (local_time.tm_year + 1900) << "-"
							<< static_cast<int>(rTime.nMonth) << "-"
							<< static_cast<int>(rTime.nDay) << " "
							<< static_cast<int>(rTime.nHour) << ":"
							<< static_cast<int>(rTime.nMinute) << ":"
							<< static_cast<int>(rTime.nSecond);

						json::object Event{ {
						{"event",pEv->nType},
						{"card", readKeyInBufferHex(rNum)},
						{"time",time.str()},
						{"flag",0},
						{"direct",nDirect},// Установим значение в шаге (default)

						}, };

						isZG_EV_UNKNOWN_KEY = true;

						cout << "nDirect - ZG_EV_UNKNOWN_KEY = " << nDirect<<'\n';


						//Вставляем в массив событие
						dto.ControlerEventsJson.emplace_back(Event);
						dto.LastKey = readKeyInBufferHex(rNum);
					}
					break;
					case ZG_EV_KEY_OPEN:
					case ZG_EV_KEY_ACCESS:
					{
						_ZG_EV_TIME rTime;
						ZG_CTR_DIRECT nDirect;
						INT nKeyIdx, nKeyBank;
						ZG_Ctr_DecodePassEvent(dto.g_hCtr, pEv->aData, &rTime, &nDirect, &nKeyIdx, &nKeyBank);

						//Мы нашли ключ в банке пробуем сформировать сообщение
						if (nKeyIdx >= 0) {
							if (nKeyIdx <= dto.KeyInController->size()) {

								_ZG_CTR_KEY a = dto.KeyInController->at(nKeyIdx);

								//Формируем время текущее
								std::time_t current_time = std::time(nullptr);
								std::tm local_time;
								localtime_s(&local_time, &current_time);
								stringstream time;
								time << (local_time.tm_year + 1900) << "-"
									<< static_cast<int>(rTime.nMonth) << "-"
									<< static_cast<int>(rTime.nDay) << " "
									<< static_cast<int>(rTime.nHour) << ":"
									<< static_cast<int>(rTime.nMinute) << ":"
									<< static_cast<int>(rTime.nSecond);



								json::object Event{ {
								{"event",pEv->nType},
								{"card", readKeyInBufferHex(a.rNum)},
								{"time",time.str()},
								{"flag",0},
								{"direct",nDirect},

								}, };

								dto.ControlerEventsJson.emplace_back(Event);

								dto.LastKey = readKeyInBuffer(a.rNum);
							}
						};

						//Вставляем в массив событие
						//_tprintf(TEXT("{DLLINFO} %.4d. %.2d.%.2d %.2d:%.2d:%.2d %s %s (key_idx: %d, bank#: %d)\n"),
						//	nIdx + j,
						//	rTime.nDay, rTime.nMonth,
						//	rTime.nHour, rTime.nMinute, rTime.nSecond,
						//	DirectStrs[nDirect],
						//	EvTypeStrs[pEv->nType],
						//	nKeyIdx, nKeyBank);
						
						//PZG_CTR_KEY pBuf = new _ZG_CTR_KEY;
						//ZG_Ctr_ReadKeys(
						//	dto.g_hCtr,
						//	nKeyIdx,
						//	pBuf,
						//	1,
						//	*ZG_PROCESSCALLBACK(readFunc),
						//	NULL,
						//	nKeyBank
						//);
						//cout << "pBuf:" << readKeyInBuffer(pBuf->rNum) << endl;
						//delete pBuf;
						//pBuf = nullptr;
						//Z_KEYNUM pNum;
						//ZG_Ctr_ReadLastKeyNum(dto.g_hCtr,&pNum);
						//readKeyInBuffer(pNum);
						
					}
					break;
					default:
					{
						_ZG_EV_TIME rTime;
						ZG_CTR_DIRECT nDirect;
						INT nKeyIdx, nKeyBank;
						ZG_Ctr_DecodePassEvent(dto.g_hCtr, pEv->aData, &rTime, &nDirect, &nKeyIdx, &nKeyBank);
						cout << "nDirect - default = " << nDirect << '\n';
						if (isZG_EV_UNKNOWN_KEY) {
							auto& json = dto.ControlerEventsJson.back();
							json.at("direct") = nDirect;
							isZG_EV_UNKNOWN_KEY = false;
						}
						/*_tprintf(TEXT("{DLLINFO} %.4d. %.2d.%.2d %.2d:%.2d:%.2d %s %s (key_idx: %d, bank#: %d)\n"),
							nIdx + j,
							rTime.nDay, rTime.nMonth,
							rTime.nHour, rTime.nMinute, rTime.nSecond,
							DirectStrs[nDirect],
							EvTypeStrs[pEv->nType],
							nKeyIdx, nKeyBank);*/
					}
					break;
					}
				}
				i += nCnt;
			}
		}

	DWORD WINAPI ZGuardClassController::staticNotifyThreadProc(LPVOID lpParameter)
	{
		dtoThread *dto = ((dtoThread*)lpParameter);
		//Вызов потока NotifyThreadProc ну или обработчка
		return ((ZGuardClassController*)lpParameter)->NotifyThreadProc(*dto);
	}

	DWORD ZGuardClassController::NotifyThreadProc(dtoThread dto)
	{
		while (dto.g_fThreadActive)
		{
			if (WaitForSingleObject(dto.g_hEvent, INFINITE) == WAIT_OBJECT_0)
			{
				ResetEvent(dto.g_hEvent);
				if (dto.g_hCtr != NULL) {
					HRESULT hr;
					UINT nMsg;
					LPARAM nMsgParam;
					while ((hr = ZG_Ctr_GetNextMessage(dto.g_hCtr, &nMsg, &nMsgParam)) == S_OK)
					{
						switch (nMsg)
						{
							case ZG_N_CTR_NEW_EVENT:
							{
								PZG_N_NEW_EVENT_INFO pInfo = PZG_N_NEW_EVENT_INFO(nMsgParam);
								_tprintf(TEXT("{DLLINFO} ==> New Events: %d\n"), pInfo->nNewCount);
								//readKeyInBuffer(pInfo->rLastNum);								
								ShowEvents(pInfo->nReadIdx, pInfo->nNewCount,dto);
								g_nAppReadEventIdx = pInfo->nWriteIdx;
							}
						}
					}
					if (hr == ZP_S_NOTFOUND)
						hr = S_OK;
				}
					
			}
		}
		return 0;
	}

	//void ZGuardClassController::StartNotifyThread()
	//{
	//	if (g_hThread != NULL)
	//		return;
	//	DWORD nThreadId;
	//	g_fThreadActive = TRUE;
	//	g_hThread = CreateThread(NULL, 0, staticNotifyThreadProc, (void*)dto, 0, &nThreadId);
	//}

	//void ZGuardClassController::StopNotifyThread()
	//{
	//	if (g_hThread == NULL)
	//		return;
	//	g_fThreadActive = FALSE;
	//	SetEvent(g_hEvent);
	//	WaitForSingleObject(g_hThread, INFINITE);
	//	CloseHandle(g_hThread);
	//	delete dto;
	//	g_hThread = NULL;
	//}

	/*void ZGuardClassController::EnableNotification(BOOL fEnable, BOOL fReport)
	{
		if (fEnable)
		{
			if (g_hEvent == NULL)
				g_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			_ZG_CTR_NOTIFY_SETTINGS rNS = { 0 };
			rNS.nNMask = ZG_NF_CTR_NEW_EVENT;
			rNS.hEvent = g_hEvent;
			rNS.nReadEvIdx = g_nAppReadEventIdx;
			rNS.nCheckStatePeriod = 300;
			if (!CheckZGError(ZG_Ctr_SetNotification(g_hCtr, &rNS), _T("ZG_Ctr_SetNotification")))
				return;
			StartNotifyThread();
		}
		else
		{
			StopNotifyThread();
			if (!CheckZGError(ZG_Ctr_SetNotification(g_hCtr, NULL), _T("ZG_Ctr_SetNotification")))
				return;
		}
		g_fCtrNotifyEnabled = fEnable;
		if (fReport)
			_tprintf(TEXT("{DLLINFO} Done.\n"));
	}*/

	void ZGuardClassController::SetControllerMode(int mode) {
		ZG_Ctr_SetCtrMode(g_hCtr, ZG_CTR_MODE(mode));
	}

	void ZGuardClassController::SetTimeLock(DWORD nLetMs=1.0, DWORD nMaxMs=1.0,INT nBankN=1.0) {
		ZG_Ctr_WriteLockTimes(g_hCtr, ZG_MODE_BLOCK,1.0, nLetMs, nMaxMs, nBankN);
	}

	void ZGuardClassController::OpenDoor(int sideLock) {
		ZG_Ctr_OpenLock(g_hCtr, sideLock);
	}

	vector<int> ZGuardClassController::DeleteCard(std::string stringKeyCart) {

		UpdateBankKey(0);

		INT nIdx=0;
		INT nCount = 1;
		INT nBankN = 0;

		//auto callback = [](INT nPos, INT nMax, PVOID pUserData)->BOOL {
		//	int Onepercent = nMax / 100.0;
		//	int CurentPercent = nPos % Onepercent;

		//	if (CurentPercent == 0)
		//		cout << int(nPos / Onepercent) << "%..";
		//	return true;
		//};

		vector<int>Index;

		for (auto& Key : *KeyInController)
		{
			std::string key1 = readKeyInBufferHex(Key.rNum);
			boost::to_upper(key1);
			std::string key2(stringKeyCart);
			boost::to_upper(key2);
			cout << "{DLLINFO} Сравниваю " << key1 << "==" << key2 << "\n";
			if (!key1.compare(key2)) {
				cout << "\n\n\n{DLLINFO} " << key1 << "==" << key2<<"\n\n\n";
				if (!CheckZGError(ZG_Ctr_ClearKeys(g_hCtr, nIdx, nCount, nullptr, nullptr, nBankN), _T("ZG_Ctr_ClearKeys"))) {
					cout << "\n\n\n{DLLINFO} Ошибка при удаление ключа   =>"<< key2 <<"\n\n\n";
					return Index;
				}
				else {
					_ZG_CTR_KEY rKey = { 0 };	//{ false, Num, ZG_KEY_NORMAL, ZG_KF_SHORTNUM, 0xFFh, aData1 };

					rKey.rNum[15] = 0x00;
					rKey.rNum[14] = 0x00;
					rKey.rNum[13] = 0x00;
					rKey.rNum[12] = 0x00;
					rKey.rNum[11] = 0x00;
					rKey.rNum[10] = 0x00;
					rKey.rNum[9] = 0x00;
					rKey.rNum[8] = 0x00;
					rKey.rNum[7] = 0x00;
					rKey.rNum[6] = 0x00;
					rKey.rNum[5] = 0x00;
					rKey.rNum[4] = 0x00;
					rKey.rNum[3] = 0x00;
					rKey.rNum[2] = 0x00;
					rKey.rNum[1] = 0x00;
					rKey.rNum[0] = 6;

					rKey.aData1[0] = 0x00;
					rKey.aData1[1] = 0x00;
					rKey.aData1[2] = 0x00;
					rKey.aData1[3] = 0x00;
					rKey.fErased = true;
					rKey.nType = ZG_KEY_NORMAL;
					rKey.nFlags = ZG_KF_SHORTNUM;
					rKey.nAccess = 0xFF;

					int nBankN = 0;
					int nKeyIdx = -1;
					if (!CheckZGError(ZG_Ctr_WriteKeys(g_hCtr, nIdx, &rKey, 1, NULL, NULL, nBankN), _T("ZG_Ctr_WriteKeys"))){
						cout << "\n\n\n{DLLINFO} Ошибка при удаление ключа(на моменте перезаписи)   =>" << key2 << "\n\n\n";
						return Index;
					}
					else {
						cout << "\n\n\n{DLLINFO} Ключ удален успешно ключ    =>" << key2 << "\n\n\n";
						Index.emplace_back(nIdx);
					}
					
				}
									
			}
			++nIdx;
		}
		UpdateBankKey(0);
		return Index;
	}

	bool ZGuardClassController::AddCart(std::string stringKeyCart) {
		UpdateBankKey(0);

		//Поиск по базе клучей
		//Если находим то сбрасываем добавление ключа

		cout << "KeyInControler размерность: " << KeyInController->size() << "\n";

		for (auto& Key : *KeyInController)
		{
			
			std::string key1 = readKeyInBufferHex(Key.rNum);
			boost::to_upper(key1);
			std::string key2(stringKeyCart);
			boost::to_upper(key2);
			cout << "{DLLINFO} Сравниваю "<< key1 <<"=="<< key2<<"\n";
			if (!key1.compare(key2)) {
				if (Key.fErased) {
					break;
				}
				cout << "{DLLINFO} Похоже в данном банке ключей уже существует такой ключ, добавление отменено\n";
				return false;
			}
		}

		unsigned int x;
		stringstream ss;
		ss << std::hex << stringKeyCart;
		ss >> x;
		char bytes[sizeof x];
		std::copy(
			static_cast<const char*>(static_cast<const void*>(&x)),
			static_cast<const char*>(static_cast<const void*>(&x)) + sizeof x,
			bytes);
		std::reverse(bytes, bytes + sizeof x);

		_ZG_CTR_KEY rKey = { 0 };	//{ false, Num, ZG_KEY_NORMAL, ZG_KF_SHORTNUM, 0xFFh, aData1 };

		rKey.rNum[15] = 0x00;
		rKey.rNum[14] = 0x00;
		rKey.rNum[13] = 0x00;
		rKey.rNum[12] = 0x00;
		rKey.rNum[11] = 0x00;
		rKey.rNum[10] = 0x00;
		rKey.rNum[9] = 0x00;
		rKey.rNum[8] = 0x00;
		rKey.rNum[7] = 0x00;
		rKey.rNum[6] = 0x00;
		rKey.rNum[5] = 0x00;
		rKey.rNum[4] = bytes[0];
		rKey.rNum[3] = bytes[1];
		rKey.rNum[2] = bytes[2];
		rKey.rNum[1] = bytes[3];
		rKey.rNum[0] = 6;

		rKey.aData1[0] = 0x00;
		rKey.aData1[1] = 0x00;
		rKey.aData1[2] = 0x00;
		rKey.aData1[3] = 0x00;
		rKey.fErased = false;
		rKey.nType = ZG_KEY_NORMAL;
		rKey.nFlags = ZG_KF_SHORTNUM;
		rKey.nAccess = 0xFF;

		int nBankN = 0;
		int nKeyIdx = -1;

		this_thread::sleep_for(chrono::milliseconds(1500));



		if (!CheckZGError(ZG_Ctr_GetKeyTopIndex(g_hCtr, &nKeyIdx, nBankN), _T("ZG_Ctr_GetKeyTopIndex"))) {
			cout << "\n{DLLINFO} Не предведеный выход из функции добавление ключей\n";
			return false;
		}
			
		cout << "\n{DLLINFO} nKeyIdx: " << nKeyIdx << '\n';

		INT nCount;
		ZG_PROCESSCALLBACK pfnCallback;
		PVOID pUserData;
		if (!CheckZGError(ZG_Ctr_WriteKeys(g_hCtr, nKeyIdx, &rKey, 1, NULL, NULL, nBankN), _T("ZG_Ctr_WriteKeys")))
			return false;		
		std::cout << "{DLLINFO} Ключ добавлен!\n";
		return true;
	}

	bool ZGuardClassController::AddCart(std::string stringKeyCart,vector<int> EmptyIndexCart) {
		UpdateBankKey(0);

		//Поиск по базе клучей
		//Если находим то сбрасываем добавление ключа

		cout << "KeyInControler размерность: " << KeyInController->size() << "\n";

		for (auto& Key : *KeyInController)
		{

			std::string key1 = readKeyInBufferHex(Key.rNum);
			boost::to_upper(key1);
			std::string key2(stringKeyCart);
			boost::to_upper(key2);
			cout << "{DLLINFO} Сравниваю " << key1 << "==" << key2 << "\n";
			if (!key1.compare(key2)) {
				if (Key.fErased) {
					break;
				}
				cout << "{DLLINFO} Похоже в данном банке ключей уже существует такой ключ, добавление отменено\n";
				return false;
			}
		}

		unsigned int x;
		stringstream ss;
		ss << std::hex << stringKeyCart;
		ss >> x;
		char bytes[sizeof x];
		std::copy(
			static_cast<const char*>(static_cast<const void*>(&x)),
			static_cast<const char*>(static_cast<const void*>(&x)) + sizeof x,
			bytes);
		std::reverse(bytes, bytes + sizeof x);

		_ZG_CTR_KEY rKey = { 0 };	//{ false, Num, ZG_KEY_NORMAL, ZG_KF_SHORTNUM, 0xFFh, aData1 };

		rKey.rNum[15] = 0x00;
		rKey.rNum[14] = 0x00;
		rKey.rNum[13] = 0x00;
		rKey.rNum[12] = 0x00;
		rKey.rNum[11] = 0x00;
		rKey.rNum[10] = 0x00;
		rKey.rNum[9] = 0x00;
		rKey.rNum[8] = 0x00;
		rKey.rNum[7] = 0x00;
		rKey.rNum[6] = 0x00;
		rKey.rNum[5] = 0x00;
		rKey.rNum[4] = bytes[0];
		rKey.rNum[3] = bytes[1];
		rKey.rNum[2] = bytes[2];
		rKey.rNum[1] = bytes[3];
		rKey.rNum[0] = 6;

		rKey.aData1[0] = 0x00;
		rKey.aData1[1] = 0x00;
		rKey.aData1[2] = 0x00;
		rKey.aData1[3] = 0x00;
		rKey.fErased = false;
		rKey.nType = ZG_KEY_NORMAL;
		rKey.nFlags = ZG_KF_SHORTNUM;
		rKey.nAccess = 0xFF;

		int nBankN = 0;
		int nKeyIdx = -1;

		this_thread::sleep_for(chrono::milliseconds(1500));

		if (!EmptyIndexCart.empty()) {
			INT nCount;
			ZG_PROCESSCALLBACK pfnCallback;
			PVOID pUserData;
			nKeyIdx = EmptyIndexCart.back();
			EmptyIndexCart.pop_back();
			if (!CheckZGError(ZG_Ctr_WriteKeys(g_hCtr, nKeyIdx, &rKey, 1, NULL, NULL, nBankN), _T("ZG_Ctr_WriteKeys")))
				return false;
			std::cout << "{DLLINFO} Ключ добавлен!\n";
			return true;
		}
		else {

			if (!CheckZGError(ZG_Ctr_GetKeyTopIndex(g_hCtr, &nKeyIdx, nBankN), _T("ZG_Ctr_GetKeyTopIndex"))) {
				cout << "\n{DLLINFO} Не предведеный выход из функции добавление ключей\n";
				return false;
			}

			cout << "\n{DLLINFO} nKeyIdx: " << nKeyIdx << '\n';

			INT nCount;
			ZG_PROCESSCALLBACK pfnCallback;
			PVOID pUserData;
			if (!CheckZGError(ZG_Ctr_WriteKeys(g_hCtr, nKeyIdx, &rKey, 1, NULL, NULL, nBankN), _T("ZG_Ctr_WriteKeys")))
				return false;
			std::cout << "{DLLINFO} Ключ добавлен!\n";
			return true;
		}
	}

	void ZGuardClassController::DoRestoreFactorySettings()
	{
		_tprintf(TEXT("{DLLINFO} Writing (0, 0)...\n"));
		if (!CheckZGError(ZG_Ctr_WriteEventIdxs(g_hCtr, 0x3, 0, 0), _T("ZG_Ctr_WriteEventIdxs")))
			return ;
		g_nAppReadEventIdx = 0;
		_tprintf(TEXT("{DLLINFO} Done.\n"));
	}

	value ZGuardClassController::getControlerEventsJson()
	{
		return dto->ControlerEventsJson;
	}

	bool ZGuardClassController::UpdateBankKey(int bank) {
		cout << "{DLLINFO} Обновление банка ключей."<< endl;
		KeyInController->clear();
		EmptyKeyIndexInController->clear();
		/////////////////////////////////////////////////////////////////////////////////////////
		//cout << "\nАдресс #0 KeyInController : " << &KeyInController << endl;
		//cout << "\nАдресс #0 EmptyKeyIndexInController : " << &EmptyKeyIndexInController << endl;
		/////////////////////////////////////////////////////////////////////////////////////////


		if(_PointCartDto == nullptr){
			//Eсли нет у нас dto то создаем новое
			_PointCartDto = new CartDto;
		}else{
			//Eсли есть удаляем старое и создаем новое
			delete _PointCartDto;
			_PointCartDto = nullptr;
			_PointCartDto = new CartDto;
		}

		_PointCartDto->EmptyKeyIndexInController = EmptyKeyIndexInController;
		_PointCartDto->KeyInController = KeyInController;
		/////////////////////////////////////////////////////////////////////////////////////////
		//cout << "\nАдресс #0 _PointCartDto->EmptyKeyIndexInController : " << &_PointCartDto->EmptyKeyIndexInController << endl;
		//cout << "\nАдресс #0 _PointCartDto->KeyInController : " << &_PointCartDto->KeyInController << endl;
		/////////////////////////////////////////////////////////////////////////////////////////


		if (!CheckZGError(ZG_Ctr_EnumKeys(g_hCtr, 0, *ZG_ENUMCTRKEYSPROC(readAllKeyFunc), (void*)_PointCartDto, bank), _T("ZG_Ctr_EnumKeys")))
			delete _PointCartDto;
			_PointCartDto = nullptr;
			return false;
		if (KeyInController->size()<=0)
			cout << "{DLLINFO} Похоже ключей в банке нет. Номер банка ключей => " << bank << endl;


		//Убираем за собой
		delete _PointCartDto;
		_PointCartDto = nullptr;
		return true;
	};

	bool ZGuardClassController::Init() {


		if (!CheckZGError(ZG_Initialize(ZP_IF_NO_MSG_LOOP), _T("ZG_Initialize")))
			return false;
		//__try
		//{
		_ZG_CVT_OPEN_PARAMS rOp;
		ZeroMemory(&rOp, sizeof(rOp));
		rOp.nType = CvtPortType;
		rOp.pszName = CvtPortName;
		rOp.nSpeed = ZG_SPEED_57600;
		if (!CheckZGError(ZG_Cvt_Open(&hCvt, &rOp, NULL), _T("ZG_Cvt_Open")))			
			return false;
		return true;
	}

	void ZGuardClassController::ChangeMode(int Mode) {
		ZG_CTR_MODE ControlerMode = static_cast<ZG_CTR_MODE>(Mode);
		cout << "{DLLINFO} Должен был быть установлен режим на: =" << Mode << '\n';
		ZG_Ctr_SetCtrMode(g_hCtr, ControlerMode);
		ZG_CTR_MODE pCurrMode;
		DWORD pFlags;
		ZG_Ctr_GetCtrModeInfo(g_hCtr, &pCurrMode, &pFlags);
		cout << "{DLLINFO} Сменен режим на: =" << pCurrMode<<'\n';
	}

	void ZGuardClassController::ChangeContext(int _CtrAddr)
	{

		CtrAddr = _CtrAddr;
		_ZG_CTR_INFO rCtrInfo;
		ZeroMemory(&rCtrInfo, sizeof(rCtrInfo));
		if (!CheckZGError(ZG_Ctr_Open(&g_hCtr, hCvt, CtrAddr, 0, &rCtrInfo, ZG_CTR_UNDEF), _T("ZG_Ctr_Open"))) {
			throw std::exception("DoCtrEventsMenu => ZG_Ctr_Open");
			return;
		}
		g_nCtrMaxEvents = rCtrInfo.nMaxEvents;
		g_fProximity = (rCtrInfo.nFlags & ZG_CTR_F_PROXIMITY) != 0;
		g_nCtrFlags = rCtrInfo.nFlags;
		
	}

	int ZGuardClassController::DoCtrEventsMenu(int _CtrAddr, int LastEventIndexInControler)
	{
		CtrAddr = _CtrAddr;

		
		ZeroMemory(&rCtrInfo, sizeof(rCtrInfo));
		if (!CheckZGError(ZG_Ctr_Open(&g_hCtr, hCvt, CtrAddr, 0, &rCtrInfo, ZG_CTR_UNDEF), _T("ZG_Ctr_Open"))) {
			throw std::exception("DoCtrEventsMenu => ZG_Ctr_Open");
			return -2;
		}

		g_nCtrMaxEvents = rCtrInfo.nMaxEvents;
		g_fProximity = (rCtrInfo.nFlags & ZG_CTR_F_PROXIMITY) != 0;
		g_nCtrFlags = rCtrInfo.nFlags;
		_tprintf(TEXT("{DLLINFO} %s addr: %d, s/n: %d, v%d.%d, Max_Events: %d, Key_Type: %s\n"),
			CtrTypeStrs[rCtrInfo.nType],
			rCtrInfo.nAddr,
			rCtrInfo.nSn,
			LOBYTE(rCtrInfo.nVersion), HIBYTE(rCtrInfo.nVersion),
			rCtrInfo.nMaxEvents,
			KeyModeStrs[g_fProximity ? 1 : 0]);
		g_fCtrNotifyEnabled = FALSE;
		INT nWrIdx, nRdIdx;
		if (!CheckZGError(ZG_Ctr_ReadEventIdxs(g_hCtr, &nWrIdx, &nRdIdx), _T("ZG_Ctr_ReadEventIdxs")))
		{
			throw std::exception("DoCtrEventsMenu => ZG_Ctr_ReadEventIdxs");
			return -2;
		}
			cout << "{DLLINFO} Номер последнего индекса считанного из банка событий: "<< nWrIdx<<endl;
			////////////////////////////////////////////////
			//Устанавливаем индекс послднего события
			// Например если контролер под номером 5 считал на прошлом проходе 100 событие
			// То при смене контекста опять на 5 контролер
			// мы увидим что событий стало 105 значит нужно считать событие начиная с 100
			// иначе идем как шли
			if(LastEventIndexInControler != -1){
				if (LastEventIndexInControler == nWrIdx) {
					g_nAppReadEventIdx = nWrIdx;
				}					
				else {
					g_nAppReadEventIdx = LastEventIndexInControler;
				}					
			}
			//////////////////////////////
			
			//EnableNotification(TRUE, FALSE);
			return nWrIdx;

		//}
	}

	/*
	Handle[in] Дескриптор контроллера.
	Idx			[in] Номер первой стираемой карты.
	Count		[in] Количество стираемых карт.Если равен 7FFFFFFFh(MAXINT), то используется значение, равное(Верхняя граница ключей(ZG_Ctr_GetKeyTopIndex) - Idx).
	Callback	[in] Callback - функция для возможности показа статуса выполнения и возможности прерывания процесса.
	UserData	[in] Параметр пользователя для Callback - функции.
	BankN		[in] Номер банка(0 - 1).
	*/
	void ZGuardClassController::ClearAllKey() {

		auto callback = [](INT nPos, INT nMax, PVOID pUserData)->BOOL {
			//int Onepercent = nMax / 100.0;
			//int CurentPercent = nPos % Onepercent;

			//if (CurentPercent == 0)
				//cout << int(nPos/Onepercent) << "%..";
			cout << "nPos/" << nPos<<"|"<< "nMax/" << nMax <<'\n';
			return true;
		};
		ZG_Ctr_ClearKeys(g_hCtr, 0, MAXINT, callback, nullptr, 0);
		KeyInController->clear();
		cout << " Готово\n";
	}
	/*
	Handle[in] Дескриптор контроллера.
	Idx			[in] Номер первой стираемой карты.
	Count		[in] Количество стираемых карт.Если равен 7FFFFFFFh(MAXINT), то используется значение, равное(Верхняя граница ключей(ZG_Ctr_GetKeyTopIndex) - Idx).
	Callback	[in] Callback - функция для возможности показа статуса выполнения и возможности прерывания процесса.
	UserData	[in] Параметр пользователя для Callback - функции.
	BankN		[in] Номер банка(0 - 1).
	*/
	void ZGuardClassController::ClearСurrentKeyBank(int Index,int Count) {

		auto callback = [](INT nPos, INT nMax, PVOID pUserData)->BOOL {
			float Onepercent = nMax / 100.0;
			float CurentPercent = nPos / Onepercent;

			if (int(CurentPercent) % 1 == 0)
				cout << int(nPos / Onepercent) << "%..";
			return true;
		};
		cout << "{DLLINFO} Процесс выполнение в процентах "<< Index <<"/"<< Count <<" => ";
		ZG_Ctr_ClearKeys(g_hCtr, Index, Count, callback, nullptr, 0);
		KeyInController->clear();
		cout << " Готово\n";
	}

	int ZGuardClassController::DoShowNewEvents(int Index)
	{
		INT nWrIdx;
		g_nAppReadEventIdx = Index;
		if (!CheckZGError(ZG_Ctr_ReadEventIdxs(g_hCtr, &nWrIdx, NULL), _T("ZG_Ctr_ReadEventIdxs"))){
			throw std::exception("DoCtrEventsMenu => ZG_Ctr_Open");
			return -2;
		}
		INT nNewCount;
		if (nWrIdx >= g_nAppReadEventIdx)
			nNewCount = (nWrIdx - g_nAppReadEventIdx);
		else
			nNewCount = (g_nCtrMaxEvents - g_nAppReadEventIdx + nWrIdx);

		if (nNewCount != 0)
			_tprintf(TEXT("{DLLINFO} Available %d new events:\n"), nNewCount);			
		//else
			//_tprintf(TEXT("{DLLINFO} No new events\n"));
		INT nShowCount;
		while (nNewCount > 0)
		{
			nShowCount = min(nNewCount, 25);
			ShowEvents(g_nAppReadEventIdx, nShowCount, *dto);
			nNewCount -= nShowCount;
			g_nAppReadEventIdx = (g_nAppReadEventIdx + nShowCount) % g_nCtrMaxEvents;
		}
		return nWrIdx;
	}

	
	/*
	Idx			Позиция текущего события.
	Event		Параметры события.
	Pos			Текущий шаг выполнения функции.
	Max			Всего количество шагов.
	UserData	Параметр пользователя для Callback - функции.
	*/

	BOOL fnEnumProc(INT nIdx, PZG_CTR_EVENT pEvent, INT nPos, INT nMax, PVOID pUserData)
	{
		std::cout <<"nIdx: " << nIdx << " | " << "nPos: " << nPos << " | " << "nMax: " << nMax << std::endl << std::endl;
		if (pEvent->nType == ZG_EV_UNKNOWN_KEY) {
			//cout << "ZG_EV_UNKNOWN_KEY => " << DecodeKeyInBufferHex(pEvent->aData)<<std::endl;
			//std::cout << "pEvent->nType: " << pEvent->nType << " | " << "pEvent->aData: " << pEvent->aData << " | " << "nIdx: " << nIdx << " | " << "nPos: " << nPos << " | " << "nMax: " << nMax << std::endl << std::endl;
			pEventVectorUnkKey.emplace_back(*pEvent);
		}
		if (pEvent->nType == ZG_EV_KEY_OPEN) {
			//cout << "ZG_EV_UNKNOWN_KEY => " << DecodeKeyInBufferHex(pEvent->aData)<<std::endl;
			//std::cout << "pEvent->nType: " << pEvent->nType << " | " << "pEvent->aData: " << pEvent->aData << " | " << "nIdx: " << nIdx << " | " << "nPos: " << nPos << " | " << "nMax: " << nMax << std::endl << std::endl;
			pEventVectorPassEvent.emplace_back(*pEvent);
		}

		return true;
	};

	void ZGuardClassController::getCard()
	{
		Z_KEYNUM rNum;
		std::cout << "{DLLINFO} Вывод ключей которые не были в банке => ZG_EV_UNKNOWN_KEY\n";
		for (auto &Event : pEventVectorUnkKey)
		{
			ZG_Ctr_DecodeUnkKeyEvent(g_hCtr, Event.aData, &rNum);
			std::cout << "{DLLINFO} HEX: " << readKeyInBufferHex(rNum) << std::endl;

		}
		std::cout << "{DLLINFO} Вывод ключей которые не были в банке => ZG_EV_KEY_OPEN\n";
		for (auto& Event : pEventVectorPassEvent)
		{
			_ZG_EV_TIME rTime;
			ZG_CTR_DIRECT nDirect;
			INT nKeyIdx, nKeyBank;
			ZG_Ctr_DecodePassEvent(g_hCtr, Event.aData, &rTime, &nDirect, &nKeyIdx, &nKeyBank);
			_tprintf(TEXT("{DLLINFO} %.2d.%.2d %.2d:%.2d:%.2d %s %s (key_idx: %d, bank#: %d)\n"),
				rTime.nDay, rTime.nMonth,
				rTime.nHour, rTime.nMinute, rTime.nSecond,
				DirectStrs[nDirect],
				EvTypeStrs[Event.nType],
				nKeyIdx, nKeyBank);
			std::cout << "{DLLINFO} HEX: " << readKeyInBufferHex(rNum) << std::endl;

		}
	};

	void ZGuardClassController::DoShowAllEvents()
	{
		clock_t t0 = clock();
		INT nWrIdx;
		if (!CheckZGError(ZG_Ctr_EnumEvents(g_hCtr,0,8126/2, fnEnumProc,NULL), _T("ZG_Ctr_EnumEvents")))
			return;
		getCard();
		clock_t t1 = clock();
		cout << "time: " << (double)(t1 - t0) / CLOCKS_PER_SEC << endl;
	}

	void ZGuardClassController::SetClock(PZG_CTR_CLOCK pClock) {

		if (!CheckZGError(ZG_Ctr_SetClock(g_hCtr, pClock), _T("ZG_Ctr_SetClock"))) {
			cout << "{DLLINFO} Не удалось синхронезировате время на контролере: "<< rCtrInfo.nSn <<"\n";
			return;
		}
		
	}