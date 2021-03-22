/*
 * Media remote framework header.
 *
 * Copyright (c) 2013-2014 Cykey (David Murray)
 * All rights reserved.
 */

#ifndef MEDIAREMOTE_H_
#define MEDIAREMOTE_H_

#include <CoreFoundation/CoreFoundation.h>

#if __cplusplus
extern "C" {
#endif

#pragma mark - Notifications

    /*
     * These are used on the local notification center.
     */

    extern CFStringRef kMRMediaRemoteNowPlayingInfoDidChangeNotification;
    extern CFStringRef kMRMediaRemoteNowPlayingPlaybackQueueDidChangeNotification;
    extern CFStringRef kMRMediaRemotePickableRoutesDidChangeNotification;
    extern CFStringRef kMRMediaRemoteNowPlayingApplicationDidChangeNotification;
    extern CFStringRef kMRMediaRemoteNowPlayingApplicationIsPlayingDidChangeNotification;
    extern CFStringRef kMRMediaRemoteRouteStatusDidChangeNotification;

#pragma mark - Keys

    extern CFStringRef kMRMediaRemoteNowPlayingApplicationPIDUserInfoKey;
    extern CFStringRef kMRMediaRemoteNowPlayingApplicationIsPlayingUserInfoKey;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoAlbum;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoArtist;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoArtworkData;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoArtworkMIMEType;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoChapterNumber;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoComposer;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoDuration;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoElapsedTime;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoGenre;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoIsAdvertisement;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoIsBanned;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoIsInWishList;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoIsLiked;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoIsMusicApp;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoPlaybackRate;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoProhibitsSkip;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoQueueIndex;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoRadioStationIdentifier;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoRepeatMode;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoShuffleMode;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoStartTime;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoSupportsFastForward15Seconds;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoSupportsIsBanned;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoSupportsIsLiked;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoSupportsRewind15Seconds;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoTimestamp;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoTitle;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoTotalChapterCount;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoTotalDiscCount;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoTotalQueueCount;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoTotalTrackCount;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoTrackNumber;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoUniqueIdentifier;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoRadioStationIdentifier;
    extern CFStringRef kMRMediaRemoteNowPlayingInfoRadioStationHash;
    extern CFStringRef kMRMediaRemoteOptionMediaType;
    extern CFStringRef kMRMediaRemoteOptionSourceID;
    extern CFStringRef kMRMediaRemoteOptionTrackID;
    extern CFStringRef kMRMediaRemoteOptionStationID;
    extern CFStringRef kMRMediaRemoteOptionStationHash;
    extern CFStringRef kMRMediaRemoteRouteDescriptionUserInfoKey;
    extern CFStringRef kMRMediaRemoteRouteStatusUserInfoKey;

#pragma mark - API

    typedef enum {
        /*
         * Use nil for userInfo.
         */
        MRMediaRemoteCommandPlay,
        MRMediaRemoteCommandPause,
        MRMediaRemoteCommandTogglePlayPause,
        MRMediaRemoteCommandStop,
        MRMediaRemoteCommandNextTrack,
        MRMediaRemoteCommandPreviousTrack,
        MRMediaRemoteCommandAdvanceShuffleMode,
        MRMediaRemoteCommandAdvanceRepeatMode,
        MRMediaRemoteCommandBeginFastForward,
        MRMediaRemoteCommandEndFastForward,
        MRMediaRemoteCommandBeginRewind,
        MRMediaRemoteCommandEndRewind,
        MRMediaRemoteCommandRewind15Seconds,
        MRMediaRemoteCommandFastForward15Seconds,
        MRMediaRemoteCommandRewind30Seconds,
        MRMediaRemoteCommandFastForward30Seconds,
        MRMediaRemoteCommandToggleRecord,
        MRMediaRemoteCommandSkipForward,
        MRMediaRemoteCommandSkipBackward,
        MRMediaRemoteCommandChangePlaybackRate,

        /*
         * Use a NSDictionary for userInfo, which contains three keys:
         * kMRMediaRemoteOptionTrackID
         * kMRMediaRemoteOptionStationID
         * kMRMediaRemoteOptionStationHash
         */
        MRMediaRemoteCommandRateTrack,
        MRMediaRemoteCommandLikeTrack,
        MRMediaRemoteCommandDislikeTrack,
        MRMediaRemoteCommandBookmarkTrack,

        /*
         * Use nil for userInfo.
         */
        MRMediaRemoteCommandSeekToPlaybackPosition,
        MRMediaRemoteCommandChangeRepeatMode,
        MRMediaRemoteCommandChangeShuffleMode,
        MRMediaRemoteCommandEnableLanguageOption,
        MRMediaRemoteCommandDisableLanguageOption
    } MRMediaRemoteCommand;

    Boolean MRMediaRemoteSendCommand(MRMediaRemoteCommand command, NSDictionary *userInfo);

    void MRMediaRemoteSetPlaybackSpeed(int speed);
    void MRMediaRemoteSetElapsedTime(double elapsedTime);

    void MRMediaRemoteSetNowPlayingApplicationOverrideEnabled(Boolean enabled);

    void MRMediaRemoteRegisterForNowPlayingNotifications(dispatch_queue_t queue);
    void MRMediaRemoteUnregisterForNowPlayingNotifications();

    void MRMediaRemoteBeginRouteDiscovery();
    void MRMediaRemoteEndRouteDiscovery();

    CFArrayRef MRMediaRemoteCopyPickableRoutes();

    typedef void (^MRMediaRemoteGetNowPlayingInfoCompletion)(CFDictionaryRef information);
    typedef void (^MRMediaRemoteGetNowPlayingApplicationPIDCompletion)(int PID);
    typedef void (^MRMediaRemoteGetNowPlayingApplicationIsPlayingCompletion)(Boolean isPlaying);

    void MRMediaRemoteGetNowPlayingApplicationPID(dispatch_queue_t queue, MRMediaRemoteGetNowPlayingApplicationPIDCompletion completion);
    void MRMediaRemoteGetNowPlayingInfo(dispatch_queue_t queue, MRMediaRemoteGetNowPlayingInfoCompletion completion);
    void MRMediaRemoteGetNowPlayingApplicationIsPlaying(dispatch_queue_t queue, MRMediaRemoteGetNowPlayingApplicationIsPlayingCompletion completion);

    void MRMediaRemoteKeepAlive();
    void MRMediaRemoteSetElapsedTime(double time);
    void MRMediaRemoteSetShuffleMode(int mode);
    void MRMediaRemoteSetRepeatMode(int mode);

    /*
     * The identifier can be obtained using MRMediaRemoteCopyPickableRoutes.
     * Use the 'RouteUID' or the 'RouteName' key.
     */

    int MRMediaRemoteSelectSourceWithID(CFStringRef identifier);
    void MRMediaRemoteSetPickedRouteWithPassword(CFStringRef route, CFStringRef password);

    CFArrayRef MRMediaRemoteCopyPickableRoutesForCategory(NSString *category);
    Boolean MRMediaRemotePickedRouteHasVolumeControl();
    Boolean MRMediaRemoteSetCanBeNowPlayingApplication(Boolean can);
    void MRMediaRemoteSetNowPlayingInfo(CFDictionaryRef information);


#if __cplusplus
}
#endif

#endif /* MEDIAREMOTE_H_ */
