/**
 * @file fsfloaterscriptedattachments.cpp
 * @brief Floater showing a scripted-attachment scan of another avatar (God Mode tool)
 *
 * <FS:Horizon> Scan scripted attachments
 */

#include "llviewerprecompiledheaders.h"

#include "fsfloaterscriptedattachments.h"

#include <algorithm>
#include <vector>

#include "llfloaterreg.h"
#include "llscrolllistctrl.h"
#include "llscrolllistcell.h"
#include "llscrolllistitem.h"
#include "lltextbox.h"
#include "llsd.h"

FSFloaterScriptedAttachments::FSFloaterScriptedAttachments(const LLSD& key)
    : LLFloater(key),
      mResultsList(nullptr),
      mStatusText(nullptr),
      mAvatarID(LLUUID::null)
{
}

FSFloaterScriptedAttachments::~FSFloaterScriptedAttachments()
{
}

bool FSFloaterScriptedAttachments::postBuild()
{
    mResultsList = getChild<LLScrollListCtrl>("results_list");
    mStatusText = getChild<LLTextBox>("status_text");
    return LLFloater::postBuild();
}

void FSFloaterScriptedAttachments::onOpen(const LLSD& key)
{
}

// static
void FSFloaterScriptedAttachments::showScanning(const LLUUID& avatar_id, const std::string& avatar_name)
{
    FSFloaterScriptedAttachments* floater = LLFloaterReg::getTypedInstance<FSFloaterScriptedAttachments>("scripted_attachments");
    if (!floater)
    {
        return;
    }

    floater->mAvatarID = avatar_id;
    floater->mAvatarName = avatar_name;

    if (floater->mResultsList)
    {
        floater->mResultsList->deleteAllItems();
    }
    if (floater->mStatusText)
    {
        floater->mStatusText->setText(llformat("Scanning %s's attachments...", avatar_name.c_str()));
    }

    floater->setTitle("Scripted Attachments - " + avatar_name);
    LLFloaterReg::showTypedInstance<FSFloaterScriptedAttachments>("scripted_attachments");
}

// static
void FSFloaterScriptedAttachments::showResults(const LLUUID& avatar_id, const LLSD& results, const std::string& status)
{
    FSFloaterScriptedAttachments* floater = LLFloaterReg::getTypedInstance<FSFloaterScriptedAttachments>("scripted_attachments");
    if (!floater)
    {
        return;
    }

    // If a newer scan was already kicked off for a different avatar in the meantime, don't
    // clobber it with a late-arriving reply for an older request.
    if (floater->mAvatarID.notNull() && avatar_id.notNull() && floater->mAvatarID != avatar_id)
    {
        return;
    }

    floater->populateResults(results, status);
    LLFloaterReg::showTypedInstance<FSFloaterScriptedAttachments>("scripted_attachments");
}

void FSFloaterScriptedAttachments::populateResults(const LLSD& results, const std::string& status)
{
    if (!mResultsList)
    {
        return;
    }

    mResultsList->deleteAllItems();

    if (status == "NOT FOUND" || status == "NOT ON REGION")
    {
        if (mStatusText)
        {
            mStatusText->setText(llformat("%s is not in this region.", mAvatarName.c_str()));
        }
        return;
    }
    if (status == "NO ATTACHMENTS" || status == "NONE")
    {
        if (mStatusText)
        {
            mStatusText->setText(llformat("%s has no visible scripted attachments.", mAvatarName.c_str()));
        }
        return;
    }
    if (status == "TIMEOUT")
    {
        if (mStatusText)
        {
            mStatusText->setText(llformat("Scan of %s timed out - no reply from the LSL Bridge.", mAvatarName.c_str()));
        }
        return;
    }

    // Sort rows by ms/frame descending before inserting, so the worst offenders are on top
    // even before the user clicks the column header.
    std::vector<LLSD> rows;
    for (LLSD::array_const_iterator it = results.beginArray(); it != results.endArray(); ++it)
    {
        rows.push_back(*it);
    }
    std::sort(rows.begin(), rows.end(), [](const LLSD& a, const LLSD& b)
    {
        return a["ms"].asReal() > b["ms"].asReal();
    });

    for (const LLSD& row : rows)
    {
        LLScrollListCell::Params name_cell;
        name_cell.column("attachment_name").type("text").value(row["name"].asString());

        LLScrollListCell::Params point_cell;
        point_cell.column("attachment_point").type("text").value(row["point_name"].asString());

        LLScrollListCell::Params count_cell;
        count_cell.column("script_count").type("text").value(row["count"].asInteger());

        LLScrollListCell::Params ms_cell;
        ms_cell.column("script_ms").type("text").value(llformat("%.2f", row["ms"].asReal()));

        LLScrollListItem::Params item;
        item.columns.add(name_cell);
        item.columns.add(point_cell);
        item.columns.add(count_cell);
        item.columns.add(ms_cell);
        mResultsList->addRow(item);
    }

    if (mStatusText)
    {
        mStatusText->setText(llformat("%s - %d scripted attachment%s found.", mAvatarName.c_str(), (int)rows.size(), rows.size() == 1 ? "" : "s"));
    }
}
