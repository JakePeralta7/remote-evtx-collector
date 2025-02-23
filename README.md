# remote-evtx-collector

## Build
```
gcc .\src\remote_evtx_collector.c -o remote_evtx_collector_x64.exe -s -m64 -lwevtapi
```

## Usage
```
remote_evtx_collector_x64.exe <remote_computer> <channel_name> <output_file>
```
