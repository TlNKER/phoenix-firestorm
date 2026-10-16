/**
 * @file fsfloaterregionintel.h
 * @brief Floater showing publicly-available region status and rules (God Mode tool)
 *
 * <FS:Horizon> Region Intel
 */

#ifndef FS_FLOATERREGIONINTEL_H
#define FS_FLOATERREGIONINTEL_H

#include "llfloater.h"
#include "llhandle.h"
#include "lluuid.h"
#include <boost/signals2.hpp>
#include <string>

class LLTextEditor;
class LLAvatarName;

class FSFloaterRegionIntel : public LLFloater
{
    friend class LLFloaterReg;

public:
    bool postBuild() override;
    void onOpen(const LLSD& key) override;
    void onClose(bool app_quitting) override;

private:
    FSFloaterRegionIntel(const LLSD& key);
    ~FSFloaterRegionIntel();

    void refresh();
    void onOwnerNameLoaded(const LLUUID& id, const LLAvatarName& av_name);
    void onRegionChanged();

    // <FS:Horizon> Simulator codename (Region Intel)
    // Kicks off an async lookup of the informal LL simulator-project codename
    // ("Nectarine", "Olive", ...) in effect on the given server-build date
    // (YYYY-MM-DD, taken from the version string in gLastVersionChannel)
    // against our own small tracking API at tinker.sl - unless we've already
    // requested that same date. The result comes back via fetchCodenameCoro,
    // which re-calls refresh() to redraw once it has an answer.
    void requestCodename(const std::string& date_str);
    static void fetchCodenameCoro(LLHandle<FSFloaterRegionIntel> handle, std::string date_str);
    // </FS:Horizon>

    LLTextEditor* mOutput;
    LLUUID mLastOwnerID;
    boost::signals2::connection mRegionChangedConnection;

    // <FS:Horizon> Simulator codename (Region Intel)
    std::string mCodenameRequestedForDate;
    std::string mCodenameDisplay;
    // </FS:Horizon>
};

#endif // FS_FLOATERREGIONINTEL_H
