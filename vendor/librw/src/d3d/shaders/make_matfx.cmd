@echo off
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /Fh matfx_env_amb_VS.h matfx_env_VS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /DDIRECTIONALS /Fh matfx_env_amb_dir_VS.h matfx_env_VS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T vs_2_0 /DDIRECTIONALS /DPOINTLIGHTS /DSPOTLIGHTS /Fh matfx_env_all_VS.h matfx_env_VS.hlsl

"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T ps_2_0 /Fh matfx_env_PS.h matfx_env_PS.hlsl
"%DXSDK_DIR%\utilities\bin\x86\fxc.exe" /nologo /T ps_2_0 /DTEX /Fh matfx_env_tex_PS.h matfx_env_PS.hlsl
