#include <windows.h>
#include <winevt.h>
#include <stdio.h>
#include <ntstatus.h>

#pragma comment(lib, "wevtapi.lib")

LPCWSTR ConvertToLPCWSTR(const char* str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    wchar_t* wstr = (wchar_t*)malloc(size_needed * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, size_needed);
    return wstr;
}

NTSTATUS ExportEventLog(LPCWSTR remoteComputer, LPCWSTR channelName, LPCWSTR outputPath) {
    EVT_RPC_LOGIN rpcLogin = {0};
    rpcLogin.Server = (LPWSTR)remoteComputer;
    rpcLogin.Flags = EvtRpcLoginAuthDefault;
    NTSTATUS status = STATUS_SUCCESS;

    printf("[+] Opening session to: %S\n", remoteComputer);
    EVT_HANDLE hSession = EvtOpenSession(EvtRpcLogin, &rpcLogin, 0, 0);
    if (hSession == NULL) {
        printf("Failed to open session, error %d", GetLastError());
        status = STATUS_UNSUCCESSFUL;
        goto cleanup;
    }

    printf("[+] Opening event log: %S\n", channelName);
    EVT_HANDLE hEventLog = EvtOpenLog(hSession, channelName, EvtOpenChannelPath);
    if (hEventLog == NULL) {
        printf("Failed to open event log, error %d", GetLastError());
        status = STATUS_UNSUCCESSFUL;
        goto cleanup;
    }

    printf("[+] Exporting event log\n");
    if (!EvtExportLog(hSession, channelName, NULL, outputPath, EvtExportLogChannelPath)) {
        printf("Failed to export event log, error %d", GetLastError());
        status = STATUS_UNSUCCESSFUL;
        goto cleanup;
    }

    printf("[+] Event log exported successfully to %S\n", outputPath);

cleanup:
    if (hEventLog) EvtClose(hEventLog);
    if (hSession) EvtClose(hSession);
    
    free((void*)remoteComputer);
    free((void*)channelName);
    free((void*)outputPath);

    return status;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <remote_computer> <channel_name> <output_path>\n", argv[0]);
        return 1;
    }

    LPCWSTR remoteComputer = ConvertToLPCWSTR(argv[1]);
    LPCWSTR channelName = ConvertToLPCWSTR(argv[2]);
    LPCWSTR outputPath = ConvertToLPCWSTR(argv[3]);

    NTSTATUS status = ExportEventLog(remoteComputer, channelName, outputPath);
    
    return status;
}
