// K380FnLock.cpp : Implement Fn-Lock function on Logitech K380

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
using namespace std;

static const int LOGITECH = 0x046d;
static const int K380 = 0xb342;
static const int TARGET_USAGE = 1;
static const int TARGET_USAGE_PAGE = 65280;

static const unsigned char setNormalMsg[] = { 0x10, 0xff, 0x0b, 0x1e, 0x00, 0x00, 0x00 };
static const unsigned char setSpecialMsg[] = { 0x10, 0xff, 0x0b, 0x1e, 0x01, 0x00, 0x00 };
static const int msgLength = 7;

bool sendMessage(const unsigned char msg[]) {
	int res;
	hid_device* handle;
	struct hid_device_info* devs;
	struct hid_device_info* cur_dev;
	bool result = false;

	res = hid_init();
	devs = hid_enumerate(LOGITECH, K380);
	cur_dev = devs;

	while (cur_dev) {
		if (cur_dev->usage == TARGET_USAGE && cur_dev->usage_page == TARGET_USAGE_PAGE) {
			handle = hid_open_path(cur_dev->path);

			res = hid_write(handle, msg, msgLength);
			if (res != msgLength) {
				printf("error: %ls\n", hid_error(handle));
			}
			hid_close(handle);

			result = res < 0 ? false : true;
			break;
		}
		cur_dev = cur_dev->next;
	}

	hid_free_enumeration(devs);
	hid_exit();
	return result;
}

bool setNormal(void) {
	return sendMessage(setNormalMsg);
}

bool setSpecial(void) {
	return sendMessage(setSpecialMsg);
}

bool detectKeyboard() {
	hid_init();
	return (hid_enumerate(LOGITECH, K380) == NULL) ? false : true;
}

