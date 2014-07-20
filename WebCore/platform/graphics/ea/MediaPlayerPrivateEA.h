/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2011, 2012 Electronic Arts, Inc.  All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MediaPlayerPrivateEA_h
#define MediaPlayerPrivateEA_h

#include "MediaPlayerPrivate.h"
#include <EAWebKit/EAWebKitClient.h>
#include "MediaPlayer.h"

namespace EA
{
    namespace WebKit
    {
        class View;        
    }
}

namespace WebCore
{

class IntRect;
class IntSize;




class MediaPlayerPrivateEA : public MediaPlayerPrivateInterface
{
public:
    ~MediaPlayerPrivateEA();

    // For setting up the engine
    static void registerMediaEngine(MediaEngineRegistrar);
    static PassOwnPtr<MediaPlayerPrivateInterface> create(MediaPlayer* player);
    static void getSupportedTypes(HashSet<String> &supported);
    static MediaPlayer::SupportsType supportsType(const String& mime, const String& codec);

    // For playing the media
    virtual void load(const String& url);
    virtual void cancelLoad();
    virtual void play(); 
    virtual void pause();    
    virtual bool supportsFullscreen() const;
    virtual IntSize naturalSize() const;
    virtual bool hasVideo() const;
    virtual bool hasAudio() const;
    virtual void setVisible(bool);
    virtual float duration() const;
    virtual float currentTime() const;
    virtual void seek(float time);
    virtual bool seeking() const;
    virtual float startTime() const;
    virtual double initialTime() const;
    virtual void setRate(float);
    virtual bool paused() const;
    virtual void setVolume(float);
    virtual bool supportsMuting() const;
    virtual void setMuted(bool);
    virtual MediaPlayer::NetworkState networkState() const; 
    virtual MediaPlayer::ReadyState readyState() const; 
    virtual float maxTimeSeekable() const;
    virtual PassRefPtr<TimeRanges> buffered() const; 
    virtual unsigned bytesLoaded() const;
    virtual void setSize(const IntSize& );
    virtual void paint(GraphicsContext*, const IntRect&);

    // Ignore these for now as unclear if needed.
    virtual bool supportsSave() const { return false; }
    virtual void prepareToPlay() { }
    virtual void setPreservesPitch(bool) { }
    virtual bool hasClosedCaptions() const { return false; }    
    virtual void setClosedCaptionsVisible(bool) { }
    
    void UpdateTimerFired(Timer<MediaPlayerPrivateEA>* );

private:
    MediaPlayerPrivateEA(MediaPlayer* player);   
    static EA::WebKit::MediaUpdateInfo& GetMediaUpdateInfo(void);
    static void Finalize(void);
    void ClientUpdate(EA::WebKit::MediaUpdateInfo::UpdateType type) const;
    bool ClientUpdateAndReturnBool(EA::WebKit::MediaUpdateInfo::UpdateType type) const;
    float ClientUpdateAndReturnTime(EA::WebKit::MediaUpdateInfo::UpdateType type) const;
    EA::WebKit::MediaUpdateInfo::MediaState ClientMediaState(void) const;
    void ClientUpdateStates(void);


    MediaPlayer*                mpWebCorePlayer;
    EA::WebKit::View*           mpEAWebKitView;
    int                         mHandleID;          // Unique handle ID
    bool                        mIsVisible;         // Store to avoid repetitive calls to client
    bool                        mIsPlaying;         // Store if play was sent
    bool                        mIsFinished;        // Media finished playing.    
    bool                        mIsLooping;         // Store if looping set
    bool                        mIsVideo;           // Set if using the video tag.
    IntSize                     mSize;              // The movie rect size.
    IntSize                     mNaturalSize;       // The natural movie size (used for aspect ratio)
    IntRect                     mMovieRect;         // The last movie position used to detect changes for client notification.
    IntRect                     mWindowRect;        // The last movie window position (clip rect) used to detect changes for client notification.    
    MediaPlayer::ReadyState     mReadyState;        // Current ready state.    
    MediaPlayer::NetworkState   mNetworkState;      // Current network state.
    EA::WebKit::MediaUpdateInfo::MediaState mMediaState; // Current media state.    
    Timer<MediaPlayerPrivateEA> mUpdateTimer;       // Used to control the repaint and check on status.

    static int sIdGenerator;                        // Used to assign unique handle ID's to each movie instance.
    static int sRefCount;                           // For auto cleanup
    static EA::WebKit::MediaUpdateInfo* spMediaUpdateInfo;    // A shared media info struct for callback updates 
};


} // WebCore


#endif // MediaPlayerPrivateEA_h
