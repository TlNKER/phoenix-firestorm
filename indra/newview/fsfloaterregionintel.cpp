/**
 * @file fsfloaterregionintel.cpp
 * @brief Floater showing publicly-available region status and rules (God Mode tool)
 *
 * <FS:Horizon> Region Intel
 */

#include "llviewerprecompiledheaders.h"

#include "fsfloaterregionintel.h"

#include <sstream>

#include "llagent.h"
#include "llappviewer.h"
#include "llavatarname.h"
#include "llavatarnamecache.h"
#include "llcorehttputil.h"
#include "llcoros.h"
#include "lluri.h"
#include "llregionflags.h"
#include "lltexteditor.h"
#include "llviewerregion.h"

namespace
{
    // <FS:Horizon> Simulator codename (Region Intel)
    const std::string CODENAME_API_URL = "https://tinker.sl/lab/api/codename.php";
    // </FS:Horizon>
}

FSFloaterRegionIntel::FSFloaterRegionIntel(const LLSD& key)
    : LLFloater(key),
      mOutput(nullptr),
      mLastOwnerID(LLUUID::null)
{
}

FSFloaterRegionIntel::~FSFloaterRegionIntel()
{
    if (mRegionChangedConnection.connected())
    {
        mRegionChangedConnection.disconnect();
    }
}

bool FSFloaterRegionIntel::postBuild()
{
    mOutput = getChild<LLTextEditor>("region_intel_output");
    return LLFloater::postBuild();
}

void FSFloaterRegionIntel::onOpen(const LLSD& key)
{
    if (!mRegionChangedConnection.connected())
    {
        mRegionChangedConnection = gAgent.addRegionChangedCallback(boost::bind(&FSFloaterRegionIntel::onRegionChanged, this));
    }
    refresh();
}

void FSFloaterRegionIntel::onClose(bool app_quitting)
{
    if (mRegionChangedConnection.connected())
    {
        mRegionChangedConnection.disconnect();
    }
}

void FSFloaterRegionIntel::onRegionChanged()
{
    refresh();
}

void FSFloaterRegionIntel::refresh()
{
    if (!mOutput)
    {
        return;
    }

    LLViewerRegion* region = gAgent.getRegion();
    if (!region)
    {
        mOutput->setText(std::string("No region information available."));
        return;
    }

    mLastOwnerID = region->getOwner();

    std::string owner_line = "(none)";
    if (mLastOwnerID.notNull())
    {
        LLAvatarName av_name;
        if (LLAvatarNameCache::get(mLastOwnerID, &av_name))
        {
            owner_line = av_name.getCompleteName();
        }
        else
        {
            owner_line = "Resolving...";
            LLAvatarNameCache::get(mLastOwnerID, boost::bind(&FSFloaterRegionIntel::onOwnerNameLoaded, this, _1, _2));
        }
    }

    // <FS:Horizon> Simulator codename (Region Intel)
    // gLastVersionChannel looks like "Second Life RC Magnum 2026-09-29.36597266981":
    // an optional "Second Life " grid prefix, a channel/type label ("RC Magnum",
    // "Server", ...), and a trailing "<date>.<build>" version. Split those into
    // their own lines, and use the date portion to look up the informal LL
    // simulator-project codename ("Nectarine", ...) for that build via our own
    // small tracking API.
    std::string type_str = "(unknown)";
    std::string version_str;
    if (!gLastVersionChannel.empty())
    {
        std::string remainder = gLastVersionChannel;
        const std::string prefix = "Second Life ";
        if (remainder.compare(0, prefix.size(), prefix) == 0)
        {
            remainder = remainder.substr(prefix.size());
        }

        size_t last_space = remainder.find_last_of(' ');
        if (last_space != std::string::npos)
        {
            type_str = remainder.substr(0, last_space);
            version_str = remainder.substr(last_space + 1);
        }
        else
        {
            type_str = remainder;
        }
    }

    std::string build_date = version_str.substr(0, version_str.find('.'));

    std::string codename_line;
    if (build_date.empty())
    {
        codename_line = "(unknown)";
    }
    else
    {
        if (build_date != mCodenameRequestedForDate)
        {
            requestCodename(build_date);
        }
        codename_line = mCodenameDisplay.empty() ? "Resolving..." : mCodenameDisplay;
    }
    // </FS:Horizon>

    bool sandbox = region->getRegionFlag(REGION_FLAGS_SANDBOX);
    bool prelude = region->isPrelude();

    std::ostringstream out;
    out << "REGION STATUS\n";
    out << "-------------\n";
    out << "Owner: " << owner_line << "\n";
    out << "Type: " << type_str << "\n";
    out << "Version: " << (version_str.empty() ? std::string("(unknown)") : version_str) << "\n";
    out << "Codename: " << codename_line << "\n";
    out << "Sandbox: " << (sandbox ? "Yes" : "No") << "\n";
    out << "Prelude: " << (prelude ? "Yes" : "No") << "\n";
    out << "Max Objects: " << region->getMaxTasks() << "\n";
    out << "Water Height: " << llformat("%.1f", region->getWaterHeight()) << " m\n";
    out << "\n";
    out << "REGION RULES\n";
    out << "------------\n";
    out << "Allow Damage: " << (region->getAllowDamage() ? "Yes" : "No") << "\n";
    out << "Allow Landmark: " << (region->getAllowLandmark() ? "Yes" : "No") << "\n";
    out << "Allow Set Home: " << (region->getAllowSetHome() ? "Yes" : "No") << "\n";
    out << "Allow Direct Teleport: " << (region->getAllowDirectTeleport() ? "Yes" : "No") << "\n";
    out << "Block Fly: " << (region->getBlockFly() ? "Yes" : "No") << "\n";
    out << "Terraform Allowed: " << (region->getAllowTerraform() ? "Yes" : "No") << "\n";
    out << "Push Restricted: " << (region->getRestrictPushObject() ? "Yes" : "No") << "\n";

    mOutput->setText(out.str());

    setTitle("Region Intel - " + region->getName());
}

