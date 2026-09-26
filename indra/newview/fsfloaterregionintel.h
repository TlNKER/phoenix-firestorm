/**
 * @file fsfloaterregionintel.h
 * @brief Floater showing publicly-available region status and rules (God Mode tool)
 *
 * <FS:Horizon> Region Intel
 */

#ifndef FS_FLOATERREGIONINTEL_H
#define FS_FLOATERREGIONINTEL_H

#include "llfloater.h"
#include "lluuid.h"
#include <boost/signals2.hpp>

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

    LLTextEditor* mOutput;
    LLUUID mLastOwnerID;
    boost::signals2::connection mRegionChangedConnection;
};

#endif // FS_FLOATERREGIONINTEL_H
