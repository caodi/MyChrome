#include "stdafx.h"
#include "UI/Wnd/MainWnd.h"
#include "include/cef_app.h"
#include "include/cef_command_line.h"
#include "common/client_app.h"
#include "browser/client_app_browser.h"
#include "renderer/client_app_renderer.h"
#include "common/client_app_other.h"
#include "browser/main_message_loop_std.h"
#include "ThirdPart/Common/Common.h"
using namespace client;



INT WINAPI wWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPWSTR lpCmdLine, __in int nShowCmd )
{
	//CefEnableHighDPISupport();
	CefMainArgs main_args(hInstance);
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());
	CefRefPtr<CefApp> app;
	ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
	if (process_type == ClientApp::BrowserProcess)
		app = new ClientAppBrowser();
	else if (process_type == ClientApp::RendererProcess)
		app = new ClientAppRenderer();
	else if (process_type == ClientApp::OtherProcess)
		app = new ClientAppOther();
	void* sandbox_info = NULL;
	int exit_code = CefExecuteProcess(main_args, app, sandbox_info); //���������
	if (exit_code >= 0)
	{
		return exit_code;
	}
	g_strExePath = GetExePath();
	CefSettings settings;//_cef_settings_t  cef_browser_settings_t
#ifndef DEBUG
	settings.log_severity = LOGSEVERITY_DISABLE;
#endif
	std::string locale("zh-CN");
	//cef_string_utf8_to_utf16(locale.c_str(), locale.size(), &settings.locale);
	std::string language("zh-CN,zh");//zh-CN,zh;q=0.8
	//cef_string_utf8_to_utf16(language.c_str(), language.size(), &settings.accept_language_list);
	settings.no_sandbox = 1;
	settings.background_color = 0xffffffff;
	CefString(&settings.application_client_id_for_file_scanning)
		.FromString("B476F0FD-9E55-4918-A7B8-A8162FD3ED8B");
	wstring strCachePath = g_strExePath + L"Cache";
	CreateDirectory(strCachePath.c_str(), NULL);
	cef_string_set(strCachePath.c_str(), strCachePath.size(), &settings.cache_path, 1);
	settings.windowless_rendering_enabled = 1;
	settings.multi_threaded_message_loop = 0;
	//settings.single_process = 0;
	scoped_ptr<MainMessageLoop> message_loop;
	message_loop.reset(new MainMessageLoopStd);
	CefInitialize(main_args, settings, app, sandbox_info);
	CPaintManagerUI::SetInstance(hInstance);
	CWndShadow::Initialize(hInstance);
	CMainWnd* pWnd = new CMainWnd;
	pWnd->Create(NULL, SOFT_NAME, UI_WNDSTYLE_FRAME, 0);
	pWnd->CenterWindow();
	pWnd->ShowWindow();
	int result = message_loop->Run();
	CefShutdown();
	message_loop.reset();
	return result;
}

