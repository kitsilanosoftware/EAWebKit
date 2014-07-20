/*
 * This file is part of the popup menu implementation for <select> elements in WebCore.
 *
 * Copyright (C) 2008, 2009, 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Coypright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
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

#include "config.h"
#include "PopupMenuEA.h"

#include "ChromeClientEA.h"
#include "FrameView.h"
#include "PopupMenuClient.h"
#include "PlatformScreen.h"
#include "PageClientEA.h"
#include "page.h"
#include "chrome.h"
#include "webframe.h"
#include "IntRect.h"
#include "RenderText.h"
#include "GraphicsContext.h"
#include "RenderThemeEA.h"
#include "ScrollBar.h"
#include "RefPtrCairo.h"
#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitView.h>
#include <EAWebKit/EAWebKitSurface.h>
#include <EAWebKit/EAWebKitInput.h>
#include <internal/include/EAWebkit_p.h>
#include "TextRun.h"

namespace ThemeEA
{
	const int kPopUpScrollWidth         = 10;   // Get this from the scroll itself?
	const int kScrollMinItems           = 3;    // Seems we need about 45 pixels for a decent scroll bar draw. 
	const int kPopupBorderWidth         = 1;    // Popup borderWidth. 
	const uint32_t kListBoxBorderColor  = 0xFF808080; // List box border color

	extern const int kScrollThickness;
	extern const int kPopupMenuBorderPadding;
}

namespace WebCore {

PopupMenuEA::PopupMenuEA(PopupMenuClient* client, const ChromeClientEA* chromeClient)
    : m_popupClient(client)
    , m_chromeClient(chromeClient)
    , m_itemCount(0)
    , m_itemScrollCount(0)
    , m_itemScrollOffset(0)
    , m_scrollOffset(0)
    , m_itemHeight(0)
    , m_focusedIndex(0)
    , m_windowRect()
    , m_scrollRect()
    , m_mouseScrollRect()
    , m_flipped(false)
    , m_scrollBarActive(false)
    , m_scrollbarCapturingMouse(false)
    , m_scrollbarWasUnderMouse(false)
    , m_viewEA(0)
    , m_surface(0)
{
    m_scrollBar = Scrollbar::createNativeScrollbar( this, VerticalScrollbar , RegularScrollbar);
}

PopupMenuEA::~PopupMenuEA(void)
{
    hide();  
}

void PopupMenuEA::disconnectClient(void)
{
    m_popupClient = 0;
    hide();
}

void PopupMenuEA::show(const IntRect& rect, FrameView* view, int index)
{
    if (!m_popupClient)
        return;
    
    EAW_ASSERT(!m_surface);

    m_listBoxRect = rect;
    m_frameView = view;
    m_focusedIndex = index;
    m_scrollOffset = 0;

     // Get the EA::WebKit::View
    Frame* pFrame = view->frame();
    if(!pFrame) 
        return;
    EA::WebKit::WebFrame* pWebFrame = EA::WebKit::WebFramePrivate::kit(pFrame);
    m_viewEA = pWebFrame->page()->view();
    EAW_ASSERT(m_viewEA);

    if(!layout())
        return;

    // Create the popup surface
    m_surface = m_viewEA->CreateOverlaySurface(m_windowRect.x(),m_windowRect.y(),m_windowRect.width(),m_windowRect.height());

    // Overlay input
    m_viewEA->SetOverlayInputClient(this);

    draw();
}

void PopupMenuEA::hide(void)
{
    if(m_surface)
    {
        if(m_viewEA)
			m_viewEA->DestroyOverlaySurface(m_surface);
        m_surface = 0;
    }   
  
    // abaldeva: We set the View to be NULL here. Otherwise, when WebCore releases its cached pages, it ends up calling 
	// this. If that page was loaded in a View that is now gone, we crash here.
	if(m_viewEA)
	{
		m_viewEA->SetOverlayInputClient(0);
		m_viewEA = NULL;
	}


    if(m_scrollbarCapturingMouse)
    {    
        // We need to send a mouse release notification so that the scroll system can know not to track the thumb.
        m_scrollBar->mouseUp();
        m_scrollbarCapturingMouse = false;
    }


    if(m_popupClient)
        m_popupClient->popupDidHide();  // This actually set the disable flag
}

// This could come from the layout or JS. We override the input now so don't expect keyboard updates here.
void PopupMenuEA::updateFromElement(void)
{
    if(!m_popupClient)
        return;

    // Check if the list size changed.
    int itemCount = m_popupClient->listSize();
    if(itemCount != m_itemCount)
    {
        if(m_surface)
        {
            m_viewEA->DestroyOverlaySurface(m_surface);
            m_surface = 0;
        }   

        m_focusedIndex = m_popupClient->selectedIndex();
        
        if(!layout())
        {
            hide();
            return;
        }
        // Create the popup surface
        m_surface = m_viewEA->CreateOverlaySurface(m_windowRect.x(),m_windowRect.y(),m_windowRect.width(),m_windowRect.height());
        m_focusedIndex = m_popupClient->selectedIndex();
        draw();        
    }
    else
    {
        const int index = m_popupClient->selectedIndex();

        if(m_focusedIndex != index)
        {
            m_focusedIndex = index;
            draw();
        }
    }
}

bool PopupMenuEA::layout(void)
{
    // The input rect m_listBoxRect is a rect of the inline popup menu element. It consists of some text and a
    // dropdown arrow on the right. The popup menu we are implementing here is a menu of text 
    // strings that displays directly below this inline element.
    EAW_ASSERT(m_frameView);

    m_itemScrollOffset = 0;
    m_scrollbarCapturingMouse = false;
    m_flipped = false;
    m_itemCount = m_popupClient->listSize();
    if((m_itemCount <= 0) || (!m_frameView))
        return false;
      
    int itemWidth = 0;
    int itemHeight = 0;

    // Find the max width/height for all the items.
    for(int i = 0; i < m_itemCount; ++i)
    {
        String text = m_popupClient->itemText(i);

        if(!text.isEmpty())
        {
            PopupMenuStyle menuStyle = m_popupClient->itemStyle(i);
            const WebCore::Font& font = menuStyle.font();            
            
            int fontHeight =  menuStyle.font().fontMetrics().height();
            int fontWidth;    
            const TextRun textRun(text.characters(), text.length());           

            if(m_popupClient->itemIsLabel(i))// Make Labels bolder than regular items in order to distinguish them. Search for optgroup tag on internet.
            {
                WebCore::FontDescription d = font.fontDescription();  
                d.setWeight(d.bolderWeight());
                WebCore::Font itemFont = WebCore::Font(d, font.letterSpacing(), font.wordSpacing());
                itemFont.update(m_popupClient->fontSelector());
                fontWidth = (int) itemFont.width(textRun);
            }
            else
            {
                fontWidth = (int) font.width(textRun);   
            }
            itemWidth = max(itemWidth, fontWidth);
            itemHeight = max(itemHeight, fontHeight); 
        }
    }

    // Set up the width (this needs to be done before the scroll bar calc).  
	const int popupScrollThickness = ThemeEA::kScrollThickness;    // Could consider another param
    int popupWidth = itemWidth + popupScrollThickness;
    if(popupWidth < m_listBoxRect.width())
        popupWidth = m_listBoxRect.width();

    // Set up the height of popup
    if (itemHeight <= 0)
        itemHeight = m_listBoxRect.height();    // Use list box height if no height was found (e.g no text in box).                 
    m_itemHeight = itemHeight;                  
    int popupHeight = itemHeight * m_itemCount;

    // Starting x,y of popup
    const IntRect listBoxScreenCoords(m_frameView->contentsToWindow(m_listBoxRect.location()), m_listBoxRect.size());
    int popupX = listBoxScreenCoords.x();
    int popupY = listBoxScreenCoords.y() + m_listBoxRect.height();

    // Set up popup location and a scroll bar if needed based on screen position.
    EA::WebKit::IntSize screenSize = m_viewEA->GetSize();
    const int deltaBottom = screenSize.mHeight - (listBoxScreenCoords.y() + m_listBoxRect.height());
    const int deltaTop  = listBoxScreenCoords.y();
    int deltaY;
    
    // Check if need to flip popup on top  
    if((deltaBottom > popupHeight) || (deltaBottom >= deltaTop))
    {
        deltaY =  deltaBottom;
        m_flipped = false;
    }
    else
    {
        deltaY =  deltaTop;
        m_flipped = true;
    }    

    if(deltaY < popupHeight)
    {
        // We can't fit all on screen so will need to scroll or flip.
        int scrollCount = 1; 
        if(m_itemHeight)
        {
            scrollCount = deltaY / m_itemHeight; 
			if(scrollCount > ThemeEA::kScrollMinItems)
                scrollCount--;  // One less or too close to the bottom
            else if(scrollCount <= 0)
                scrollCount = 1;    // A min of 1 element visible
        }
        popupHeight = m_itemHeight * scrollCount;
        m_itemScrollCount = scrollCount;
        
        // Set up the scroll bar
        // In frame view space
        m_scrollRect.setX(m_frameView->x() + popupWidth - popupScrollThickness - ThemeEA::kPopupBorderWidth);    
        m_scrollRect.setY(m_frameView->y());
        m_scrollRect.setWidth(popupScrollThickness);
        m_scrollRect.setHeight(popupHeight);
        
        // In screen space got mouse detection
        m_mouseScrollRect.setX(popupX + popupWidth - popupScrollThickness - ThemeEA::kPopupBorderWidth);
        if(!m_flipped)
            m_mouseScrollRect.setY(popupY);
        else
             m_mouseScrollRect.setY(popupY - (m_listBoxRect.height() + popupHeight));    
        m_mouseScrollRect.setWidth(popupScrollThickness + ThemeEA::kPopupBorderWidth);
        m_mouseScrollRect.setHeight(popupHeight);
        
        // In popup surface space
        IntRect localScrollRect( (popupWidth - popupScrollThickness - ThemeEA::kPopupBorderWidth), 0, popupScrollThickness, popupHeight);
        m_scrollBar->setFrameRect(localScrollRect);
        m_scrollBar->setSteps(1, std::min(1, m_itemScrollOffset -1), popupHeight);
        m_scrollBar->setProportion(m_itemScrollCount, m_itemCount);
        m_scrollBar->setEnabled(true);
        m_scrollBarActive = true;

        // Find starting scroll offset as the starting focused index might not be 0
        
        int requestedScrollIndex = 0;
        if( m_focusedIndex < m_itemScrollOffset)
        {
            requestedScrollIndex = m_focusedIndex;
            updateScrollOffset(requestedScrollIndex);
        }
        else if (m_focusedIndex >= (m_itemScrollOffset + m_itemScrollCount))
        {
            requestedScrollIndex = m_focusedIndex - m_itemScrollCount + 1; 
            updateScrollOffset(requestedScrollIndex);
        }
       
        ScrollableArea::scrollToYOffsetWithoutAnimation(requestedScrollIndex);
    }
    else
    {
        m_scrollBarActive = false;
        if(m_scrollBar.get())
            m_scrollBar->setEnabled(false);
    }

     // Adjust the final popup rect with height info
     if(m_flipped)
        popupY -= (m_listBoxRect.height() + popupHeight);    

     // Store the popup rect in screen space
     m_windowRect.setX(popupX);
     m_windowRect.setY(popupY);
     m_windowRect.setWidth(popupWidth);
     m_windowRect.setHeight(popupHeight);
  
     return true;
}

void PopupMenuEA::draw(void)
{
    if(!m_surface)
        return;

    // Set up the draw context. Popups draw to their own surface which then gets pasted on top of the view after the frame render. 
    EA::WebKit::ISurface::SurfaceDescriptor surfaceDescriptor = {0};
    m_surface->Lock(&surfaceDescriptor);    

    int surfaceW = 0;
    int surfaceH = 0;
    m_surface->GetContentDimensions(&surfaceW,&surfaceH);

    RefPtr<cairo_surface_t> surface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, surfaceW, surfaceH, surfaceDescriptor.mStride));    
	RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(surface.get()));
	WebCore::GraphicsContext context(cairoContext.get());

    // Fill in the default background by sampling any unfocused index to find the CSS default unfocused color.
    int sampleIndex = 0;
    if((sampleIndex == m_focusedIndex) && (m_itemCount > 1))
        sampleIndex++;
    PopupMenuStyle sampleStyle = m_popupClient->itemStyle(sampleIndex);
    Color defaultBgColor(sampleStyle.backgroundColor()); 
    if((int) defaultBgColor.rgb() == 0)             
        defaultBgColor.setRGB(EA::WebKit::GetThemeParameters().mColorInactiveListBoxSelectionBack);
    context.setFillColor(defaultBgColor, ColorSpaceDeviceRGB);
    const IntRect windowRect(0,0,m_windowRect.width(),m_windowRect.height());
    context.fillRect(windowRect);

    // Draw the visible items
    int indexOffset = 0;
    int count = m_itemCount; 
    if(m_scrollBarActive)
    {
        indexOffset = m_itemScrollOffset;
        count = indexOffset + m_itemScrollCount; 
    }

    for(int i = indexOffset; i < count; i++)
    {
        const bool bHighlighted(i == m_focusedIndex);
        Color bgColor; 
        Color textColor; 
        PopupMenuStyle itemStyle = m_popupClient->itemStyle(i);

        if(bHighlighted)
        {
            bgColor = EA::WebKit::GetThemeParameters().mColorActiveListBoxSelectionBack;
            textColor = EA::WebKit::GetThemeParameters().mColorActiveListBoxSelectionFore;
        }
        else
        {
            // Grab the CSS values but use our defaults if 0
            bgColor = itemStyle.backgroundColor();   
            if((int)bgColor.rgb() == 0)
                defaultBgColor.setRGB(EA::WebKit::GetThemeParameters().mColorInactiveListBoxSelectionBack);
            
            textColor = itemStyle.foregroundColor();   
            if((int) textColor.rgb() == 0)
                textColor.setRGB(EA::WebKit::GetThemeParameters().mColorInactiveListBoxSelectionFore);
        }
        
        // Build the item rect
        const int topY = (i - indexOffset) * m_itemHeight;
        const IntRect itemRect(ThemeEA::kPopupBorderWidth, topY, m_windowRect.width() - ThemeEA::kPopupBorderWidth, m_itemHeight);       
       
        // Only fill if we have something different for this item since we already filled the full background with the estimated default
        if(bgColor != defaultBgColor)
        {
            context.setFillColor(bgColor, ColorSpaceDeviceRGB);    
            context.fillRect(itemRect);
        }
    
        // Draw the text
        const Font& font = itemStyle.font();
        const FontMetrics& fontMetrics = font.fontMetrics();
 		const int fontAscent = (int) fontMetrics.floatAscent();
        const String  text(m_popupClient->itemText(i));
        const TextRun textRun(text.characters(), text.length());
        context.setFillColor(textColor, ColorSpaceDeviceRGB);    // The fill color is used for the text draw
        context.drawText(font, textRun, IntPoint(itemRect.x() + ThemeEA::kPopupMenuBorderPadding, itemRect.y() + fontAscent));
    }

    // Draw the scrollbar if active
    if(m_scrollBarActive)
    {
        context.save();
        m_scrollBar->paint(&context, m_scrollRect);
        context.restore();
    }

    // We draw the border last
    context.setFillColor(0, ColorSpaceDeviceRGB);          // Disable the fill
    context.setStrokeStyle(SolidStroke);
    context.setStrokeColor(ThemeEA::kListBoxBorderColor, ColorSpaceDeviceRGB);
    context.drawRect(windowRect);

    m_surface->Unlock();   

    // Request a redraw so that the new overlay gets transfered to the main view surface.
    EA::WebKit::IntRect dirtyRegion(m_windowRect.x(),m_windowRect.y(),m_windowRect.width(),m_windowRect.height());
    m_viewEA->AddDirtyRegion(dirtyRegion);                
}

void PopupMenuEA::drawScrollBar(void)
{
    if( !m_surface || !m_scrollBarActive)
        return;

    // Set up the draw context 
    EA::WebKit::ISurface::SurfaceDescriptor surfaceDescriptor = {0};
    m_surface->Lock(&surfaceDescriptor);

    int surfaceW = 0;
    int surfaceH = 0;
    m_surface->GetContentDimensions(&surfaceW,&surfaceH);

    RefPtr<cairo_surface_t> surface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, surfaceW, surfaceH, surfaceDescriptor.mStride));    
	RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(surface.get()));
	WebCore::GraphicsContext context(cairoContext.get());
    
    // Paint the scrollbar if active
    m_scrollBar->paint(&context, m_scrollRect);
    m_surface->Unlock();   

    // Request a redraw so that the new overlay gets transfered to the main view surface.
    EA::WebKit::IntRect dirtyRegion(m_mouseScrollRect.x(),m_mouseScrollRect.y(),m_mouseScrollRect.width(),m_mouseScrollRect.height());
    m_viewEA->AddDirtyRegion(dirtyRegion);    
}

void PopupMenuEA::validateScrollOffset(int& requestedOffset)
{
     if(requestedOffset < 0)
        requestedOffset = 0;
     else if(requestedOffset > (m_itemCount - m_itemScrollCount))
        requestedOffset = m_itemCount - m_itemScrollCount;
}

void PopupMenuEA::updateScrollOffset(const int requestedOffset)
{
    if(requestedOffset < m_itemScrollOffset)
        m_itemScrollOffset = requestedOffset;        
    else if (requestedOffset >= (m_itemCount - m_itemScrollCount))
        m_itemScrollOffset = m_itemCount - m_itemScrollCount; 
    else
        m_itemScrollOffset =  requestedOffset;
}

void PopupMenuEA::validateIndex(int& requestedIndex)
{
   if(requestedIndex >= m_itemCount)
        requestedIndex = (m_itemCount - 1);
    if(requestedIndex < 0) 
        requestedIndex = 0;
}

void PopupMenuEA::updateFocusIndex(int index, bool updateSelected, bool updateScroll)
{
    int requestedIndex = index;
    validateIndex(requestedIndex);
    
    // Skip item if not enabled
    while(!m_popupClient->itemIsEnabled(requestedIndex))
    {
        if(index > m_focusedIndex)
        {
            requestedIndex++;
            if(requestedIndex >= m_itemCount)
            {
                requestedIndex = m_focusedIndex;   // Don't move  for nothing else
                break;
            }
        }
        else
        {
            requestedIndex--;
            if(requestedIndex < 0)
            {
                requestedIndex = m_focusedIndex;    // Don't move  for nothing else
                break;
            }
        }
    }

    // Adjust scroll offsets 
    if(m_scrollBarActive)
    {        
        bool updateScrollValue = false;
        int requestedScrollIndex = requestedIndex;
    
        // If the requestedItem is within the scroll area, we don't update the scroll bar offset
        // This so that the move move over menu items does not activate the scroll thumb
        if(requestedScrollIndex < m_itemScrollOffset)
        {
            updateScrollOffset(requestedScrollIndex);
            updateScrollValue = true;
        }
        else if (requestedScrollIndex >= (m_itemScrollOffset + m_itemScrollCount))
        {
            requestedScrollIndex = requestedScrollIndex - m_itemScrollCount + 1; 
            updateScrollOffset(requestedScrollIndex);
            updateScrollValue = true;
        }

        if(updateScrollValue || updateSelected || updateScroll)
            ScrollableArea::scrollToYOffsetWithoutAnimation(requestedScrollIndex);
    }

    if(updateSelected)
    {
        m_popupClient->setTextFromItem(requestedIndex);
        m_popupClient->valueChanged(requestedIndex);    
    }

    if(requestedIndex != m_focusedIndex)
    {
        m_focusedIndex = requestedIndex;
        draw();     
    }
}

void PopupMenuEA::OverlayBegin(void)
{
    // Was used for ref count in 1.x but there might not be a need for it as overlays are not shared.
}

void PopupMenuEA::OverlayEnd(void)
{
}

void PopupMenuEA::OnKeyboardEvent(const EA::WebKit::KeyboardEvent& keyboardEvent)
{
  using namespace EA::WebKit;

    int index = m_focusedIndex;
   
    if(keyboardEvent.mbDepressed)
    {
        if(keyboardEvent.mbChar)
        {
            // Test for letters that match the first letter of the menu item.
            const UChar c = keyboardEvent.mId;   
            const String s(&c, 1);

             for(int i = 0; i < m_itemCount; ++i)
            {
                String text = m_popupClient->itemText(i);

                if(!text.isEmpty())
                {
                    if(text.startsWith(s, false))
                    {
                        index = i;
                        break;
                    }
                }
            }
        }
        else if (m_popupClient)
        {
            switch(keyboardEvent.mId)
            {
				case kCarriageReturn:
				case kNewLine:
                    m_popupClient->setTextFromItem(index);  
                    m_popupClient->valueChanged(index); 
                    hide();
                    break;

                case kEscape:
                    hide();
                    break;

                case kArrowUp:
                        index -= 1;
                    break;

                case kArrowDown:
                        index += 1;
                     break;

                case kHome:
                        index = 0;
                    break;

                case kEnd:
                        index = (m_itemCount - 1);     
                    break;
                
                default:
                    break;
            }
        }
        
        if(index != m_focusedIndex)
            updateFocusIndex(index);
    }
}

void PopupMenuEA::OnMouseMoveEvent(const EA::WebKit::MouseMoveEvent& mouseMoveEvent)
{
    using namespace EA::WebKit;
    
    if(m_scrollBarActive)
    {
        // Convert to the scroll bar space on the popup surface 
        IntPoint localPos(mouseMoveEvent.mX - m_mouseScrollRect.x(), mouseMoveEvent.mY - m_mouseScrollRect.y());
        IntPoint globalPos(mouseMoveEvent.mX,mouseMoveEvent.mY);

        WebCore::MouseEventType mousePressType;			
        if(m_scrollbarCapturingMouse)
            mousePressType = MouseEventPressed;
        else
           mousePressType = MouseEventReleased;
        
        PlatformMouseEvent evt(localPos,globalPos, LeftButton, mousePressType, 1, 
            (mouseMoveEvent.mModifiers & kModifierMaskShift),(mouseMoveEvent.mModifiers & kModifierMaskControl),
            (mouseMoveEvent.mModifiers & kModifierMaskAlt),(mouseMoveEvent.mModifiers & kModifierMaskOS) ,0);      
        
        // Make sure mouse is in the scroll bar area
        if(m_mouseScrollRect.contains(mouseMoveEvent.mX, mouseMoveEvent.mY))
        {
            m_scrollbarWasUnderMouse = true;
            
            // Sent event to scrollbar
            m_scrollBar->mouseMoved(evt);      

            // Adjust the starting scroll offset as the thumb move is just used for viewing but not selecting
            
            if(m_scrollbarCapturingMouse)
            {
                int requestedOffset = m_scrollBar->value(); 
                updateScrollOffset(requestedOffset);
                draw();                     // Update items and scroll bar
            }
            else
            {
                drawScrollBar();            // Only the scroll bar itself as index values should not have changed
            }
            return;    
        }
        else if(m_scrollbarCapturingMouse)
        {    
            // We need to send a mouse release notification so that the scroll system can know not to track the thumb.
            m_scrollBar->mouseUp();
            m_scrollbarCapturingMouse = false;
        }

        // Update scrollbar if mouse was over previously
        if(m_scrollbarWasUnderMouse)
        {
            m_scrollbarWasUnderMouse = false;         
            
            // Sent event to scrollbar
            m_scrollBar->mouseMoved(evt);      
            drawScrollBar();
        }
    }
        
    if(m_windowRect.contains(mouseMoveEvent.mX, mouseMoveEvent.mY))
    {
        // Mouse over scroll bar or items?
        int  requestedIndex = (mouseMoveEvent.mY - m_windowRect.y()) / m_itemHeight; 

        validateIndex(requestedIndex);
        if(m_scrollBarActive)
            requestedIndex += m_itemScrollOffset;

        updateFocusIndex(requestedIndex);
    }

}

void PopupMenuEA::OnMouseButtonEvent(const EA::WebKit::MouseButtonEvent& mouseButtonEvent)
{
    using namespace EA::WebKit;

    if(mouseButtonEvent.mId != kMouseLeft)
        return;

    if(m_scrollBarActive)
    {
        // Update mouse press status - we track this for the mouse move event does not have a press
        if(!mouseButtonEvent.mbDepressed)
        {
            m_scrollBar->mouseUp();
            m_scrollbarCapturingMouse = false;  
        }
        else
        {
            // Make sure mouse is in the scroll bar area
            if(m_mouseScrollRect.contains(mouseButtonEvent.mX, mouseButtonEvent.mY))
            {
                m_scrollbarWasUnderMouse = true;
                // Convert to the relative pos of the scroll bar on the popup surface
                IntPoint localPos(mouseButtonEvent.mX - m_mouseScrollRect.x(), mouseButtonEvent.mY - m_mouseScrollRect.y());
                     
                // We need to rebuild a mouse mouse event to send down to the scroll system
                IntPoint globalPos(mouseButtonEvent.mX,mouseButtonEvent.mY);

                PlatformMouseEvent evt(localPos,globalPos, LeftButton, MouseEventPressed, 1, 
                (mouseButtonEvent.mModifiers & kModifierMaskShift),(mouseButtonEvent.mModifiers & kModifierMaskControl),
                (mouseButtonEvent.mModifiers & kModifierMaskAlt),(mouseButtonEvent.mModifiers & kModifierMaskOS) ,0);                

                m_scrollBar->mouseDown(evt);        
                m_scrollbarCapturingMouse = true;  

                // Adjust the starting scroll offset as the thumb move is just used for viewing but not selecting
                int requestedOffset = m_scrollBar->value(); 
                updateScrollOffset(requestedOffset);

                draw();
                return;      
            }
        }
    
        // Update scrollbar if mouse was over previously
        drawScrollBar();
    }

    if(mouseButtonEvent.mbDepressed)
    {
        // We should only change the selected element if the user clicked inside the html select element. Otherwise, following code would
        // set the focus to a random element based on where the user clicks in the rest of the window.
        if(m_windowRect.contains(mouseButtonEvent.mX, mouseButtonEvent.mY))
        {
	        int requestedIndex = (mouseButtonEvent.mY - m_windowRect.y()) / m_itemHeight;

	        if(m_scrollBarActive)
	        {
                if(requestedIndex < 0)
			        requestedIndex = 0;
		        else if(requestedIndex >= m_itemScrollCount)
			        requestedIndex = (m_itemScrollCount -1);

		        requestedIndex += m_itemScrollOffset;
	        }

	        updateFocusIndex(requestedIndex, true, true);
        }
		else 
		{ // On consoles, we don't actually move our cursor when selecting items. So we try to figure out if the we are
		  // in that situation.
		#if defined(EA_PLATFORM_CONSOLE)
			bool onConsole = true;
		#elif defined(EA_PLATFORM_WINDOWS)
			bool onConsole = m_viewEA->IsEmulatingConsoleOnPC();
		#elif defined(EA_PLATFORM_OSX)
			bool onConsole = false;
		#endif
			if(onConsole)
			{
				IntRect windowRectIncludingNode = m_windowRect;
				//Shift the rectangle to encompass the original/root node(That is where our cursor is as we don't move it).
				windowRectIncludingNode.move(0, (m_flipped ? m_itemHeight : -m_itemHeight));
				if(windowRectIncludingNode.contains(mouseButtonEvent.mX, mouseButtonEvent.mY))
				{
					m_popupClient->setTextFromItem(m_focusedIndex); 
					m_popupClient->valueChanged(m_focusedIndex);
				}
			}
		}
        hide();
    }
}

void PopupMenuEA::OnMouseWheelEvent(const EA::WebKit::MouseWheelEvent& mouseWheelEvent)
{
    int delta = mouseWheelEvent.mZDelta <= 0 ? 1 : -1;
    int index = m_focusedIndex + delta;
    updateFocusIndex(index, false, false);
}

void PopupMenuEA::OnFocusChangeEvent(bool hasFocus)
{
    if(!hasFocus)
        hide();
}

bool PopupMenuEA::OnButtonEvent(const EA::WebKit::ButtonEvent& buttonEvent)
{
	switch(buttonEvent.mID)
	{
	case EA::WebKit::kButton1://Up 
		{

			if(m_focusedIndex == 0)
				return true;//intentional to return true so that we indicate that the overlay input client handled the input so that we don't end up jumping else where on the page.

			updateFocusIndex(m_focusedIndex-1);
			return true;
		}

	case EA::WebKit::kButton3://Down 
		{
			if(m_focusedIndex == m_itemCount-1)
				return true;//intentional to return true so that we indicate that the overlay input client handled the input so that we don't end up jumping else where on the page.

			updateFocusIndex(m_focusedIndex+1);
			return true;
		}

	default:
		{
			return false;
		}
	}

	return false;
}

// For ScrollableArea (we just use it to set the y offset of the scrollbar) 
int PopupMenuEA::scrollSize(ScrollbarOrientation orientation) const
{
    return ((orientation == VerticalScrollbar) && m_scrollBar) ? (m_scrollBar->totalSize() - m_scrollBar->visibleSize()) : 0;
}

int PopupMenuEA::scrollPosition(Scrollbar*) const
{
    return m_scrollOffset;
}

void PopupMenuEA::setScrollOffset(const IntPoint& offset)
{
   m_scrollOffset = offset.y();
}

} // Webcore namespace

