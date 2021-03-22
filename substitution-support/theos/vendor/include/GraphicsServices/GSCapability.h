/*

GSCapability.h ... Graphics Service Capability.
 
Copyright (c) 2009, KennyTM~
All rights reserved.
 
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the KennyTM~ nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
*/

#ifndef GSCAPABILITY_H
#define GSCAPABILITY_H

#include <CoreFoundation/CoreFoundation.h>
#include <Availability2.h>

#if __cplusplus
extern "C" {
#endif

	/*! @brief Get the raw value of a key in the capability plist.
	 The capability plist can be read from a shared memory region named "GSCapability"
	 */
	CFPropertyListRef _getCapability(CFStringRef capability);
	
	CFStringRef GSGetLocalizedDeviceName();	///< Get the localized device name (which is the "device-name-localized" capability).
	CFStringRef GSGetDeviceName();	///< Get the device name (which is the "device-name" capability).
	Boolean GSSystemHasCapability(CFStringRef capability);	///< Check if the system has the specified capability.
	
	
	Boolean GSSystemCanTakePhoto();	///< Returns if the device can take photos (i.e. have "still-camera" but not "cameraRestriction" capabilities).
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	Boolean GSSystemHasTelephonyCapability();	///< Returns if the device has "telephony" capability.
#endif
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_2
	Boolean GSSystemGetCellularDataCapability(void);	///< Returns if the device has "cellular-data" capability.
	Boolean GSSystemGetTelephonyCapability(void);	///< Returns if the device has "telephony" capability.
#endif
	
	/// If the capability is a dictionary (e.g. the "screen-dimensions" capability), copy the value of a key in that dictionary.
	CFPropertyListRef GSSystemCopySubcapability(CFStringRef capability, CFStringRef subcapability);
	CFPropertyListRef GSSystemCopyCapability(CFStringRef capability);	///< Copy the value of a capability.
	
	extern CFStringRef kGSCapabilityChangedNotification;

	extern CFStringRef kGSCameraRestriction;
	extern CFStringRef kGSInAppPurchasesRestriction;
	extern CFStringRef kGSVolumeLimitRestriction;

	extern CFStringRef kGSDeviceNameString;
	extern CFStringRef kGSLocalizedDeviceNameString;

	extern CFStringRef kGSTelephonyMaximumGeneration;
	
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_1
	extern CFStringRef kGSARMV6ExecutionCapability;
	extern CFStringRef kGSARMV7ExecutionCapability;
#endif
	extern CFStringRef kGSAccelerometerCapability;
	extern CFStringRef kGSAccessibilityCapability;
	extern CFStringRef kGSAppleInternalInstallCapability;
	extern CFStringRef kGSApplicationInstallationCapability;
	extern CFStringRef kGSAutoFocusCameraCapability;
	extern CFStringRef kGSBluetoothCapability;
	extern CFStringRef kGSCameraCapability;
	extern CFStringRef kGSDelaySleepForHeadsetClickCapability;
	extern CFStringRef kGSDisplayFCCLogosViaSoftwareCapability;
	extern CFStringRef kGSDisplayIdentifiersCapability;
	extern CFStringRef kGSEncodeAACCapability;
	extern CFStringRef kGSEncryptedDataPartitionCapability;
	extern CFStringRef kGSExplicitContentRestriction;
	extern CFStringRef kGSGPSCapability;
	extern CFStringRef kGSGasGaugeBatteryCapability;
	extern CFStringRef kGSGreenTeaDeviceCapability;
	extern CFStringRef kGSHasAllFeaturesCapability;
	extern CFStringRef kGSInternationalSettingsCapability;
	extern CFStringRef kGSLaunchApplicationsWhileAnimatingCapability;
	extern CFStringRef kGSLoadThumbnailsWhileScrollingCapability;
	extern CFStringRef kGSLocationServicesCapability;
	extern CFStringRef kGSMMSCapability;
	extern CFStringRef kGSMagnetometerCapability;
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_1
	extern CFStringRef kGSMarketingNameString;
#endif
	extern CFStringRef kGSMicrophoneCapability;
	extern CFStringRef kGSNikeIpodCapability;
	extern CFStringRef kGSNotGreenTeaDeviceCapability;
	extern CFStringRef kGSOpenGLES1Capability;
	extern CFStringRef kGSOpenGLES2Capability;
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_1
	extern CFStringRef kGSPeer2PeerCapability;
#endif
	extern CFStringRef kGSPiezoClickerCapability;
	extern CFStringRef kGSPlatformStandAloneContactsCapability;
	extern CFStringRef kGSProximitySensorCapability;
	extern CFStringRef kGSRingerSwitchCapability;
	extern CFStringRef kGSSMSCapability;
	extern CFStringRef kGSScreenDimensionsCapability;
	extern CFStringRef kGSSensitiveUICapability;
	extern CFStringRef kGSTVOutSettingsCapability;
	extern CFStringRef kGSTelephonyCapability;
	extern CFStringRef kGSUnifiedIPodCapability;
	extern CFStringRef kGSVideoCameraCapability;
	extern CFStringRef kGSVoiceControlCapability;
	extern CFStringRef kGSVolumeButtonCapability;
	extern CFStringRef kGSWiFiCapability;
	extern CFStringRef kGSYouTubeCapability;
	extern CFStringRef kGSYouTubePluginCapability;
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_2
	extern CFStringRef kGS720pPlaybackCapability;
	extern CFStringRef kGSCellularDataCapability;
	extern CFStringRef kGSContainsCellularRadioCapability;
	extern CFStringRef kGSDataPlanCapability;
	extern CFStringRef kGSDisplayPortCapability;
	extern CFStringRef kGSH264EncoderCapability;
	extern CFStringRef kGSHideNonDefaultApplicationsCapability;
	extern CFStringRef kGSWildcatCapability;
#endif
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_4_0
	extern CFStringRef kGSCameraFlashCapability;
	extern CFStringRef kGSCanRasterizeEfficientlyCapability;
	extern CFStringRef kGSFrontFacingCameraCapability;
	extern CFStringRef kGSHiDPICapability;
	extern CFStringRef kGSIOSurfaceBackedImagesCapability;
	extern CFStringRef kGSMultitaskingCapability;
	extern CFStringRef kGSVeniceCapability;
#endif
	
	/// Subcapabilities
	extern CFStringRef kGSMainScreenHeight;
	extern CFStringRef kGSMainScreenOrientation;
	extern CFStringRef kGSMainScreenScale;
	extern CFStringRef kGSMainScreenWidth;	
	
	extern CFStringRef kGSEnforceGoogleMail;
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_2
	extern CFStringRef kGSEnforceCameraShutterClick;
#endif
	
#if __cplusplus
}
#endif

#endif