@echo off
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /Fh default_amb_VS.h default_VS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /DDIRECTIONALS /Fh default_amb_dir_VS.h default_VS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /DDIRECTIONALS /DPOINTLIGHTS /DSPOTLIGHTS /Fh default_all_VS.h default_VS.hlsl

"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T ps_2_0 /Fh default_PS.h default_PS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T ps_2_0 /DTEX /Fh default_tex_PS.h default_PS.hlsl

"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /Fh im2d_VS.h im2d_VS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T ps_2_0 /Fh im2d_PS.h im2d_PS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T ps_2_0 /DTEX /Fh im2d_tex_PS.h im2d_PS.hlsl
