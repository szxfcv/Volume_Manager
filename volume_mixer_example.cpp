
#include <Windows.h>
#include <MMDeviceAPI.h>
#include <Mmsystem.h>
#include <Audiopolicy.h>
#include <psapi.h>
#include <iostream>
#include <iomanip>
#include <string>

#pragma comment(lib, "Winmm.lib")
#pragma comment ( lib, "psapi.lib" )


bool has_window(DWORD target_pid)
{
    HWND hwnd = GetTopWindow(NULL);
    do {
        if (GetWindowLongPtr(hwnd, GWLP_HWNDPARENT) != 0) {
            continue;
        }

        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (target_pid == pid) {
            return true;
        }
    } while ((hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) != NULL);

    return false;
}


int main(int argc, char *argv[]) {
    if (argc < 1)return 0;
    for (int i = 1; i < argc; i++) {
        std::cout << argv[i] << std::endl;
    }

    ::CoInitialize(nullptr);
    IMMDeviceEnumerator* deviceEnumerator;
    ::CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    IMMDevice* device;
    deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &device);
    IAudioSessionManager2* sessionManager;
    device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&sessionManager));
    IAudioSessionEnumerator* sessionEnumerator;
    sessionManager->GetSessionEnumerator(&sessionEnumerator);
    unsigned int count;
    sessionEnumerator->GetCount(reinterpret_cast<int*>(&count));

    //�����œn���ꂽ�v���Z�X���ɂ��āA���ʂ��w�肷�鏈��
    if (strcmp(argv[1],"execute")== 0) {
        std::string s = std::string(argv[2]);

        //if (argc > 4)return 0;
        //�v���Z�X����1�P��Ƃ͌���Ȃ�
        if (argc > 4) {
            
            //�v���Z�X����1�P��o�Ȃ��Ƃ��Aargv[2] ����@argv[argc-2]�܂ł̓v���Z�X��
            for (int i = 3; i < argc - 1; i++) {
                s += " ";
                s += argv[i];
            }
            //argv[2] = s.c_str();
        }
        else {
            s = argv[2];
        }
        for (unsigned int i = 0; i < count; i++) {
            IAudioSessionControl* session1;
            sessionEnumerator->GetSession(i, &session1);
            IAudioSessionControl2* session;
            session1->QueryInterface(&session);
            DWORD processId;
            session->GetProcessId(&processId);
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

            // �v���Z�X���̎󂯎��ꏊ
            TCHAR waBaseName[MAX_PATH];
            // �v���Z�X�n���h������v���Z�X�����擾���܂��B
            ::GetModuleBaseName(hProcess, NULL, waBaseName, _countof(waBaseName));
            std::cout << "PID = " << processId << " | ";
            std::string str = "�v���Z�X�� = ";
            std::cout << str;
            // �v���Z�X���̕\��
            std::wcout << waBaseName;
            std::cout << std::endl;
            char cwaBaseName[MAX_PATH];
            WideCharToMultiByte(CP_ACP, 0, waBaseName, -1, cwaBaseName, MAX_PATH, NULL, NULL);
            
            //std::cout << cwaBaseName << argv[2] << std::endl;
            
            //if (strcmp(cwaBaseName, argv[2])==0) {
            if (strcmp(cwaBaseName, s.c_str()) == 0) {
                if (NULL != hProcess) {
                        
                    ISimpleAudioVolume* audioVolume;
                    session->QueryInterface(&audioVolume);
                    float volumeLevel;
                    audioVolume->GetMasterVolume(&volumeLevel);
                    session->QueryInterface(&audioVolume);
                    std::string svolume = argv[argc-1];
                    audioVolume->SetMasterVolume(std::stof(svolume), nullptr);

                    // �v���Z�X�n���h���̃N���[�Y
                    ::CloseHandle(hProcess);

                    audioVolume->Release();
                }
            }
            

            session1->Release();
            session->Release();
        }
    }
    else if (strcmp(argv[1],"info")==0) {
        for (unsigned int i = 0; i < count; i++) {
            IAudioSessionControl* session1;
            sessionEnumerator->GetSession(i, &session1);
            IAudioSessionControl2* session;
            session1->QueryInterface(&session);
            DWORD processId;
            session->GetProcessId(&processId);
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

            if (NULL != hProcess) {

                // �v���Z�X���̎󂯎��ꏊ
                TCHAR waBaseName[MAX_PATH];
                // �v���Z�X�n���h������v���Z�X�����擾���܂��B
                ::GetModuleBaseName(hProcess, NULL, waBaseName, _countof(waBaseName));
                std::cout << "PID = " << processId << " | ";
                std::string str = "�v���Z�X�� = ";
                std::cout << str;
                // �v���Z�X���̕\��
                std::wcout << waBaseName << " | ";
                ISimpleAudioVolume* audioVolume;
                session->QueryInterface(&audioVolume);
                float volumeLevel;
                audioVolume->GetMasterVolume(&volumeLevel);
                std::cout << "���� : " << std::fixed << std::setprecision(2) << volumeLevel << std::endl;
                // �v���Z�X�n���h���̃N���[�Y
                ::CloseHandle(hProcess);
                audioVolume->Release();

            }
            session1->Release();
            session->Release();
        }
    }
    else {

    }


        


    


/*
    for (unsigned int i = 0; i < count; i++) {
        IAudioSessionControl* session1;
        sessionEnumerator->GetSession(i, &session1);
        IAudioSessionControl2* session;
        session1->QueryInterface(&session);
        DWORD processId;
        session->GetProcessId(&processId);
        if (processId != ::GetCurrentProcessId()) {
            continue;
        }
        ISimpleAudioVolume* audioVolume;
        session->QueryInterface(&audioVolume);
        unsigned int volume = 0;
        while (true) {
            if (volume <= 100) {
                audioVolume->SetMasterVolume(static_cast<float>(volume) / 100, nullptr);
            }
            else if (volume < 200) {
                audioVolume->SetMasterVolume(static_cast<float>(200 - volume) / 100, nullptr);
            }
            else {
                volume = 0;
            }
            ::Sleep(10);
            volume++;
        }
    }*/
    return 0;
}
