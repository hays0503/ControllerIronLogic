//#pragma once
#ifndef AllHeader_H
#define AllHeader_H
//
#include "pch.h"
#include <boost/beast/core.hpp> 
#include <boost/beast/http.hpp> 
#include <boost/beast/version.hpp> 
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>	
#include <string>
#include <iostream>
#include <locale.h>
#include "ZGuard.h"
#include "ZPort.h"
#include "Utils.h"

#if !defined(ZGUARD_LINKONREQUEST)
#pragma comment(lib, "./ZGuard.lib")
#elif !defined(ZPORT_LINKONREQUEST)
#pragma comment(lib, "./ZGuard.lib")
#endif // !ZGUARD_LINKONREQUEST

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using namespace boost;             // from <boost/json.hpp>
using namespace boost::json;             // from <boost/json.hpp>
static LPCTSTR EvTypeStrs[] = {
	TEXT(""),
	TEXT("Open button"),				// ������� ������� �������
	TEXT("Key not found"),				// ���� �� ������ � ����� ������
	TEXT("Open key"),					// ���� ������, ����� �������
	TEXT("Key unscheduled"),			// ���� ������, ������ �� ��������
	TEXT("Open RS485"),					// ������� ���������� �� ����
	TEXT("Door is blocked (key)"),		// ���� ������, ����� �������������
	TEXT("Door is blocked (button)"),	// ������� ������� ��������������� ����� �������
	TEXT("Door is broken"),				// ����� ��������
	TEXT("Door is left open"),			// ����� ��������� �������� (timeout)
	TEXT("Passage"),					// ������ ���������
	TEXT("Sensor 1"),					// �������� ������ 1
	TEXT("Sensor 2"),					// �������� ������ 2
	TEXT("Controller is rebooted"),		// ������������ �����������
	TEXT("Button is disabled"),			// ������������� ������ ����������
	TEXT("Double pass"),				// ������� �������� �������
	TEXT("Door opened"),				// ����� ������� ������
	TEXT("Door closed"),				// ����� �������
	TEXT("Power off"),					// ������� �������
	TEXT("Power is on (ElectroControl)"),	// ��������� ��������������
	TEXT("Power is off (ElectroControl)"),	// ��������� ��������������
	TEXT("Enable the lock (trigger)"),		// ��������� ����� (�������)
	TEXT("Disable the lock (trigger)"),	// ���������� ����� (�������)
	TEXT("Changed state Mode"),			// ��������� ��������� �����
	TEXT("Changed state Fire"),			// ��������� ��������� ������
	TEXT("Changed state Security"),		// ��������� ��������� ������
	TEXT("Unknown key"),				// ����������� ����
	TEXT("Passage in the gateway"),		// �������� ���� � ����
	TEXT("Blocked the entrance to the gateway (busy)"),		// ������������ ���� � ���� (�����)
	TEXT("Allowed to enter the gateway"),	// �������� ���� � ����
	TEXT("AntiPassBack"),				// ������������ ������ (�����������)
	TEXT("Hotel40"),
	TEXT("Hotel41")
};

static LPCTSTR DirectStrs[] = {
	TEXT(""),
	TEXT("IN"),   // ����
	TEXT("OUT")   // �����
};

static LPCTSTR EcSubEvStrs[] = {
	TEXT(""),
	TEXT("CARD_DELAY"),			// ��������� �������� ����� � ������ ������� (��� �����) �������� ��������
	TEXT("RESERVED1"),			// (���������������)
	TEXT("ON_NET"),				// �������� �������� �� ����
	TEXT("OFF_NET"),			// ��������� �������� �� ����
	TEXT("ON_SCHED"),			// �������� �� ��������� ����
	TEXT("OFF_SHED"),			// ��������� �� ��������� ����
	TEXT("CARD"),				// ��������� �������� ����� � ������������ ����������
	TEXT("RESERVED2"),			// (���������������)
	TEXT("OFF_TIMEOUT"),		// ��������� ����� ��������� ��������
	TEXT("OFF_EXIT")			// ��������� �� ������������ ������� ������
};

static LPCTSTR FireSubEvStrs[] = {
	TEXT(""),
	TEXT("OFF_NET"),			// ��������� �� ����
	TEXT("ON_NET"),				// �������� �� ����
	TEXT("OFF_INPUT_F"),		// ��������� �� ����� FIRE
	TEXT("ON_INPUT_F"),			// �������� �� ����� FIRE
	TEXT("OFF_TEMP"),			// ��������� �� ������� �����������
	TEXT("ON_TEMP")				// �������� �� ������� �����������
};

static LPCTSTR SecurSubEvStrs[] = {
	TEXT(""),
	TEXT("OFF_NET"),			// ��������� �� ����
	TEXT("ON_NET"),				// �������� �� ����
	TEXT("OFF_INPUT_A"),		// ��������� �� ����� ALARM
	TEXT("ON_INPUT_A"),			// �������� �� ����� ALARM
	TEXT("OFF_TAMPERE"),		// ��������� �� �������
	TEXT("ON_TAMPERE"),			// �������� �� �������
	TEXT("OFF_DOOR"),			// ��������� �� ������� �����
	TEXT("ON_DOOR")				// �������� �� ������� �����
};

static LPCTSTR ModeSubEvStrs[] = {
	TEXT(""),
	TEXT("RS485 allow"),		// ��������� �������� �� ����
	TEXT("RS485 denied"),		// �������� ��������� �� ����
	TEXT("Timezone start"),			// �������� ��������� ����
	TEXT("Timezone finish"),			// ���������� ��������� ����
	TEXT("Card allow"),			// ��������� ������
	TEXT("Card denied")			// �������� ��������� ������
};

