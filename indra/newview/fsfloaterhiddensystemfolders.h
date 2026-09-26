/**
 * @file fsfloaterhiddensystemfolders.h
 * @brief Floater for choosing which system/virtual inventory folders are hidden (God Mode tool)
 *
 * <FS:Horizon> Hidden System Folders
 */

#ifndef FS_FLOATERHIDDENSYSTEMFOLDERS_H
#define FS_FLOATERHIDDENSYSTEMFOLDERS_H

#include "llfloater.h"

class LLScrollListCtrl;

class FSFloaterHiddenSystemFolders : public LLFloater
{
    friend class LLFloaterReg;

public:
    bool postBuild() override;
    void onOpen(const LLSD& key) override;

private:
    FSFloaterHiddenSystemFolders(const LLSD& key);
    ~FSFloaterHiddenSystemFolders();

    void refresh();
    void onApply();
    void onOK();
    void onCancel();

    LLScrollListCtrl* mFolderList;
};

#endif // FS_FLOATERHIDDENSYSTEMFOLDERS_H
