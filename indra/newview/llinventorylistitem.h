/**
 * @file llinventorylistitem.h
 * @brief Inventory list item panel.
 *
 * Class LLPanelInventoryListItemBase displays inventory item as an element
 * of LLInventoryItemsList.
 *
 * $LicenseInfo:firstyear=2010&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#ifndef LL_LLINVENTORYLISTITEM_H
#define LL_LLINVENTORYLISTITEM_H

// llcommon
#include "llassettype.h"

// llui
#include "llpanel.h"
#include "llstyle.h"
#include "lliconctrl.h"
#include "lltextbox.h"
#include "lluicolor.h" // <FS:Horizon> Set Display Name - for getDisplayNameOverrideColor()'s return type

// newview
#include "llwearabletype.h"

class LLViewerInventoryItem;

/**
 * @class LLPanelInventoryListItemBase
 *
 * Base class for Inventory flat list item. Panel consists of inventory icon
 * and inventory item name.
 * This class is able to display widgets(buttons) on left(before icon) and right(after text-box) sides
 * of panel.
 *
 * How to use (see LLPanelClothingListItem for example):
 * - implement init() to build panel from xml
 * - create new xml file, fill it with widgets you want to dynamically show/hide/reshape on left/right sides
 * - redefine postBuild()(call base implementation) and add needed widgets to needed sides,
 *
 */
class LLPanelInventoryListItemBase : public LLPanel
{
public:
    struct Params : public LLInitParam::Block<Params, LLPanel::Params>
    {
        Optional<LLStyle::Params>   default_style,
                                    mismatch_style, // <FS:Ansariel> Better attachment list
                                    worn_style;
        Optional<LLUIImage*>        hover_image,
                                    selected_image,
                                    separator_image;
        Optional<LLIconCtrl::Params>    item_icon;
        Optional<LLTextBox::Params>     item_name;
        Params();
    };

    typedef enum e_item_state {
        IS_DEFAULT,
        IS_WORN,
        IS_MISMATCH // <FS:Ansariel> Better attachment list
    } EItemState;

    static LLPanelInventoryListItemBase* create(LLViewerInventoryItem* item);

    virtual void draw();

    /**
     * Let item know it need to be refreshed in next draw()
     */
    void setNeedsRefresh(bool needs_refresh){ mNeedsRefresh = needs_refresh; }

    bool getNeedsRefresh(){ return mNeedsRefresh; }

    /**
     * Add widget to left side
     */
    void addWidgetToLeftSide(const std::string& name, bool show_widget = true);
    void addWidgetToLeftSide(LLUICtrl* ctrl, bool show_widget = true);

    /**
     * Add widget to right side, widget is supposed to be child of calling panel
     */
    void addWidgetToRightSide(const std::string& name, bool show_widget = true);
    void addWidgetToRightSide(LLUICtrl* ctrl, bool show_widget = true);

    /**
     * Mark widgets as visible. Only visible widgets take part in reshaping children
     */
    void setShowWidget(const std::string& name, bool show);
    void setShowWidget(LLUICtrl* ctrl, bool show);

    /**
     * Set spacing between widgets during reshape
     */
    void setWidgetSpacing(S32 spacing) { mWidgetSpacing = spacing; }

    S32 getWidgetSpacing() { return mWidgetSpacing; }

    /**
     * Inheritors need to call base implementation of postBuild()
     */
    /*virtual*/ bool postBuild();

    /**
     * Handles item selection
     */
    /*virtual*/ void setValue(const LLSD& value);

    /**
     * Handles filter request
     */
    /*virtual*/ S32  notify(const LLSD& info);

    /* Highlights item */
    /*virtual*/ bool handleHover(S32 x, S32 y, MASK mask);
    /* Removes item highlight */
    /*virtual*/ void onMouseLeave(S32 x, S32 y, MASK mask);

    /** Get the name of a corresponding inventory item */
    // <FS:Horizon> Set Display Name - returns the viewer-only display name
    // override when one is set (same source updateItem() renders), so
    // sorting matches what's actually shown; falls back to the real item
    // name otherwise. Returned by value since the override is computed on
    // the fly rather than stored on the item itself.
    std::string getItemName() const;
    // </FS:Horizon>

    /** Get the asset type of a corresponding inventory item */
    LLAssetType::EType getType() const;

    /** Get the wearable type of a corresponding inventory item */
    LLWearableType::EType getWearableType() const;

