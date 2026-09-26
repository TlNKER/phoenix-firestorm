/**
 * @file fsfloaterhiddensystemfolders.cpp
 * @brief Floater for choosing which system/virtual inventory folders are hidden (God Mode tool)
 *
 * <FS:Horizon> Hidden System Folders
 */

#include "llviewerprecompiledheaders.h"

#include "fsfloaterhiddensystemfolders.h"

#include <vector>

#include "llscrolllistcell.h"
#include "llscrolllistctrl.h"
#include "llscrolllistitem.h"
#include "llviewercontrol.h"

namespace
{
    // The set of system/virtual inventory folders this floater can hide. Matched by exact,
    // non-localized inventory folder name - the same names checked by
    // LLInventoryPanel::updateHiddenSystemFolders() and the "#Firestorm" check in
    // LLInventoryPanel::buildViewsTree() (indra/newview/llinventorypanel.cpp). Add a folder
    // here AND wire up its hide behavior there if more folders should become configurable.
    const char* const HORIZON_HIDEABLE_FOLDERS[] =
    {
        "Calling Cards",
        "Current Outfit",
        "Favorites",
        "#Firestorm",
    };
}

FSFloaterHiddenSystemFolders::FSFloaterHiddenSystemFolders(const LLSD& key)
    : LLFloater(key),
      mFolderList(nullptr)
{
}

FSFloaterHiddenSystemFolders::~FSFloaterHiddenSystemFolders()
{
}

bool FSFloaterHiddenSystemFolders::postBuild()
{
    mFolderList = getChild<LLScrollListCtrl>("folder_list");

    getChild<LLUICtrl>("ok_btn")->setCommitCallback(boost::bind(&FSFloaterHiddenSystemFolders::onOK, this));
    getChild<LLUICtrl>("apply_btn")->setCommitCallback(boost::bind(&FSFloaterHiddenSystemFolders::onApply, this));
    getChild<LLUICtrl>("cancel_btn")->setCommitCallback(boost::bind(&FSFloaterHiddenSystemFolders::onCancel, this));

    return LLFloater::postBuild();
}

void FSFloaterHiddenSystemFolders::onOpen(const LLSD& key)
{
    refresh();
}

void FSFloaterHiddenSystemFolders::refresh()
{
    if (!mFolderList)
    {
        return;
    }

    mFolderList->deleteAllItems();

    const LLSD& hidden_folders = gSavedSettings.getLLSD("FSHiddenSystemFolders");

    for (const char* folder_name : HORIZON_HIDEABLE_FOLDERS)
    {
        bool is_hidden = false;
        for (LLSD::array_const_iterator it = hidden_folders.beginArray(); it != hidden_folders.endArray(); ++it)
        {
            if (it->asString() == folder_name)
            {
                is_hidden = true;
                break;
            }
        }

        LLScrollListCell::Params check_cell;
        check_cell.column("shown").type("checkbox").value(!is_hidden);

        LLScrollListCell::Params name_cell;
        name_cell.column("folder_name").type("text").value(std::string(folder_name));

        LLScrollListItem::Params item;
        item.columns.add(check_cell);
        item.columns.add(name_cell);
        mFolderList->addRow(item);
    }
}

void FSFloaterHiddenSystemFolders::onApply()
{
    if (!mFolderList)
    {
        return;
    }

    LLSD hidden_folders = LLSD::emptyArray();

    for (LLScrollListItem* row : mFolderList->getAllData())
    {
        if (!row)
        {
            continue;
        }
        bool shown = row->getColumn(0)->getValue().asBoolean();
        if (!shown)
        {
            hidden_folders.append(row->getColumn(1)->getValue().asString());
        }
    }

    gSavedSettings.setLLSD("FSHiddenSystemFolders", hidden_folders);
}

void FSFloaterHiddenSystemFolders::onOK()
{
    onApply();
    closeFloater();
}

void FSFloaterHiddenSystemFolders::onCancel()
{
    closeFloater();
}
