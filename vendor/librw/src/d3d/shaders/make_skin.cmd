@echo off
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /Fh skin_amb_VS.h skin_VS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /DDIRECTIONALS /Fh skin_amb_dir_VS.h skin_VS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /DDIRECTIONALS /DPOINTLIGHTS /DSPOTLIGHTS /Fh skin_all_VS.h skin_VS.hlsl
