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
#include "llregionflags.h"
#include "lltexteditor.h"
#include "llviewerregion.h"

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

    std::string type_line = gLastVersionChannel.empty() ? std::string("(unknown)") : gLastVersionChannel;

    bool sandbox = region->getRegionFlag(REGION_FLAGS_SANDBOX);
    bool prelude = region->isPrelude();

    std::ostringstream out;
    out << "REGION STATUS\n";
    out << "-------------\n";
    out << "Owner: " << owner_line << "\n";
    out << "Type: " << type_line << "\n";
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