static LPCTSTR ModeStrs[] = {
	TEXT(""),
	TEXT("Norm"),				// �����
	TEXT("Block"),				// ����������
	TEXT("Free"),				// ���������
	TEXT("Wait")				// ��������
};

static LPCTSTR HModeStrs[] = {
	TEXT(""),
	TEXT("Norm"),				// �����
	TEXT("Block"),				// ����������
	TEXT("Free"),				// ���������
	TEXT("???")					// ���������������
};

static LPCTSTR HotelSubEvStrs[] = {
	TEXT(""),
	TEXT("FreeCard"),			// ����� ��������
	TEXT("BlockCard"),			// ����� �����������
	TEXT("DopFunc"),			// �������������� �������
	TEXT("NewRCard"),			// ������� ��������� �����
	TEXT("Network"),
	TEXT("Timezone"),
	TEXT("Counter"),			// �������� �������
	TEXT("CryptoKey"),			// �������� ����������
	TEXT("Pulse Z"),			// ����������� ������� � ������� 2� ������
	TEXT("State Change")		// ��������� ������� -���� ������ ����� � ��������� ����� ��� ����� 2 ������� 
};

//C ������� ���� ��������� ����������� ������ � �����
//(������ ����������� ���)
struct dtoThread
{
	HANDLE& g_hEvent;
	HANDLE& g_hCtr;
	BOOL& g_fThreadActive;
	INT& g_nCtrMaxEvents;
	json::array& ControlerEventsJson;//C������ �� ���������� ����� ����{JSON}
	vector<_ZG_CTR_KEY>* KeyInController;
	vector<int>* EmptyKeyIndexInController;
	bool& isSendData;
	std::string& LastKey;
};

struct CartDto
{
	vector<_ZG_CTR_KEY>* KeyInController;
	vector<int>* EmptyKeyIndexInController;
};
//#include <boost/beast/core.hpp>
//#include <boost/beast/http.hpp>
//#include <boost/beast/version.hpp>
//#include <boost/asio/dispatch.hpp>
//#include <boost/asio/strand.hpp>
//#include <boost/config.hpp>
//#include <boost/asio/ip/tcp.hpp>
//#include <boost/json.hpp>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
//#include <iostream>
//#include <algorithm>
//#include <cstdlib>
//#include <functional>
//#include <iostream>
//#include <memory>
//#include <string>
//#include <thread>
//#include <vector>
//#include <Windows.h>
//#include <conio.h>
//#include <stdio.h>
//#include <typeinfo>
//
//namespace beast = boost::beast;         // from <boost/beast.hpp>
//namespace http = beast::http;           // from <boost/beast/http.hpp>
//namespace net = boost::asio;            // from <boost/asio.hpp>
//using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//
//using namespace boost;             // from <boost/json.hpp>
//using namespace boost::json;             // from <boost/json.hpp>
//
//// Report a failure
//// �������� �� ������
//void
//fail(beast::error_code ec, char const* what)
//{
//    std::cerr << what << ": " << ec.message() << "\n";
//}
//
//template <class Body, class Allocator>
//http::message_generator
//handle_requestCustom(
//    beast::string_view doc_root,
//    http::request<Body, http::basic_fields<Allocator>>&& req)
//{
//    // Returns a bad request response
//    // ���������� �������� ����� �� ������
//    auto const bad_request =
//        [&req](beast::string_view why)
//    {
//        http::response<http::string_body> res{ http::status::bad_request, req.version() };
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, "text/html");
//        res.keep_alive(req.keep_alive());
//        res.body() = std::string(why);
//        res.prepare_payload();
//        return res;
//    };
//
//    // Returns a not found response
//    // ���������� �� ��������� �����
//    auto const not_found =
//        [&req](beast::string_view target)
//    {
//        http::response<http::string_body> res{ http::status::not_found, req.version() };
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, "text/html");
//        res.keep_alive(req.keep_alive());
//        res.body() = "The resource '" + std::string(target) + "' was not found.";
//        res.prepare_payload();
//        return res;
//    };
//
//    // Returns a server error response
//    // ���������� ����� �� ������ �������
//    auto const server_error =
//        [&req](beast::string_view what)
//    {
//        http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, "text/html");
//        res.keep_alive(req.keep_alive());
//        res.body() = "An error occurred: '" + std::string(what) + "'";
//        res.prepare_payload();
//        return res;
//    };
//
//    // Make sure we can handle the method
//    // ���������, ��� �� ����� ���������� �����
//    if (req.method() != http::verb::get &&
//        req.method() != http::verb::head)
//        return bad_request("Unknown HTTP-method");
//
//    // Request path must be absolute and not contain "..".
//    // ���� ������� ������ ���� ���������� � �� ��������� "..".
//    if (req.target().empty() ||
//        req.target()[0] != '/' ||
//        req.target().find("..") != beast::string_view::npos)
//        return bad_request("Illegal request-target");
//
//    beast::error_code ec;
//
//    // Handle an unknown error
//    // ��������� ����������� ������
//    if (ec)
//        return server_error(ec.message());
//
//    // Respond to GET request
//    // ����� �� GET-������
//    http::response<http::string_body> res{ http::status::ok, req.version() };
//    std::string resultContent = "{ 'data':123 }";
//    auto const size = resultContent.size();
//    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//    res.set(http::field::content_type, beast::string_view("application/json"));
//    res.content_length(size);
//    res.body() = resultContent;
//    res.keep_alive(req.keep_alive());
//
//    return res;
//}
//
//
//
//
//
#endif
