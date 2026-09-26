/**
 * @file fsfloaterscriptedattachments.h
 * @brief Floater showing a scripted-attachment scan of another avatar (God Mode tool)
 *
 * <FS:Horizon> Scan scripted attachments
 */

#ifndef FS_FLOATERSCRIPTEDATTACHMENTS_H
#define FS_FLOATERSCRIPTEDATTACHMENTS_H

#include "llfloater.h"
#include "lluuid.h"

class LLScrollListCtrl;
class LLTextBox;

class FSFloaterScriptedAttachments : public LLFloater
{
    friend class LLFloaterReg;

public:
    // Called right after the scan request is sent, so the floater opens immediately
    // in a "Scanning..." state rather than waiting for the bridge's reply.
    static void showScanning(const LLUUID& avatar_id, const std::string& avatar_name);

    // Called by FSLSLBridge once a scan has fully completed (or failed/timed out).
    // status: "OK" (results contains 0+ rows), "NOT FOUND" / "NOT ON REGION" / "NO ATTACHMENTS" / "NONE" / "TIMEOUT"
    static void showResults(const LLUUID& avatar_id, const LLSD& results, const std::string& status);

    bool postBuild() override;
    void onOpen(const LLSD& key) override;

private:
    FSFloaterScriptedAttachments(const LLSD& key);
    ~FSFloaterScriptedAttachments();

    void populateResults(const LLSD& results, const std::string& status);

    LLScrollListCtrl* mResultsList;
    LLTextBox* mStatusText;
    LLUUID mAvatarID;
    std::string mAvatarName;
};

#endif // FS_FLOATERSCRIPTEDATTACHMENTS_H
