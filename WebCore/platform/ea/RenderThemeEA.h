/*
 * This file is part of the theme implementation for form controls in WebCore.
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2011, 2012, 2013 Electronic Arts, Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */
#ifndef RenderThemeEA_h
#define RenderThemeEA_h

#include "RenderTheme.h"

namespace WebCore {

#if ENABLE(PROGRESS_TAG)
class RenderProgress;
#endif
class RenderStyle;
class HTMLMediaElement;

class RenderThemeEA : public RenderTheme {
private:
    RenderThemeEA(Page* page);
    virtual ~RenderThemeEA();

public:
    
    static PassRefPtr<RenderTheme> create(Page*);

    String extraDefaultStyleSheet();
    
    void addIntrinsicMargins(RenderStyle* style) const;

    virtual bool supportsHover(const RenderStyle*) const;
    virtual bool supportsFocusRing(const RenderStyle* style) const;

    virtual int baselinePosition(const RenderObject* o) const;

    // A method asking if the control changes its tint when the window has focus or not.
    virtual bool controlSupportsTints(const RenderObject*) const;

    // A general method asking if any control tinting is supported at all.
    virtual bool supportsControlTints() const;

    // The platform selection color.
    virtual Color platformActiveSelectionBackgroundColor() const;
    virtual Color platformInactiveSelectionBackgroundColor() const;
    virtual Color platformActiveSelectionForegroundColor() const;
    virtual Color platformInactiveSelectionForegroundColor() const;
    virtual Color platformTextSearchHighlightColor() const;
    virtual Color platformActiveListBoxSelectionBackgroundColor() const;
    virtual Color platformInactiveListBoxSelectionBackgroundColor() const;
    virtual Color platformActiveListBoxSelectionForegroundColor() const;
    virtual Color platformInactiveListBoxSelectionForegroundColor() const;

    virtual void systemFont(int propId, FontDescription&) const;

    virtual int minimumMenuListSize(RenderStyle*) const;

    virtual void adjustSliderThumbSize(RenderStyle*) const;
#if ENABLE(VIDEO)
    virtual IntPoint volumeSliderOffsetFromMuteButton(RenderBox* muteButtonBox, const IntSize& size) const;
    virtual bool usesMediaControlVolumeSlider() const { return false; }
    virtual String extraMediaControlsStyleSheet();
#endif
protected:
    virtual bool paintCheckbox(RenderObject* o, const PaintInfo& i, const IntRect& r);
    virtual void setCheckboxSize(RenderStyle*) const;

    virtual bool paintRadio(RenderObject* o, const PaintInfo& i, const IntRect& r);
    virtual void setRadioSize(RenderStyle*) const;

    virtual void adjustButtonStyle(CSSStyleSelector*, RenderStyle*, Element*) const;
    virtual bool paintButton(RenderObject*, const PaintInfo&, const IntRect&);

    virtual bool paintTextField(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void adjustTextFieldStyle(CSSStyleSelector*, RenderStyle*, Element*) const;

    virtual bool paintTextArea(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void adjustTextAreaStyle(CSSStyleSelector*, RenderStyle*, Element*) const;

    virtual bool paintMenuList(RenderObject* o, const PaintInfo& i, const IntRect& r);
    virtual void adjustMenuListStyle(CSSStyleSelector*, RenderStyle*, Element*) const;

    virtual bool paintMenuListButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void adjustMenuListButtonStyle(CSSStyleSelector*, RenderStyle*, Element*) const;

#if ENABLE(PROGRESS_TAG)
    virtual void adjustProgressBarStyle(CSSStyleSelector*, RenderStyle*, Element*) const;
    virtual bool paintProgressBar(RenderObject*, const PaintInfo&, const IntRect&);
#endif

    virtual bool paintSliderTrack(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void adjustSliderTrackStyle(CSSStyleSelector*, RenderStyle*, Element*) const;

    virtual bool paintSliderThumb(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void adjustSliderThumbStyle(CSSStyleSelector*, RenderStyle*, Element*) const;

    virtual bool paintSearchField(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void adjustSearchFieldStyle(CSSStyleSelector*, RenderStyle*, Element*) const;

    virtual void adjustSearchFieldCancelButtonStyle(CSSStyleSelector*, RenderStyle*, Element*) const;
    virtual bool paintSearchFieldCancelButton(RenderObject*, const PaintInfo&, const IntRect&);

    virtual void adjustSearchFieldDecorationStyle(CSSStyleSelector*, RenderStyle*, Element*) const;
    virtual bool paintSearchFieldDecoration(RenderObject*, const PaintInfo&, const IntRect&);

    virtual void adjustSearchFieldResultsDecorationStyle(CSSStyleSelector*, RenderStyle*, Element*) const;
    virtual bool paintSearchFieldResultsDecoration(RenderObject*, const PaintInfo&, const IntRect&);

 
    virtual bool paintMediaFullscreenButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaPlayButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaMuteButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaSeekBackButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaSeekForwardButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaSliderTrack(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaSliderThumb(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaVolumeSliderContainer(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaVolumeSliderTrack(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaVolumeSliderThumb(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaRewindButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaReturnToRealtimeButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaToggleClosedCaptionsButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaControlsBackground(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaCurrentTime(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintMediaTimeRemaining(RenderObject*, const PaintInfo&, const IntRect&);

    double caretBlinkFrequency();

#if ENABLE(PROGRESS_TAG)
    // Returns the repeat interval of the animation for the progress bar.
    virtual double animationRepeatIntervalForProgressBar(RenderProgress* renderProgress) const;
    // Returns the duration of the animation for the progress bar.
    virtual double animationDurationForProgressBar(RenderProgress* renderProgress) const;
#endif

private:
	AtomicString m_buttonFontFamily;
};


}

#endif // RenderThemeEA_h