void FSFloaterRegionIntel::onOwnerNameLoaded(const LLUUID& id, const LLAvatarName& av_name)
{
    if (id != mLastOwnerID)
    {
        // A newer refresh (region change) has superseded this lookup.
        return;
    }
    refresh();
}

// <FS:Horizon> Simulator codename (Region Intel)
void FSFloaterRegionIntel::requestCodename(const std::string& date_str)
{
    if (date_str.empty() || date_str == mCodenameRequestedForDate)
    {
        return;
    }

    mCodenameRequestedForDate = date_str;
    mCodenameDisplay.clear();

    LLCoros::instance().launch("FSFloaterRegionIntel::fetchCodenameCoro",
        boost::bind(&FSFloaterRegionIntel::fetchCodenameCoro, getDerivedHandle<FSFloaterRegionIntel>(), date_str));
}

// static
void FSFloaterRegionIntel::fetchCodenameCoro(LLHandle<FSFloaterRegionIntel> handle, std::string date_str)
{
    LLCore::HttpRequest::policy_t httpPolicy(LLCore::HttpRequest::DEFAULT_POLICY_ID);
    LLCoreHttpUtil::HttpCoroutineAdapter::ptr_t httpAdapter =
        std::make_shared<LLCoreHttpUtil::HttpCoroutineAdapter>("FSFloaterRegionIntelCodenameCoro", httpPolicy);
    LLCore::HttpRequest::ptr_t httpRequest = std::make_shared<LLCore::HttpRequest>();

    std::string url = CODENAME_API_URL + "?date=" + LLURI::escape(date_str);

    LLSD result = httpAdapter->getAndSuspend(httpRequest, url);

    LLSD httpResults = result[LLCoreHttpUtil::HttpCoroutineAdapter::HTTP_RESULTS];
    LLCore::HttpStatus status = LLCoreHttpUtil::HttpCoroutineAdapter::getStatusFromLLSD(httpResults);

    // The floater may have been closed while this request was in flight -
    // handle.get() returns nullptr in that case and there's nothing left to
    // update.
    FSFloaterRegionIntel* floater = handle.get();
    if (!floater)
    {
        return;
    }

    if (!status || !result.has("display"))
    {
        floater->mCodenameDisplay = "(unavailable)";
    }
    else
    {
        floater->mCodenameDisplay = result["display"].asString();
    }

    floater->refresh();
}
// </FS:Horizon>