    /** Get the description of a corresponding inventory item */
    const std::string& getDescription() const;

    /** Get the creation date of a corresponding inventory item */
    time_t getCreationDate() const;

    /** Get the associated inventory item */
    LLViewerInventoryItem* getItem() const;

    void setSeparatorVisible(bool visible) { mSeparatorVisible = visible; }
    void resetHighlight() { mHovered = false; }

    virtual ~LLPanelInventoryListItemBase(){}

protected:

    LLPanelInventoryListItemBase(LLViewerInventoryItem* item, const Params& params);

    typedef std::vector<LLUICtrl*> widget_array_t;

    /**
     * Called after inventory item was updated, update panel widgets to reflect inventory changes.
     */
    virtual void updateItem(const std::string& name,
                            bool favorite,
                            EItemState item_state = IS_DEFAULT);

    void setLeftWidgetsWidth(S32 width) { mLeftWidgetsWidth = width; }
    void setRightWidgetsWidth(S32 width) { mRightWidgetsWidth = width; }

    /**
     * Set all widgets from both side visible/invisible. Only enabled widgets
     * (see setShowWidget()) can become visible
     */
    virtual void setWidgetsVisible(bool visible);

    /**
     * Reshape all child widgets - icon, text-box and side widgets
     */
    virtual void reshapeWidgets();

    /** set wearable type icon image */
    void setIconImage(const LLUIImagePtr& image);

    /** Set item title - inventory item name usually */
    void setTitle(const std::string& title,
                  const std::string& highlit_text,
                  EItemState item_state = IS_DEFAULT);

    /**
     * Show tool tip if item name text size > panel size
     */
    virtual bool handleToolTip( S32 x, S32 y, MASK mask);

    virtual const LLPanelInventoryListItemBase::Params& getDefaultParams() const; // <FS:Ansariel> Better attachment list

    // <FS:Horizon> Set Display Name - hooks used in setTitle() below for an
    // item showing a display-name override, so a subclass used in a
    // specific context (see LLPanelWearableOutfitItem/FSPanelCOFWearableOutfitListItem
    // in llwearableitemslist.h) can convey that by color, by font, or both,
    // without duplicating setTitle()'s whole style-selection logic.

    // Whether to color the item using getDisplayNameOverrideColor() below.
    // Default true (color is the normal way this is shown); a context that
    // conveys the override some other way (e.g. an italic font) instead
    // returns false here so the usual worn/unworn color isn't overwritten.
    virtual bool useDisplayNameOverrideColor() const { return true; }

    // Color used when useDisplayNameOverrideColor() is true.
    virtual const LLUIColor& getDisplayNameOverrideColor() const;

    // Font to use for the item's name. Default returns base_font unchanged
    // (no font change) - a context that conveys the override via font
    // (e.g. italic, or bold+italic when item_state is IS_WORN) overrides
    // this instead. base_font is the style's own font if one is already
    // set, otherwise the title control's current font.
    virtual const LLFontGL* getDisplayNameOverrideFont(const LLFontGL* base_font, EItemState item_state) const { return base_font; }
    // </FS:Horizon>

    const LLUUID mInventoryItemUUID;
    bool mHovered;

private:

    /** reshape left side widgets
     * Deprecated for now. Disabled reshape left for now to reserve space for 'delete'
     * button in LLPanelClothingListItem according to Neal's comment (https://codereview.productengine.com/secondlife/r/325/)
     */
    void reshapeLeftWidgets();

    /** reshape right side widgets */
    void reshapeRightWidgets();

    /** reshape remaining widgets */
    void reshapeMiddleWidgets();


    LLIconCtrl*     mIconCtrl;
    LLTextBox*      mTitleCtrl;

    LLUIImagePtr    mIconImage;
    LLUIImagePtr    mHoverImage;
    LLUIImagePtr    mSelectedImage;
    LLUIImagePtr    mSeparatorImage;

    bool            mSelected = false;
    bool            mSeparatorVisible = false;
    bool            mIsFavorite = false; // note that any setter needs to update tittle

    std::string     mHighlightedText;

    widget_array_t  mLeftSideWidgets;
    widget_array_t  mRightSideWidgets;
    S32             mWidgetSpacing;

    S32             mLeftWidgetsWidth;
    S32             mRightWidgetsWidth;
    bool            mNeedsRefresh;
};

#endif //LL_LLINVENTORYLISTITEM_H