bool detectKeyboard2() {
	// Get Number Of Devices
	UINT nDevices = 0;
	GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST));

	// Allocate Memory For Device List
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	pRawInputDeviceList = new RAWINPUTDEVICELIST[sizeof(RAWINPUTDEVICELIST) * nDevices];

	// Fill Device List Buffer
	int nResult;
	nResult = GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST));

	// Got Device List?
	if (nResult < 0)
	{
		// Clean Up
		delete[] pRawInputDeviceList;

		// Error
		cout << "ERR: Could not get device list.";
		cin.get();
		return 0;
	}

	// Loop Through Device List
	for (UINT i = 0; i < nDevices; i++)
	{
		// Get Character Count For Device Name
		UINT nBufferSize = 0;
		nResult = GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, // Device
			RIDI_DEVICENAME,                // Get Device Name
			NULL,                           // NO Buff, Want Count!
			&nBufferSize);                 // Char Count Here!

		  // Got Device Name?
		if (nResult < 0)
		{
			// Error
			cout << "ERR: Unable to get Device Name character count.. Moving to next device." << endl << endl;

			// Next
			continue;
		}

		// Allocate Memory For Device Name
		WCHAR* wcDeviceName = new WCHAR[nBufferSize + 1];

		// Got Memory
		if (wcDeviceName == NULL)
		{
			// Error
			cout << "ERR: Unable to allocate memory for Device Name.. Moving to next device." << endl << endl;

			// Next
			continue;
		}

		// Get Name
		nResult = GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, // Device
			RIDI_DEVICENAME,                // Get Device Name
			wcDeviceName,                   // Get Name!
			&nBufferSize);                 // Char Count

		  // Got Device Name?
		if (nResult < 0)
		{
			// Error
			cout << "ERR: Unable to get Device Name.. Moving to next device." << endl << endl;

			// Clean Up
			delete[] wcDeviceName;

			// Next
			continue;
		}

		// Set Device Info & Buffer Size
		RID_DEVICE_INFO rdiDeviceInfo;
		rdiDeviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
		nBufferSize = rdiDeviceInfo.cbSize;

		// Get Device Info
		nResult = GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice,
			RIDI_DEVICEINFO,
			&rdiDeviceInfo,
			&nBufferSize);

		// Got All Buffer?
		if (nResult < 0)
		{
			// Error
			cout << "ERR: Unable to read Device Info.. Moving to next device." << endl << endl;

			// Next
			continue;
		}

		// Mouse
		if (rdiDeviceInfo.dwType == RIM_TYPEMOUSE)
		{
			//// Current Device
			//cout << endl << "Displaying device " << i + 1 << " information. (MOUSE)" << endl;
			//wcout << L"Device Name: " << wcDeviceName << endl;
			//cout << "Mouse ID: " << rdiDeviceInfo.mouse.dwId << endl;
			//cout << "Mouse buttons: " << rdiDeviceInfo.mouse.dwNumberOfButtons << endl;
			//cout << "Mouse sample rate (Data Points): " << rdiDeviceInfo.mouse.dwSampleRate << endl;
			//if (rdiDeviceInfo.mouse.fHasHorizontalWheel)
			//{
			//	cout << "Mouse has horizontal wheel" << endl;
			//}
			//else
			//{
			//	cout << "Mouse does not have horizontal wheel" << endl;
			//}
		}

		// Keyboard
		else if (rdiDeviceInfo.dwType == RIM_TYPEKEYBOARD)
		{
			// Current Device
			cout << endl << "Displaying device " << i + 1 << " information. (KEYBOARD)" << endl;
			wcout << L"Device Name: " << wcDeviceName << endl;
			cout << "Keyboard mode: " << rdiDeviceInfo.keyboard.dwKeyboardMode << endl;
			cout << "Number of function keys: " << rdiDeviceInfo.keyboard.dwNumberOfFunctionKeys << endl;
			cout << "Number of indicators: " << rdiDeviceInfo.keyboard.dwNumberOfIndicators << endl;
			cout << "Number of keys total: " << rdiDeviceInfo.keyboard.dwNumberOfKeysTotal << endl;
			cout << "Type of the keyboard: " << rdiDeviceInfo.keyboard.dwType << endl;
			cout << "Subtype of the keyboard: " << rdiDeviceInfo.keyboard.dwSubType << endl;
		}

		// Some HID
		else // (rdi.dwType == RIM_TYPEHID)
		{
			// Current Device
			if (rdiDeviceInfo.hid.dwVendorId == LOGITECH) {
				cout << endl << "Displaying device " << i + 1 << " information. (HID)" << endl;
				wcout << L"Device Name: " << wcDeviceName << endl;
				//cout << "Vendor Id:" << rdiDeviceInfo.hid.dwVendorId << endl;
				//cout << "Product Id:" << rdiDeviceInfo.hid.dwProductId << endl;
				//cout << "Version No:" << rdiDeviceInfo.hid.dwVersionNumber << endl;
				cout << "Usage for the device: " << rdiDeviceInfo.hid.usUsage << endl;
				cout << "Usage Page for the device: " << rdiDeviceInfo.hid.usUsagePage << endl;
			}
		}

		// Delete Name Memory!
		delete[] wcDeviceName;
	}

	// Clean Up - Free Memory
	delete[] pRawInputDeviceList;

	// Exit
	cout << endl << "Finnished.";
	cin.get();
	return 0;
	hid_init();
	return (hid_enumerate(LOGITECH, K380) == NULL) ? false : true;
}


int main(int argc, char* argv[]) {
	//bool success = setNormal();
	//bool locked = false;
	//while (true) {
	//	if (detectKeyboard())
	//		printf("1");
	//	else
	//		printf("2");
	//}

	//while (true) {
	//	detectKeyboard2();
	//}

	//while (true) {
	//	if (detectKeyboard() && !locked) {
	//		setNormal();
	//		locked = true;
	//		printf("Keyboard Connected: Fn Locked\n");
	//	}
	//	else if (!detectKeyboard() && locked) {
	//		locked = false;
	//		printf("Keyboard Disconnected\n");
	//	}
	//	Sleep(500);
	//}

	system("mode con cols=20 lines=1");
	if (detectKeyboard()) {
		if (setNormal()) {
			printf("Fn Lock 성공");
		}
	}
	else {
		printf("K380 연결 안 됨");
	}
	Sleep(300);

	return 0;
}


/**
TODO
키보드 기기 전환 시 연결 끊긴 것으로 인식 안 함
백그라운드 실행(시스템 트레이로 최소화)
아이콘 변경
루프 안 Sleep
*/