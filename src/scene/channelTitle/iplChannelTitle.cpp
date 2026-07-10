#include "scene/channelTitle/iplChannelTitle.h"

#include "BS2/BS2.h"
#include "iplSound.h"
#include "iplSystem.h"
#include "layout/GUIManager.h"
#include "math/iplInterporation.h"
#include "scene/setting/iplNCDSetting.h"
#include "system/iplBS2Manager.h"
#include "system/iplChannelRsoThread.h"
#include "system/iplChannelScriptManager.h"
#include "system/iplController.h"
#include "system/iplDialogWindow.h"
#include "system/iplNandManager.h"
#include "system/iplNandWall.h"
#include "system/iplResetHandler.h"
#include "system/iplSDVFWorker.h"
#include "system/iplSaveDataManager.h"
#include "utility/iplESMisc.h"
#include "utility/iplFrameController.h"
#include "utility/iplGraphics.h"

#include <private/os/OSExec.h>
#include <private/os/OSSram.h>
#include <private/os/OSPlayRecord.h>
#include <private/os/OSPlayTime.h>
#include <private/wpad/WPADInternal.h>
#include <revolution/os.h>
#include <revolution/rso.h>
#include <revolution/sc.h>
#include <revolution/vi.h>
#include <revolution/wpad.h>


extern "C" void iplChannelTitle_813BA684(void*);
extern "C" void iplChannelTitle_813BA784(void*);
extern void iplSavedata_813596B8(void* mgr, u64 titleId);
namespace ipl { namespace utility { s32 DeleteTitleContent(EGG::Heap*, ESTitleId); } }

extern const u64 lbl_816102F0[];
extern const f32 lbl_81696AB4;
extern char lbl_8164EF00[];

// Struct types for .data blocks with embedded pointer arrays
typedef struct {
    char mChangeTextB[14];
    u8 _pad0[2];
    const char* mGroupNames[8];
    u8 _pad1[4];
} GroupNamePtrBlock;

typedef struct {
    char mChangeOut[27];
    u8 _pad0[1];
    const char* mAnmNames[10];
} AnmNamePtrBlock;

#pragma pack(push, 1)
typedef struct {
    char mText[26];
    const char* mPtrs[12];
} EAE6Block;
#pragma pack(pop)

// .sdata strings referenced by pointer arrays
extern char lbl_8164EAD4[];
extern EAE6Block lbl_8164EAE6;
extern volatile const f32 lbl_816949F4 = 28.0f;
extern volatile const f32 lbl_816949F8 = 1.0f;
extern const u8 lbl_816949FC = 0xFF;
extern const u8 lbl_816949FD = 0xFF;
extern const u8 lbl_816949FE = 0xFF;
extern const u8 lbl_816949FF = 0x00;

extern "C" void* lbl_81696AAC[2] = { NULL, NULL };

template class ipl::math::HermiteIntp<f32>;

#include "scene/channelSelect/iplChannelObj.h"
#include "scene/channelSelect/iplChannelSelect.h"

GroupNamePtrBlock lbl_8164E7C0 = {
    "G_ChangeTextB",
    {0, 0},
    { "G_FocusBtnA", "G_FocusBtnB", "G_SelectBtnA", "G_SelectBtnB",
      "G_OnOffBtnA", "G_OnOffBtnB", "G_ChangeTextA", (const char*)&lbl_8164E7C0 },
    {0, 0, 0, 0}
};

AnmNamePtrBlock lbl_8164E8F0 = {
    "my_ChTop_a_ChangeOut.brlan",
    {0},
    {  "my_ChTop_a_FocusBtnA_off.brlan", "my_ChTop_a_FocusBtn_on.brlan",
        "my_ChTop_a_SelectBtn_Ac.brlan", "my_ChTop_a_OffBtn.brlan",
      "my_ChTop_a_OnBtn.brlan", "my_ChTop_a_ChangeTextOut.brlan",
        "my_ChTop_a_ChangeTextIn.brlan", "my_ChTop_a_ChangeIn.brlan",
      "my_ChTop_a_ChangeRoop.brlan", (const char*)&lbl_8164E8F0 }
};

// Data section labels referenced by bind functions
const char* lbl_8164E968[3] = { "banner.brlan", "banner_Start.brlan", "banner_Loop.brlan" };

// Data for startChanSound disk info comparison
const ESTitleId lbl_816102E0[2] = { 0x0000000052543800ULL, 0x0000000052543700ULL };

const char* lbl_81696A20[2] = { "B_BtnA", "B_BtnB" };
char lbl_81696A28[8] = "Change";
const char* lbl_81696A30 = lbl_81696A28;

// Data for bindDiskAnms - string labels
const char* lbl_8164E988[5] = { "G_Comment0", "G_DiskIn", "G_Wii", "G_GC", NULL };
const char* lbl_81696A40 = lbl_8164EAD4;
char lbl_81696A44[8] = "G_Back";
const char* lbl_81696A4C = lbl_81696A44;
const char* lbl_81696A50 = (const char*)&lbl_8164EAE6;

const char* lbl_8164EAAC[10] = {
    "my_DiskCh_a_Start.brlan", "my_DiskCh_a_DiskStart.brlan",
    "my_DiskCh_a_DiskLoop.brlan", "my_DiskCh_a_DiskEnd.brlan",
    "my_DiskCh_a_DiskLost.brlan", "my_DiskCh_a_DiskIn.brlan",
    "my_DiskCh_a_DiskEject.brlan", "my_DiskCh_a_Unknown.brlan",
    "my_DiskCh_a_UnknownLoop.brlan", "my_DiskCh_a_UnknwnEject.brlan"
};


char lbl_8164EAD4[] = "my_DiskCh_a.brlyt";
EAE6Block lbl_8164EAE6 = {
    "my_GCTop_a_BackLoop.brlan",
    { "Fre_a", "Fre_d", "Fre_i", "Fre_l", "Fre_e", "Fre_f",
        "Fre_g", "Fre_h", "Fre_b", "Fre_c", "Fre_j", "Fre_k" }
};

char lbl_81696A9C[] = "T_BtnA";
char lbl_81696AA3[] = "T_BtnB";

namespace ipl {
    namespace scene {
        ESTicketView ChannelTitle::msTicketView;
        ESTicketView ChannelTitle::msUnlockTicket;

        ChannelTitle::~ChannelTitle() {}

        void ChannelTitle::setDebugRsoInterval(u32 val) {}

        void ChannelTitle::startResetting() {
            snd::sSystem.resetAllSound();
        }

        BOOL ChannelTitle::isFirstCall() const {
            return !mbIsFirstCall;
        }

        BOOL ChannelTitle::isResetAcceptable() const {
            if (mState == 5 && (mSubState == 0xF || mSubState == 0x11)) goto return_false;
            if (mState < 0x11 || mState > 0x16) goto return_true;
        return_false:
            return FALSE;
        return_true:
            return TRUE;
        }

        BOOL ChannelTitle::isInLaunching() const {
            if (mState == 5 && System::getFader()->getStatus() != EGG::Fader::PREPARE_OUT)
                return TRUE;
            return FALSE;
        }

        BOOL ChannelTitle::isEnableAppStart() {
            return (u32)(100.0f / System::getAnimDelta()) < unk_0x88;
        }

        void ChannelTitle::getRsoExBufData(void* rsoExBuf) const {
            memcpy(rsoExBuf, System::getChannelManager()->getChannel(mRsoPage, mRsoIndex).rsoExBuf,
                   channel::RSO_EXTRA_BUFFER_LENGTH);
        }

        void ChannelTitle::setRsoExBufData(const void* rsoExBuf) {
            memcpy(System::getChannelManager()->getChannel(mRsoPage, mRsoIndex).rsoExBuf, rsoExBuf,
                   channel::RSO_EXTRA_BUFFER_LENGTH);
        }

        void* ChannelTitle::allocFromRsoExHeap(u32 size, int align) {
            return mpRsoHeapArr[mHeapIndex]->alloc(size, align);
        }

        void ChannelTitle::freeToRsoExHeap(void* buffer) {
            if (buffer != NULL) {
                mpRsoHeapArr[mHeapIndex]->free(buffer);
            }
        }

        u32 ChannelTitle::getAllocatableSizeForRsoExHeap() const {
            return mpRsoHeapArr[mHeapIndex]->getAllocatableSize(4);
        }

        layout::Animator* ChannelTitle::getRsoAnimator(int idx) const {
            return (mAnmState != 4) ? ((layout::Animator**)mTitleDataPath)[idx] : NULL;
        }

        void ChannelTitle::prepare() {
            startGetTicketLimitTask();

            mpLayoutFile = System::getNandManager()->readLayoutAsync(getSceneHeap(), "chanTtl.ash", false);
            mSomething_0x250 = (int)System::getNandManager()->readLayoutAsync(getSceneHeap(), "diskBann.ash", false);

            mpLoadResult[mSomething_0x218] = (void*)loadLayoutFileAsync(
                mChannelPage, mChannelIndex,
                &mpLoadBuf[mSomething_0x218],
                (nand::File**)&mpLoadFile[mSomething_0x218],
                &mpLoadUnk[mSomething_0x218]);

            loadModule(mChannelPage, mChannelIndex);
        }
        void ChannelTitle::create() {
            mpLayout_0x208 = new (0x580) layout::Object(
                getSceneHeap(),
                mpLayoutFile,
                "arc", "my_ChTop_a.brlyt");
            if (SCGetAspectRatio() == 1) {
                layout::Object* obj = mpLayout_0x208;
                nw4r::lyt::Pane* pane = obj->FindPaneByName("Picture_04");
                nw4r::lyt::Material* mat = pane->GetMaterial();
                GXTexObj texObj;
                mat->GetTexture(&texObj, 0);
                pane = obj->FindPaneByName("Picture_05");
                mat = pane->GetMaterial();
                mat->GetTexture(&texObj, 0);
                pane = obj->FindPaneByName("Picture_06");
                mat = pane->GetMaterial();
                mat->GetTexture(&texObj, 0);
                GXTexObj texArr[3];
                GXTexObj* texBase = texArr;
                for (int y = 0; y < 3; y++) {
                    GXTexObj* dst = texBase;
                    for (int x = 0; x < 4; x++) {
                        pane = obj->FindPaneByName(
                            lbl_8164EAE6.mPtrs[y * 4 + x]);
                        mat = pane->GetMaterial();
                        mat->SetTexture(0, *dst);
                        dst++;
                    }
                    texBase += 8;
                }
            }
            {
                nw4r::lyt::Pane* pane = mpLayout_0x208->FindPaneByName((const char*)lbl_81696AAC[0]);
                setMessage(pane, 1, 1);
            }
            channel::Manager* mgr = System::getChannelManager();
            if (mgr->checkNeedUpdate(mChannelPage, mChannelIndex)) {
                nw4r::lyt::Pane* pane = mpLayout_0x208->FindPaneByName(*(const char**)((int)&lbl_81696AAC + 4));
                setMessage(pane, 0x14, 1);
            } else {
                nw4r::lyt::Pane* pane = mpLayout_0x208->FindPaneByName(*(const char**)((int)&lbl_81696AAC + 4));
                setMessage(pane, 2, 1);
            }
            int enable = isEnableToExecute(mChannelPage, mChannelIndex);
            if (enable) {
                mStartBtnFlag = 1;
            }
            mpFc_0x94 = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                lbl_8164E8F0.mAnmNames[0], lbl_8164E7C0.mGroupNames[0], false, false);
            mpFc_0x98 = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                lbl_8164E8F0.mAnmNames[1], lbl_8164E7C0.mGroupNames[0], false, true);
            if (mpChannelSelect->mStartType == 2 && isEnableToExecute(mChannelPage, mChannelIndex)) {
                unk_0xBC = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                    lbl_8164E8F0.mAnmNames[0], lbl_8164E7C0.mGroupNames[1], false, true);
                mpFrameCtrl_0xC0 = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                    lbl_8164E8F0.mAnmNames[1], lbl_8164E7C0.mGroupNames[1], false, false);
            } else {
                unk_0xBC = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                    lbl_8164E8F0.mAnmNames[0], lbl_8164E7C0.mGroupNames[1], false, false);
                int enable2 = isEnableToExecute(mChannelPage, mChannelIndex);
                mpFrameCtrl_0xC0 = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                    lbl_8164E8F0.mAnmNames[1], lbl_8164E7C0.mGroupNames[1], false, enable2);
            }
            mpFrameCtrl_0xEC = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                lbl_8164E8F0.mAnmNames[2], lbl_8164E7C0.mGroupNames[2], false, false);
            mpFrameCtrl_0x114 = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                lbl_8164E8F0.mAnmNames[2], lbl_8164E7C0.mGroupNames[3], false, false);
            *(void**)((int)this + 0x140) = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                lbl_8164E8F0.mAnmNames[3], lbl_8164E7C0.mGroupNames[4], false, false);
            *(void**)((int)this + 0x168) = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                lbl_8164E8F0.mAnmNames[3], lbl_8164E7C0.mGroupNames[5], false, false);
            *(void**)((int)this + 0x144) = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                lbl_8164E8F0.mAnmNames[4], lbl_8164E7C0.mGroupNames[4], false, false);
            int enable3 = isEnableToExecute(mChannelPage, mChannelIndex);
            mpFrameCtrl_0x16C = (utility::FrameController*)mpLayout_0x208->bindToGroup(
                lbl_8164E8F0.mAnmNames[4], lbl_8164E7C0.mGroupNames[5], false, enable3 != 0 ? false : true);
            for (int x = 5, y = 6; x <= 6; x++) {
                *(void**)((int)this + 0xF0 + x * 4) = mpLayout_0x208->bindToGroup(
                    lbl_8164E8F0.mAnmNames[x], lbl_8164E7C0.mGroupNames[y], false, false);
            }
            for (int i = 7; i <= 9; i++) {
                *(void**)((int)this + 0x1D4 + i * 4) = mpLayout_0x208->bindToGroup(
                    lbl_8164E8F0.mAnmNames[i], (const char*)NULL, false, false);
            }
            mpLayout_0x208->finishBinding();
            ChannelTitleEventHandler* handler = new (0x10) ChannelTitleEventHandler(this);
            gui::PaneManager* mgr2 = new (0x34) gui::PaneManager(
                handler, (const nw4r::lyt::DrawInfo*)((u8*)mpLayout_0x208 + 0x298),
                System::getMem2App(), (EGG::Allocator*)NULL, false);
            mpPaneMgr_0x20C = mgr2;
            mgr2->createLayoutScene(*mpLayout_0x208->getNW4RLyt());
            mgr2->update(0);
            {
                nw4r::lyt::Pane* p = mpLayout_0x208->FindPaneByName((const char*)lbl_81696A20[0]);
                mpPaneMgr_0x20C->getPaneComponentByPane(p)->getPane()->SetVisible(true);
            }
            {
                nw4r::lyt::Pane* p = mpLayout_0x208->FindPaneByName(*(const char**)((int)&lbl_81696A20 + 4));
                mpPaneMgr_0x20C->getPaneComponentByPane(p)->getPane()->SetVisible(true);
            }
            mpLayout_0x210 = new (0x580) layout::Object(
                getSceneHeap(),
                mpLayoutFile,
                "arc", "my_Timer_a.brlyt");
            mpObject_0x214 = new (0x580) layout::Object(
                getSceneHeap(),
                mpLayoutFile,
                "arc", "my_Timer_b.brlyt");
            {
                nw4r::lyt::Pane* pane = mpObject_0x214->FindPaneByName("T_TimerMes");
                nw4r::lyt::TextBox* textBox = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(pane);
                const wchar_t* msg = System::getMessageManager()->getMessage(0x1A);
                textBox->SetString(msg);
            }
            mpLayout_0x334 = new (0x580) layout::Object(
                getSceneHeap(),
                mpLayoutFile,
                "arc", "my_ChTopMes_a.brlyt");
            mpFrameCtrl_0x338 = (utility::FrameController*)mpLayout_0x334->bind("my_ChTopMes_a_Appear.brlan", true);
            mpFrameCtrl_0x33C = (utility::FrameController*)mpLayout_0x334->bind("my_ChTopMes_a_Lost.brlan", false);
            mpLayout_0x334->finishBinding();
            updateLockedMsg(0);
            dispTimeLimitWindow();
            mpLayout_0x254 = new (0x580) layout::Object(
                getSceneHeap(),
                (nand::LayoutFile*)mSomething_0x250,
                "arc", (const char*)NULL);
            bindDiskAnms();
            {
                nw4r::lyt::Pane* pane = mpLayout_0x254->FindPaneByName("T_Bar");
                setMessage(pane, 0, 1);
            }
            {
                nw4r::lyt::Pane* pane = mpLayout_0x254->FindPaneByName("T_Comment0");
                setMessage(pane, 4, 1);
            }
            {
                nw4r::lyt::Pane* pane = mpLayout_0x254->FindPaneByName("T_Comment1");
                setMessage(pane, 6, 1);
            }
            if (!isDiskChannelByDraw(mChannelPage, mChannelIndex)) {
                mpLayout_0x254->GetRootPane()->SetVisible(false);
            }
            mpLayout_0x254->finishBinding();
            createChanLayout();
            ipl::math::HermiteIntp<f32>* hermite = new (0x30) ipl::math::HermiteIntp<f32>();
            mpTitleHermite = hermite;
            hermite->init(lbl_81696AB4, 255.0f, 28.0f, 0.0f, 0.0f, 1.0f, 0);
            hermite->setAnmType(0);
            GXRenderModeObj* rmo = System::getRenderModeObj();
            mpCapture_0x358 = new (0x34) utility::Capture(
                System::getMem2App(),
                0, 0,
                rmo->efbHeight,
                rmo->fbWidth,
                GX_TF_RGBA8);
            mCounterLimit_0x390 = 0x18CA0;
            mCounter_0x38C = 0x18CA0;
            mState = 2;
            tryToStartScene();
            mpRsoHeapArr[0] = EGG::ExpHeap::create(0x80000, System::getMem2App(), 6);
            mpRsoHeapArr[1] = EGG::ExpHeap::create(0x80000, System::getMem2App(), 6);
            *(u8*)((int)System::getSceneManager()->getScene(5) + 0x105) = 1;
        }
        void ChannelTitle::draw() {
            if (mState == 2) return;

            int drawLayer = *(int*)((char*)System::getSceneManager() + 0x100);
            if (drawLayer == 1) {
                if (mState == 3 || mState == 6) {
                    ipl::utility::Graphics::setOrtho(0);
                    _GXColor color;
                    color.r = lbl_816949FC;
                    color.g = lbl_816949FD;
                    color.b = lbl_816949FE;
                    color.a = lbl_816949FF;
                    nw4r::ut::Rect rect(
                        mDispX - mpChannelSelect->mChanThumbOff_X,
                        mDispY + mpChannelSelect->mChanThumbOff_Y,
                        mDispX + mpChannelSelect->mChanThumbOff_X,
                        mDispY - mpChannelSelect->mChanThumbOff_Y);
                    color.a = (u8)mpTitleHermite->get();
                    ipl::utility::Graphics::drawTexture(rect, mpCapture_0x358->getGXTex(), color, 1, ipl::utility::Graphics::ORI_NONE);
                    _GXColor zeroColor;
                    zeroColor.r = 0;
                    zeroColor.g = 0;
                    zeroColor.b = 0;
                    zeroColor.a = 0;
                    zeroColor.a = (u8)mpTitleHermite->get();
                    drawPolygonAroundRect(rect, zeroColor);
                    return;
                }
                ipl::utility::Graphics::setDefaultOrtho(0);
                if (mpLayout_0x23C != NULL) mpLayout_0x23C->draw();
                mpLayout_0x254->draw();
                mpLayout_0x210->draw();
                mpObject_0x214->draw();
                mpLayout_0x334->draw();
                mpLayout_0x208->draw();
                return;
            }

            if (drawLayer != 0) return;
            if (mState == 3 || mState == 6) {
                ipl::utility::Graphics::setDefaultOrtho(0);
                if (mpLayout_0x23C != NULL) mpLayout_0x23C->draw();
                mpLayout_0x254->draw();
                mpLayout_0x210->draw();
                mpObject_0x214->draw();
                mpLayout_0x334->draw();
                mpLayout_0x208->draw();
                mpCapture_0x358->capture(true);
            }
        }
        void ChannelTitle::destroy() {
            if (mpLoadResult[0] != NULL && (u32)mpLoadResult[0] != (u32)mSomething_0x250 && (u32)mpLoadResult[0] != (u32)mpAnimator_0x328) {
                delete (layout::Object*)mpLoadResult[0];
            }
            if (mpLoadResult[1] != NULL && (u32)mpLoadResult[1] != (u32)mSomething_0x250 && (u32)mpLoadResult[1] != (u32)mpAnimator_0x328) {
                delete (layout::Object*)mpLoadResult[1];
            }
            if (mpLayout_0x23C != NULL && mpLayout_0x23C != mpLayout_0x254 && mpLayout_0x23C != mpObject_0x32C) {
                mpLayout_0x23C->destroyHeap();
            }
            if (mpAnimator_0x328 != NULL) {
                delete mpAnimator_0x328;
            }
            if (mpObject_0x32C != NULL) {
                mpObject_0x32C->destroyHeap();
            }
            if (mpLoadFile[0] != NULL) {
                delete (layout::Animator*)mpLoadFile[0];
            }
            if (mpLoadFile[1] != NULL) {
                delete (layout::Animator*)mpLoadFile[1];
            }
            delete mpCapture_0x358;
            delete mpRsoLoadFile;
            mpRsoHeapArr[0]->destroy();
            mpRsoHeapArr[1]->destroy();
            mpRsoExHeap->destroy();
            System::getChannelManager()->setLoadedChJump(false);
            *(u8*)((u8*)System::getSceneManager()->getScene(5) + 0x105) = 0;
        }
        void ChannelTitle::calcCommon() {
            mpPaneMgr_0x20C->calc();
            mpLayout_0x208->calc();

            if (mSomething_0x384 == 2) {
                calcChannelCS();
            } else {
                calcChannelRso();
            }

            if (System::getChannelManager()->getSceneID(mChannelPage, mChannelIndex) == 0xF) {
                updateDiskState(mChannelPage, mChannelIndex);
                mpLayout_0x254->calc();
            } else {
                calcModuleChannel();
                System::getChannelManager()->setUnk_0x1B81(false);
            }

            if ((mState == 1 || mState == 0xE || (u32)(mState - 4) <= 1) && unk_0x90 == 0) {
                if (mpAnim_0x244 != NULL && !mpAnim_0x244->isPlaying()) {
                    if (mpAnim_0x248 != NULL) {
                        mpAnim_0x248->initFrame();
                        mpAnim_0x248->restart();
                    } else if (mpAnimContainer_0x240 != NULL) {
                        mpAnimContainer_0x240->initFrame();
                        mpAnimContainer_0x240->restart();
                    }
                    mpAnim_0x244 = NULL;
                }
            }

            if (mState == 5 && unk_0x37C == 0 && !mpFrameCtrl_0x114->isPlaying()) {
                unk_0xBC->initFrame();
                unk_0xBC->restart();
                unk_0x37C = 1;
            }

            if (mpLayout_0x23C != NULL) mpLayout_0x23C->calc();
            mpLayout_0x210->calc();
            mpObject_0x214->calc();
            mpLayout_0x334->calc();
        }

        FaderSceneCommand ChannelTitle::calcFadein() {
            if (mState == 2) {
                tryToStartScene();
                return FADER_SCN_CONTINUE;
            }
            if (!mpTitleHermite->isPlaying()) {
                initChanAnmAndSound();
                Button* btn = (Button*)System::getSceneManager()->getScene(5);
                btn->setEventHandler(mpButtonEventHandler, NULL);
                if (*(s32*)((char*)mpChannelSelect + 0xC4) != 2) {
                    btn->animation(0x18);
                    btn->animation(0x17);
                }
                mState = 1;
                return FADER_SCN_NEXT;
            }
            mpTitleHermite->calc();
            return FADER_SCN_CONTINUE;
        }
        FaderSceneCommand ChannelTitle::calcNormal() {
            switch (mState) {
            case 0: calcNormalNormal(); break;
            case 1: case 2: case 3: break;
            case 4: prepareForBoot2(mSubState); mState = 5; break;
            case 5: case 6: case 7: break;
            case 8: calcNormalChangeWait(); break;
            case 9: calcNormalChangeNext(); break;
            case 0xA: tryToGoBackward(); break;
            case 0xB: calcNormalParentalDialog(); break;
            case 0xC: calcNormalSettingDialog(); break;
            case 0xD: calcNormalControllerDialog(); break;
            case 0xE: calcNormalWaitTmd(); break;
            case 0xF: calcNormalWaitLockedTitle(); break;
            case 0x10: calcNormalLockedDiskDialog(); break;
            case 0x11: calcNormalUpdateDialog(); break;
            case 0x12: calcNormalUpdateAcceptDialog(); break;
            case 0x13: calcNormalUpdating(); break;
            case 0x14: calcNormalUpdateSucceeded(); break;
            case 0x15: calcNormalUpdateUnk1(); break;
            case 0x16: calcNormalUpdateUnk0(); break;
            case 0x17: calcNormalUpdateWaiting(); break;
            case 0x18: return FADER_SCN_CONTINUE;
            }
            if (mState == 5) {
                unk_0x388 = 1;
                unk_0x88++;
                *(u8*)((char*)System::getPointer() + 0x3D) = 0;
                if ((u32)unk_0x36C | (u32)unk_0x368) {
                    if (unk_0x370 != 0) {
                        unk_0x36C = NULL;
                        unk_0x368 = NULL;
                        SCFlushAsync(0);
                    }
                }
                if (isEnableAppStart() && unk_0x370 != 0) {
                    return FADER_SCN_NEXT;
                }
                return FADER_SCN_CONTINUE;
            } else if ((u32)(mState - 6) <= 1) {
                unk_0x388 = 1;
                return FADER_SCN_NEXT;
            }
            return FADER_SCN_CONTINUE;
        }
        void ChannelTitle::initCalcFadeout() {
            Button* btn = (Button*)System::getSceneManager()->getScene(5);
            if (mState == 5 || mState == 7) {
                System::getFader()->fadeOut();
                snd::sSystem.stopAllSound(0x14);
                OSReport("sound stopped\n");
            } else if (mState == 6) {
                btn->animation(0x1A);
                btn->animation(0x19);
            }
            btn->setEventHandler(NULL, NULL);
        }

        FaderSceneCommand ChannelTitle::calcFadeout() {
            if (mState == 5) {
                if (unk_0x348 == 0 && WPADGetStatus() == 0) {
                    unk_0x348 = OSGetTick();
                }
                if (System::getFader()->getStatus() != 0 || System::isReceiveScheduleStopped() == FALSE || (System::getNwc24Manager() != NULL && *(u8*)((u8*)System::getNwc24Manager() + 0xA31) != 0) || SCCheckStatus() == 1) {
                    return FADER_SCN_CONTINUE;
                }
                OSReport("NWC24 Scheduler stopped.\n");
                SDVFWorker* sdvfWorker = NULL;
                if (System::getCdbManager() != NULL) {
                    cdb::Manager* cdbMgr = System::getCdbManager();
                    sdvfWorker = (SDVFWorker*)((u8*)cdbMgr + 0x18);
                }
                if (mSubState == 0x11) {
                    if (!nandwall::checkNandCapacityAppBootable()) {
                        OSReport("Nand full! OSRebootSystem.\n");
                        rebootSystem();
                    }
                    while (WPADGetStatus() != 0 || System::getBS2Manager()->getIPLState() != ipl::bs2::IPL_STATE_8 || !System::getSaveData()->isFinished(mpFlushFile) || (mSomething_0x384 != 0 && mAnmState != 4) || (sdvfWorker != NULL && sdvfWorker->is_working())) {
                        snd::sSystem.calc();
                        System::getBS2Manager()->update();
                        VIWaitForRetrace();
                        if (mSomething_0x384 == 2) {
                            calcChannelCS();
                        } else {
                            calcChannelRso();
                        }
                        if (WPADGetStatus() != 0) {
                            OSReport("wait for WPAD\n");
                        }
                        if (System::getBS2Manager()->getIPLState() != ipl::bs2::IPL_STATE_8) {
                            OSReport("wait for BS2\n");
                        }
                    }
                    if (sdvfWorker != NULL) {
                        sdvfWorker->terminate_async();
                        while (!sdvfWorker->is_terminated()) {
                            snd::sSystem.calc();
                            VIWaitForRetrace();
                        }
                        // TODO: this shouldnt be here. temporarily added to match .data
                        OSReport("SDVFWorker teminated.\n");

                        OSReport("VI Black\n");
                    }
                    if (mpFlushFile != NULL) {
                        ((void(*)(void*, int))((*(void***)mpFlushFile)[2]))(mpFlushFile, 1);
                    }
                    VISetBlack(1);
                    VIFlush();
                    VIWaitForRetrace();
                    OSReport("VI Black\n");
                    while (!__OSSyncSram()) {
                        OSReport("sync sram\n");
                    }
                    ESTitleId titleId = System::getChannelManager()->getTitleID(mChannelPage, mChannelIndex);
                    u16* titleName0 = System::getChannelManager()->getTitleName(mChannelPage, mChannelIndex, 0);
                    wchar_t titleNameBuf[21];
                    memcpy(titleNameBuf, titleName0, 0x2A);
                    u16* titleName1 = System::getChannelManager()->getTitleName(mChannelPage, mChannelIndex, 1);
                    wchar_t titleNameBuf2[21];
                    memcpy(titleNameBuf2, titleName1, 0x2A);
                    u8 shops[2];
                    shops[0] = *(u8*)((int)this + 0x362);
                    shops[1] = *(u8*)((int)this + 0x363);
                    __OSCreatePlayRecord(titleNameBuf, ((char*)&titleId) + 4, (char*)shops);
                    OSReport("Create play recode\n");
                    BS2SetStateFlags();
                    OSReport("Launch\n");
                    channel::Manager* mgr = System::getChannelManager();
                    if (*(u8*)((u8*)mgr + 0x1DA0) != 0) {
                        void* bootArg = *(void**)((u8*)mgr + 0x1BA0);
                        __OSLaunchTitlelForSystem(titleId, 1, (const char*)bootArg, NULL);
                    } else {
                        __OSLaunchTitlevForSystem(titleId, 0, NULL);
                    }
                } else if (mSubState == 0xF) {
                    if (!nandwall::checkNandCapacityAppBootable()) {
                        OSReport("SDVFWorker teminated.\n");
                        rebootSystem();
                    }
                    if (sdvfWorker != NULL) {
                        while (sdvfWorker->is_working()) {
                            VIWaitForRetrace();
                        }
                        sdvfWorker->terminate_async();
                        while (!sdvfWorker->is_terminated()) {
                            snd::sSystem.calc();
                            VIWaitForRetrace();
                        }
                        OSReport("SDVFWorker teminated.\n");
                    }
                    if (mSceneType == 0xB) {
                        u16* name0 = System::getChannelManager()->getTitleName(mChannelPage, mChannelIndex, 0);
                        wchar_t name0Buf[21];
                        memcpy(name0Buf, name0, 0x2A);
                        u16* name1 = System::getChannelManager()->getTitleName(mChannelPage, mChannelIndex, 1);
                        wchar_t name1Buf[21];
                        memcpy(name1Buf, name1, 0x2A);
                        char* diskId;
                        char* diskMaker;
                        System::getBS2Manager()->getDiskInfo(&diskId, &diskMaker);
                        __OSCreatePlayRecord(name0Buf, diskId, diskMaker);
                        OSReport("Create play recode\n");
                        System::getBS2Manager()->startRVLGame();
                    } else if (mSceneType == 0xD) {
                        System::getBS2Manager()->startGCGame();
                    }
                } else if (mSubState == 0x21) {
                    if (!nandwall::checkNandCapacityAppBootable()) {
                        OSReport("SDVFWorker teminated.\n");
                        rebootSystem();
                    }
                    while (!System::getSaveData()->isFinished(mpFlushFile)) {
                        snd::sSystem.calc();
                        VIWaitForRetrace();
                        if (mSomething_0x384 == 2) {
                            calcChannelCS();
                        } else {
                            calcChannelRso();
                        }
                        if (WPADGetStatus() != 0) {
                            OSReport("wait for WPAD\n");
                        }
                        if (System::getBS2Manager()->getIPLState() != ipl::bs2::IPL_STATE_8) {
                            OSReport("wait for BS2\n");
                        }
                    }
                    if (sdvfWorker != NULL) {
                        sdvfWorker->terminate_async();
                        while (!sdvfWorker->is_terminated()) {
                            snd::sSystem.calc();
                            VIWaitForRetrace();
                        }
                        OSReport("SDVFWorker teminated.\n");
                    }
                    if (mpFlushFile != NULL) {
                        ((void(*)(void*, int))((*(void***)mpFlushFile)[2]))(mpFlushFile, 1);
                    }
                    ESTitleId titleId = System::getChannelManager()->getTitleID(mChannelPage, mChannelIndex);
                    u16* titleName0 = System::getChannelManager()->getTitleName(mChannelPage, mChannelIndex, 0);
                    wchar_t titleNameBuf[21];
                    memcpy(titleNameBuf, titleName0, 0x2A);
                    u16* titleName1 = System::getChannelManager()->getTitleName(mChannelPage, mChannelIndex, 1);
                    wchar_t titleNameBuf2[21];
                    memcpy(titleNameBuf2, titleName1, 0x2A);
                    u8 shops[2];
                    shops[0] = *(u8*)((int)this + 0x362);
                    shops[1] = *(u8*)((int)this + 0x363);
                    __OSCreatePlayRecord(titleNameBuf, ((char*)&titleId) + 4, (char*)shops);
                    OSReport("Create play recode\n");
                    System::getBS2Manager()->startRVLGame();
                } else {
                    math::VEC3 vec(0.0f, 0.0f, 0.0f);
                    utility::Graphics::setOrthoTrans(vec);
                    math::VEC2 scale(lbl_816949F8, lbl_816949F8);
                    utility::Graphics::setOrthoScale(scale);
                }
                return FADER_SCN_NEXT;
            }
            if (mState == 6) {
                if (!mpTitleHermite->isPlaying()) {
                    return FADER_SCN_NEXT;
                }
                ((void(*)(void*))((*(void***)mpTitleHermite)[3]))(mpTitleHermite);
                return FADER_SCN_CONTINUE;
            }
            if (mState == 7) {
                if (System::getFader()->getStatus() != 0) {
                    return FADER_SCN_CONTINUE;
                }
                math::VEC3 vec(0.0f, 0.0f, 0.0f);
                utility::Graphics::setOrthoTrans(vec);
                math::VEC2 scale(lbl_816949F8, lbl_816949F8);
                utility::Graphics::setOrthoScale(scale);
                snd::sSystem.initFx();
                snd::sSystem.startBGM("WIPL_BGM_MENU");
                return FADER_SCN_NEXT;
            }
            return FADER_SCN_CONTINUE;
        }

        void ChannelTitle::getRsoTitleDataPath(char* path) const {
            channel::Manager* mgr = System::getChannelManager();
            // TODO: use titleid macros
            u32 lo = mgr->getTitleID(mRsoPage, mRsoIndex) & 0xFFFFFFFF;
            u32 hi = (mgr->getTitleID(mRsoPage, mRsoIndex) >> 32) & 0xFFFFFFFF;
            sprintf(path, "/title/%08x/%08x/data/", hi, lo);
        }
        BOOL ChannelTitle::isStartAnimFinished() const {
            int r4 = 1;
            int r5 = 1;
            int bothNull = 0;
            utility::FrameController* r7 = mpAnimContainer_0x240;
            if (r7 == NULL && mpAnim_0x244 == NULL) bothNull = 1;
            if (!bothNull) {
                int check = 0;
                utility::FrameController* r6 = mpAnim_0x244;
                if (r6 != NULL && !r6->isPlaying()) check = 1;
                if (!check) r5 = 0;
            }
            if (!r5) {
                int check = 0;
                if (r7 != NULL && !r7->isPlaying()) check = 1;
                if (!check) r4 = 0;
            }
            return r4;
        }

        void ChannelTitle::createChanLayout() {
            int id = System::getChannelManager()->getSceneID(mChannelPage, mChannelIndex);
            BOOL isDisk = (id == 0xF);
            if (isDisk) {
                createDiskChanLayout();
            } else {
                createModuleChanLayout();
                if (mpLayout_0x23C != NULL) {
                    mpLayout_0x23C->finishBinding();
                }
            }
        }
        void ChannelTitle::createDiskChanLayout() {
            int idx = mSomething_0x218;
            int idxTimes4 = idx * 4;
            void* file = mpLoadResult[idx];
            if ((u32)file == (u32)mSomething_0x250) {
                mpLayout_0x23C = mpLayout_0x254;
                for (int i = 0; i < 3; i++) {
                    ((void**)((char*)this + 0x240))[i] = NULL;
                }
                for (int y = 0; y < 5; y++) {
                    for (int x = 0; x < 10; x++) {
                        void* ptr = *(void**)((char*)this + 0x258 + y * 0x28 + x * 4);
                        if (ptr != NULL) {
                            *(int*)((char*)ptr + 0x14) = 0;
                        }
                    }
                }
                ((layout::Animator*)*(utility::FrameController**)((char*)this + 0x2F8))->initAnmFrame();
                mpLayout_0x254->GetRootPane()->SetVisible(true);
                mpLayout_0x23C->calc();
            } else if (mpAnimator_0x328 != NULL && file == mpAnimator_0x328) {
                if (mpObject_0x32C == NULL) {
                    mpObject_0x32C = layout::Object::create(
                        System::getMem2App(), 0x40000,
                        (nand::LayoutFile*)file,
                        "arc", "my_GCTop_a.brlyt");
                    mpFrameCtrl_0x330 = mpObject_0x32C->bindToGroup("", "", false, true);
                    mpObject_0x32C->finishBinding();
                    mpObject_0x32C->adjustHeap();
                }
                mpLayout_0x23C = mpObject_0x32C;
                mpAnimContainer_0x240 = mpFrameCtrl_0x330;
                mpAnimContainer_0x240->setAnmType(ANIM_TYPE_LOOP);
                mpAnim_0x244 = NULL;
                mpAnim_0x248 = NULL;
                ((layout::Animator*)mpAnimContainer_0x240)->initAnmFrame();
                mpLayout_0x23C->calc();
            } else {
                void* ptr = mpLoadBuf[idx];
                if (ptr != NULL) {
                    mpLayout_0x23C = layout::Object::create(
                        mpRsoExHeap, 0x40000, (nand::LayoutFile*)ptr, "arc", "banner.brlyt");
                    ChannelObj::setLangPane(mpLayout_0x23C);
                    bindChanAnms();
                    mpLayout_0x23C->finishBinding();
                    unk_0x90 = 1;
                } else {
                    mpLayout_0x23C = NULL;
                    for (int i = 0; i < 3; i++) {
                        ((void**)((char*)this + 0x240))[i] = NULL;
                    }
                }
            }
        }
        void ChannelTitle::createModuleChanLayout() {
            if (((nand::LayoutFile*)mpLoadResult[mSomething_0x218])->checkData() == 1) {
                mpLayout_0x23C = layout::Object::create(
                    mpRsoExHeap, 0x40000,
                    (nand::LayoutFile*)mpLoadResult[mSomething_0x218],
                    "arc", "banner.brlyt");
                ChannelObj::setLangPane(mpLayout_0x23C);
                bindChanAnms();
            } else {
                mpLayout_0x23C = NULL;
                for (int i = 0; i < 3; i++) {
                    ((void**)((char*)this + 0x240))[i] = NULL;
                }
            }
        }

        void ChannelTitle::calcModuleChannel() {
            if (mState != 1) goto end;
            if (System::getChannelManager()->getUnlockChannelState(mChannelPage, mChannelIndex) == 0) goto end;
            if (mStartBtnFlag != 0) goto check_enable;
            if (isEnableToExecute(mChannelPage, mChannelIndex) != 0) goto do_update;
        check_enable:
            if (mStartBtnFlag == 0) goto end;
            if (isEnableToExecute(mChannelPage, mChannelIndex) != 0) goto end;
        do_update:
            updateLockedMsg(0);
            changeStartButton();
        end:
            ;
        }
        void ChannelTitle::calcNormalNormal() {
            if (mpChannelSelect->mStartType == 2) {
                if (!isEnableToExecute(mChannelPage, mChannelIndex)) {
                    ((Button*)System::getSceneManager()->getScene(5))->animation(0x18);
                    ((Button*)System::getSceneManager()->getScene(5))->animation(0x17);
                    System::getChannelManager()->setLoadedChJump(false);
                    snd::sSystem.startSE("WIPL_SE_GRAY_BUTTON");
                } else {
                    reserveNextScene();
                    mpFrameCtrl_0x114->initFrame();
                    mpFrameCtrl_0x114->restart();
                    snd::sSystem.startSE("WIPL_SE_DECIDE");
                }
                mpChannelSelect->mStartType = 1;
            } else {
                Button* button = (Button*)System::getSceneManager()->getScene(5);
                if (button != NULL && button->isActive()) {
                    button->update();
                }

                if (mState == 1) {
                    controller::Interface* con = System::getMasterController();
                    if (con->down(controller::BTN_NEXT_LEFT)) {
                        int page, index;
                        searchChannel(1, &page, &index);
                        startChangeChannel(page, index);
                        snd::sSystem.startSE("WSD_SELECT");
                    } else if (con->down(controller::BTN_NEXT_RIGHT)) {
                        int page, index;
                        searchChannel(0, &page, &index);
                        startChangeChannel(page, index);
                        snd::sSystem.startSE("WSD_SELECT");
                    } else {
                        mpPaneMgr_0x20C->update();
                        if (isEnableToExecute(mChannelPage, mChannelIndex)) {
                            if (mStartBtnFlag == 0) {
                                mStartBtnFlag = 1;
                            } else if (mStartBtnFlag == 1) {
                                mStartBtnFlag = 2;
                            }
                        }
                    }
                }
            }
        }
        void ChannelTitle::calcNormalChangeWait() {
            if (mpFrameCtrl_0x1F0->isPlaying()) return;

            int otherSlot = 1 - mSomething_0x218;

            if (mpLoadResult[otherSlot] != NULL) {
                if (!((nand::LayoutFile*)mpLoadResult[otherSlot])->isFinished()) goto fallthrough;
            }
            if (mpLoadFile[otherSlot] != NULL) {
                if (!((nand::File*)mpLoadFile[otherSlot])->isFinished()) goto fallthrough;
            }

            if (mpFrameCtrl_0x1C0->isPlaying()) goto fallthrough;
            if (mpFrameCtrl_0x1C4->isPlaying()) goto fallthrough;
            if (unk_0x371 == 0) goto fallthrough;
            if (!System::getChannelManager()->isLoadedLockedMsg()) goto fallthrough;
            if (mpFrameCtrl_0x33C->isPlaying()) goto fallthrough;
            if (mSomething_0x384 != 0 && mAnmState != 4) goto fallthrough;

            if (mpFrameCtrl_0x1F4->isPlaying()) {
                mpFrameCtrl_0x1F4->stop();
            }

            if (mpLoadResult[mSomething_0x218] == (void*)mSomething_0x250) {
                mpLayout_0x254->GetRootPane()->SetVisible(false);
            } else if (mpLoadResult[mSomething_0x218] != NULL) {
                if (mpLoadResult[mSomething_0x218] != mpAnimator_0x328) {
                    if (mpLoadResult[mSomething_0x218] == NULL) {
                        if (mpLoadBuf[mSomething_0x218] != NULL && mpLayout_0x23C != NULL) {
                            mpLayout_0x23C->destroyHeap();
                        }
                    } else {
                        if (mpLoadBuf[mSomething_0x218] == NULL) {
                            delete (nand::LayoutFile*)mpLoadResult[mSomething_0x218];
                        }
                        if (mpLayout_0x23C != NULL) {
                            mpLayout_0x23C->destroyHeap();
                        }
                    }
                }
            }

            mpLoadResult[mSomething_0x218] = NULL;
            mpLayout_0x23C = NULL;
            mpRsoHeapArr[mHeapIndex]->freeAll();
            mHeapIndex ^= 1;
            dispTimeLimitWindow();
            snd::sSystem.stopBannerSound(0);
            snd::sSystem.stopSE((void*)_pad_0x24C, 0);

            mpLoadFile[mSomething_0x218] = NULL;
            mpLoadUnk[mSomething_0x218] = NULL;
            mSomething_0x218 = 1 - mSomething_0x218;
            loadModule(mChannelPage, mChannelIndex);
            createChanLayout();
            mpFrameCtrl_0x1F8->initFrame();
            mpFrameCtrl_0x1F8->restart();

            if (mpFrameCtrl_0x1C0->getCurrentFrame() == mpFrameCtrl_0x1C0->getMaxFrame()) {
                const char* paneName = (const char*)lbl_81696AAC[1];
                bool needUpdate = System::getChannelManager()->checkNeedUpdate(mChannelPage, mChannelIndex);
                nw4r::lyt::Pane* pane = mpLayout_0x208->GetRootPane()->FindPaneByName(paneName, true);
                if (needUpdate) {
                    setMessage(pane, 0x14, 0);
                } else {
                    setMessage(pane, 0x2, 0);
                }

                mpFrameCtrl_0x1C4->initFrame();
                mpFrameCtrl_0x1C4->restart();
                ((layout::Animator*)mpFrameCtrl_0x1C0)->initAnmFrame();
                mpFrameCtrl_0x1C0->stop();
            }

            changeStartButton();

            if (System::getChannelManager()->getUnlockChannelState(mChannelPage, mChannelIndex) == 1) {
                updateLockedMsg(1);
            }

            mState = 9;
            goto end;

        fallthrough:
            if (!mpFrameCtrl_0x1F4->isPlaying()) {
                mpFrameCtrl_0x1F4->initFrame();
                mpFrameCtrl_0x1F4->restart();
            }
        end:
            ;
        }
        void ChannelTitle::calcNormalChangeNext() {
            if (mpFrameCtrl_0x1F8->isPlaying()) return;
            initChanAnmAndSound();
            mState = 1;
        }
        void ChannelTitle::calcNormalParentalDialog() {
            if (mSubState == 0xF) {
                if (System::getBS2Manager()->getIPLState() != 5 ||
                    System::getChannelManager()->isUnk_0x1B81())
                {
                    unk_0x70 = (void*)2;
                }
            }

            SceneObj* scene = System::getSceneManager()->getScene(0x1B);
            if (scene == NULL) {
                if (System::getSceneManager()->getReservedScene() != NULL) {
                    goto found;
                }
            } else {
                goto found;
            }

            // Not found path
            if ((int)unk_0x70 == 1) {
                if (mSubState == 0x12) {
                    reserveSettingScene(true);
                } else if (mSubState == 0x11) {
                    prepareForBoot2(mSubState);
                    mState = 5;
                } else if (mSubState == 0xF) {
                    prepareForBoot2(mSubState);
                    System::getBS2Manager()->reserveRVLGame();
                    mState = 5;
                } else if (mSubState == 0x21) {
                    mState = 0xF;
                }
            } else {
                ((Button*)System::getSceneManager()->getScene(5))->animation(0x18);
                ((Button*)System::getSceneManager()->getScene(5))->animation(0x17);
                unk_0xBC->initFrame();
                unk_0xBC->restart();
                System::getHomeButtonMenu()->enable();
                System::getChannelManager()->setLoadedChJump(false);
                mState = 1;
            }
            return;

        found:
            if (scene == NULL) return;
            if (*(int*)((u8*)scene + 0x70) == 2) {
                unk_0x70 = (void*)2;
            } else if (*(int*)((u8*)scene + 0x70) >= 3) {
                if (*(int*)((u8*)scene + 0x70) < 4) {
                    unk_0x70 = (void*)2;
                }
            }
            if (*(int*)((u8*)scene + 0x70) >= 1) {
                if ((int)unk_0x70 == 0) {
                    prepareForBoot1(mSubState);
                    if (mSubState == 0x21) {
                        System::getChannelManager()->loadLockedTitleAsync(
                            mChannelPage, mChannelIndex, msUnlockTicket);
                    }
                    unk_0x70 = (void*)1;
                }
            }
        }
        void ChannelTitle::calcNormalSettingDialog() {
            switch (System::getDialog()->getLastResult()) {
            case 2: {
                ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x18);
                ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x17);
                unk_0xBC->initFrame();
                unk_0xBC->restart();
                System::getChannelManager()->setLoadedChJump(false);
                mState = 1;
                break;
            }
            case 1:
                if (isSetParental()) {
                    startParentalDialog(1);
                } else {
                    reserveSettingScene(TRUE);
                }
                break;
            }
        }

        void ChannelTitle::calcNormalControllerDialog() {
            switch (System::getDialog()->getLastResult()) {
            case 0:
                ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x18);
                ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x17);
                unk_0xBC->initFrame();
                unk_0xBC->restart();
                mState = 1;
                break;
            }
        }

        void ChannelTitle::calcNormalWaitTmd() {
            if (!mTmdReady) return;

            if (mSubState == 0xF) {
                if (mpTmdView_0x35C != NULL) {
                    mpRsoExHeap->free(mpTmdView_0x35C);
                }
                mState = 4;
                return;
            }

            {
                int flag = *(u16*)((u8*)System::getChannelManager() + mChannelPage * 0x540 + mChannelIndex * 0x70 + 0x16);
                BOOL hasView;
                if (flag != 0) { hasView = TRUE; } else { hasView = FALSE; }
                if (!hasView) {
                    if (!utility::ESMisc::CheckTmdCountryCode(mpTmdView_0x35C)) {
                        System::getErrorHandler()->set(ErrorHandler::DEFAULT, 3, NULL, 0, -1);
                    }
                }
            }

            mTmdGroupId = mpTmdView_0x35C->head.groupId;

            if (CheckParentalControl(mpTmdView_0x35C)) {
                prepareForBoot1(mSubState);
                if (mSubState == 0x21) {
                    System::getChannelManager()->loadLockedTitleAsync(mChannelPage, mChannelIndex, msUnlockTicket);
                    mState = 0xF;
                } else {
                    mState = 4;
                }
            } else {
                System::getHomeButtonMenu()->enable();
                startParentalDialog(0);
            }

            if (mpTmdView_0x35C != NULL) {
                mpRsoExHeap->free(mpTmdView_0x35C);
            }
        }
        void ChannelTitle::calcNormalWaitLockedTitle() {
            if (unk_0x37C == 0) {
                if (System::getSceneManager()->getScene(0x1B) != NULL) return;
                if (System::getSceneManager()->getReservedScene() != NULL) return;
                if (mpFrameCtrl_0x114->isPlaying()) return;
                utility::FrameController* fc30 = unk_0xBC;
                fc30->initFrame();
                fc30->restart();
                unk_0x37C = 1;
                return;
            }

            int state = System::getChannelManager()->getUnlockChannelState(mChannelPage, mChannelIndex);
            if (state == 4) {
                prepareForBoot2(mSubState);
                System::getBS2Manager()->reserveRVLGame();
                mState = 5;
                return;
            }
            int state2 = System::getChannelManager()->getUnlockChannelState(mChannelPage, mChannelIndex);
            if (state2 == 3) return;

            System::getHomeButtonMenu()->enable();
            System::getChannelManager()->setLoadedChJump(false);

            if (System::getBS2Manager()->getIPLState() == 7) {
                if (*(u8*)((u8*)System::getSceneManager()->getScene(5) + 0xD5) != 0) {
                    ((Button*)System::getSceneManager()->getScene(5))->animation(0x1A);
                    ((Button*)System::getSceneManager()->getScene(5))->animation(0x19);
                }
                System::getDialog()->callBtn1(6, 0x2E);
                mState = 0x10;
            } else {
                if (unk_0x70 != 0) {
                    ((Button*)System::getSceneManager()->getScene(5))->animation(0x18);
                    ((Button*)System::getSceneManager()->getScene(5))->animation(0x17);
                }
                mState = 1;
            }
        }
        void ChannelTitle::calcNormalLockedDiskDialog() {
            switch (System::getDialog()->getLastResult()) {
            case 1:
                ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x18);
                ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x17);
                mState = 1;
                break;
            }
        }

        void ChannelTitle::calcNormalUpdateDialog() {
            if (System::getBS2Manager()->getIPLState() != 3) {
                System::getDialog()->terminate();
                mState = 0x15;
                return;
            }
            if (System::getDialog()->getLastResult() == 1) goto body;
            goto end;
        body:
            if ((u32)System::getRegion() == 2) {
                if ((u32)System::getLanguage() == 2) {
                    System::getDialog()->callBtn1(0x17A, 0x179, 63.0f);
                } else {
                    System::getDialog()->callBtn1Sml(0x17A, 0x179);
                }
            } else if ((u32)System::getLanguage() == 4 || (u32)System::getLanguage() == 3) {
                System::getDialog()->callBtn1(0x17A, 0x178, 63.0f);
            } else {
                System::getDialog()->callBtn1Sml(0x17A, 0x178);
            }
            mState = 0x12;
        end:
            ;
        }
        void ChannelTitle::calcNormalUpdateAcceptDialog() {
            switch (System::getBS2Manager()->getIPLState()) {
            default:
                System::getDialog()->terminate();
                mState = 0x15;
                break;
            case 3:
                switch (System::getDialog()->getLastResult()) {
                case 1:
                    System::getNandManager()->closeContentsAll();
                    SCSetUpdateType(1);
                    SCFlush();
                    System::getBS2Manager()->startUpdate();
                    System::getDialog()->callBtnPrg(0x162);
                    mState = 0x13;
                    break;
                }
                break;
            }
        }

        void ChannelTitle::calcNormalUpdating() {
            int status = System::getBS2Manager()->getIPLState();
            u32 total = System::getBS2Manager()->getEntrySize();
            u32 current = System::getBS2Manager()->getEntryOffset();
            if ((int)(current / (total / 100)) != 100) {
                System::getDialog()->setProgBarLength((int)(current / (total / 100)));
            }
            if (status == 1 || (u32)(status - 9) <= 1u) {
                System::getDialog()->terminate();
                mState = 0x15;
            } else if ((u32)(status - 4) <= 1u) {
                System::getDialog()->setProgBarLength(100);
                mState = 0x14;
            }
        }

        void ChannelTitle::calcNormalUpdateSucceeded() {
            DialogWindow* dw = System::getDialog();
            if (dw->getLastResult() != -1) {
                dw->callBtn0(0x163, 0xB4, 0);
                mState = 0x16;
            }
        }

        void ChannelTitle::calcNormalUpdateUnk1() {
            DialogWindow* dw = System::getDialog();
            if (dw->getLastResult() != -1) {
                dw->callBtn1(0x164, 0x1);
                mState = 0x17;
            }
        }

        void ChannelTitle::calcNormalUpdateUnk0() {
            DialogWindow* dw = System::getDialog();
            switch (dw->getLastResult()) {
            default:
                break;
            case 0:
                System::getResetHandler()->reset();
                mState = 0x18;
                break;
            }
        }

        void ChannelTitle::calcNormalUpdateWaiting() {
            DialogWindow* dw = System::getDialog();
            if (dw->getLastResult() > 0) {
                System::getResetHandler()->reset();
                mState = 0x18;
            }
        }

        void ChannelTitle::tryToStartScene() {
            if (mpChannelSelect->tellStartingZoomAnm()) return;
            utility::FrameController* fc = mpTitleHermite;
            fc->initFrame();
            fc->restart();
            mState = 3;
        }

        void ChannelTitle::tryToGoBackward() {
            if (mSomething_0x384 != 0) {
                if (mAnmState != 4) goto set_flag;
            }

            if (!mpChannelSelect->prepareRestarting(mChannelPage)) goto end;
            mpChannelSelect->restart(mChannelPage, mChannelIndex);
            mpTitleHermite->init(
                0.0f, 255.0f, lbl_816949F4, 0.0f, 0.0f, 0, lbl_816949F8);
            mpTitleHermite->setAnmType(1);
            mpTitleHermite->initFrame();
            mpTitleHermite->restart();
            mState = 6;
            snd::sSystem.startSE("WIPL_SE_CH_UNSELECT");
            if (snd::sBannerSoundPlayer.getUnk_0x00()) {
                snd::sSystem.stopBannerSound(0x1C);
            }
            if ((void*)_pad_0x24C != NULL && *(int*)(void*)_pad_0x24C != 0) {
                snd::sSystem.stopSE((void*)_pad_0x24C, 0x1C);
            }
            goto end;

        set_flag:
            unk_0x388 = 1;

        end:
            ;
        }
        void ChannelTitle::loadModule(int page, int index) {
            if (System::isSafeMode()) return;

            if (mSomething_0x384 != 0) {
                if (mpRsoLoadFile != NULL) {
                    delete mpRsoLoadFile;
                    mpRsoLoadFile = NULL;
                }
                mpRsoHeader = NULL;
            }

            channel::Manager* mgr = System::getChannelManager();
            u8 loaded = *(u8*)((u8*)mgr + page * 0x540 + index * 0x70 + 0x24);
            int rsoThing = 0;
            if (loaded != 0) {
                int metaHdr = *(int*)((u8*)mgr + page * 0x540 + index * 0x70 + 0x10);
                rsoThing = (*(int*)((char*)metaHdr + 0x58) >> 16) & 0xF;
            }

            if (rsoThing != 0) {
                mpRsoLoadFile = mgr->loadBannerRsoAsync((EGG::Heap*)mTitleType, page, index);
                mbIsFirstCall = 0;
                mSomething_0x384 = 1;
                mAnmState = 1;
                mRsoPage = page;
                mRsoIndex = index;
                return;
            }

            int csThing = 0;
            if (loaded != 0) {
                int metaHdr = *(int*)((u8*)mgr + page * 0x540 + index * 0x70 + 0x10);
                csThing = (*(int*)((char*)metaHdr + 0x58) >> 12) & 0xF;
            }

            if (csThing != 0) {
                mpRsoLoadFile = mgr->loadBannerCSAsync((EGG::Heap*)mTitleType, page, index);
                mbIsFirstCall = 0;
                mSomething_0x384 = 2;
                mAnmState = 1;
                mRsoPage = page;
                mRsoIndex = index;
                return;
            }

            mSomething_0x384 = 0;
            mAnmState = 0;
        }
        void ChannelTitle::calcChannelCS() {
            if (mSomething_0x384 != 2) return;
            if (mState == 2) return;
            if (mState == 3) return;
            if (mState == 9) return;

            switch (mAnmState) {
            case 2:
                {
                    channel::ChannelScriptManager* mgr = System::getCSManager();
                    mgr->calc();
                    void* thread = mpRsoThread;
                    void** vtable = *(void***)thread;
                    int (*isDone)(void*) = (int (*)(void*))vtable[0x20 / 4];
                    if (isDone(thread)) {
                        if (*(int*)((u8*)mgr + 0x34) == 1) {
                            startChanSound();
                            *(int*)((u8*)mgr + 0x34) = 0;
                        }
                        mgr->finish();
                        mAnmState = 3;
                    }
                }
                return;
            case 3:
                calcCSFinish();
                return;
            case 0:
                calcModuleInit();
                return;
            case 1:
                calcCSWait();
                return;
            }
        }
        void ChannelTitle::calcChannelRso() {
            if (mSomething_0x384 != 1) return;
            if (mState == 2 || mState == 3 || mState == 9) return;

            if (mAnmState == 1) {
                if (!mpRsoLoadFile->isFinished()) return;

                if (mpRsoLoadFile->checkData() == 1) goto setupRso;
                if (mpRsoLoadFile->checkData() == 0) goto setupRso;

                mCounter_0x38C = 0;
                mbIsFirstCall = 1;
                mAnmState = 0;
                return;

            setupRso:
                mpRsoHeader = (RSOObjectHeader*)mpRsoLoadFile->getBuffer();
                unk_0x3F0 = NULL;

                if (*(int*)((char*)mpRsoHeader + 0x1c) != 0) {
                    unk_0x3F0 = new ((EGG::Heap*)mTitleType, (int)32) u8[*(u32*)((char*)mpRsoHeader + 0x1c)];
                    memset(unk_0x3F0, 0, *(u32*)((char*)mpRsoHeader + 0x1c));
                }

                RSOLinkList(mpRsoHeader, unk_0x3F0);

                if (RSOIsImportSymbolResolvedAll(mpRsoHeader)) {
                    OSReport("Module's ImportSymbol is resolved all.\n");
                } else {
                    OSReport("%d module's ImportSymbols are not resolved.\n",
                        RSOGetNumImportSymbolsUnresolved(mpRsoHeader));
                }

                unk_0x3F4 = RSOFindExportSymbolAddr(mpRsoHeader, "Calc");

                channel::ThreadCalcFunc createFunc = (channel::ThreadCalcFunc)RSOFindExportSymbolAddr(
                    mpRsoHeader, "ThreadCalc");
                mpRsoThread->setCalcFunc(createFunc);

                (*(void(**)(void*))((char*)mpRsoHeader + 0x24))(mpRsoHeader);

                void* createFunc2 = RSOFindExportSymbolAddr(
                    mpRsoHeader, "Create");
                if (createFunc2 != NULL) {
                    ((void (*)(void*))createFunc2)((char*)mpLayout_0x23C + 4);
                }

                mpRsoThread->start();

                unk_0x389 = 0;
                unk_0x3F8 = 0;
                mAnmState = 2;
                return;
            }

            if (mAnmState == 0) { calcModuleInit(); return; }
            if (mAnmState >= 3) return;

            if (unk_0x3F8 == 0) {
                if (mpRsoThread->IsThreadTerminated()) {
                    mpRsoThread->WaitForThreadExit();
                    unk_0x3F8 = 1;
                }
            }

            if (unk_0x389 == 0) {
                if (((channel::CalcFunc)unk_0x3F4)(unk_0x388 == 1)) {
                    unk_0x389 = 1;
                }
            }

            if (unk_0x3F8 != 0 && unk_0x389 != 0) {
                mCounter_0x38C = 0;
                mbIsFirstCall = 1;

                (*(void(**)(void*))((char*)mpRsoHeader + 0x28))(mpRsoHeader);
                RSOUnLinkList(mpRsoHeader);
                unk_0x3F4 = NULL;
                mpRsoThread->setCalcFunc(NULL);
                __dla(unk_0x3F0);
                unk_0x3F0 = NULL;

                int heapIdx = 1 - mHeapIndex;
                mpRsoHeapArr[heapIdx]->freeAll();
                mAnmState = 0;
            }
        }
        void ChannelTitle::calcModuleInit() {
            if (unk_0x388) {
                unk_0x388 = 0;
                mCounter_0x38C = 0;
                mAnmState = 4;
                return;
            }
            mCounter_0x38C++;
            if (mCounter_0x38C > mCounterLimit_0x390) {
                int heapIdx = mHeapIndex;
                mAnmState = 1;
                mHeapIndex = heapIdx ^ 1;
            }
            if (System::getChannelManager()->checkUseAltSound(mChannelPage, mChannelIndex)) {
                if (mCounter_0x38C == mCounterLimit_0x390 - 0xF0) {
                    snd::sSystem.stopBannerSound(0xB4);
                }
            }
        }

        void ChannelTitle::calcCSWait() {
            nand::File* file = (nand::File*)mpRsoLoadFile;
            if (!file->isFinished()) return;

            if (file->checkData() == 1) goto setup;
            if (file->checkData() == 0) goto setup;

            mCounter_0x38C = 0;
            mbIsFirstCall = 1;
            mAnmState = 0;
            return;

        setup:
            {
                ESTitleId titleId = System::getChannelManager()->getTitleID(mChannelPage, mChannelIndex);

                if (mDebugRsoInterval == NULL) {
                    mDebugRsoInterval = EGG::ExpHeap::create((u32)-1, (EGG::Heap*)mTitleType, 0);
                }

                System::getCSManager()->create((EGG::ExpHeap*)mDebugRsoInterval);

                typedef struct { u64 data[4]; } CSTemplate;
                CSTemplate tmp = { 0ULL, 0ULL, 0ULL, 0x0000010000000000ULL };

                channel::ChannelScriptManager::CSData* data = (channel::ChannelScriptManager::CSData*)((char*)&tmp + 4);
                data->heap = mpRsoHeapArr[mHeapIndex];
                data->layout = mpLayout_0x23C;
                data->anims = (layout::Animator**)mTitleDataPath;
                data->titleId = titleId;
                data->threadTerminated = mbIsFirstCall;

                nwc24::Manager* nwc24 = System::getNwc24Manager();
                data->unk_0x19 = (nwc24 != NULL && nwc24->isNewMessageThere((u32)data->titleId)) ? 1 : 0;

                System::getCSManager()->setData(*data);

                int initResult = System::getCSManager()->init(file, mpRsoThread);
                if (initResult != 0) {
                    mAnmState = 3;
                } else {
                    mAnmState = 2;
                }
            }
        }
        void ChannelTitle::calcCSFinish() {
            mbIsFirstCall = 1;
            System::getCSManager()->destroy();
            mCounter_0x38C = 0;
            mpRsoHeapArr[1 - mHeapIndex]->freeAll();
            mDebugRsoInterval->destroy();
            mDebugRsoInterval = NULL;
            mAnmState = 0;
        }

        void ChannelTitle::initChanAnmAndSound() {
            if (isDiskChannelByDraw(mChannelPage, mChannelIndex)) return;

            nw4r::lyt::Pane* rootPane = mpLayout_0x254->GetRootPane();
            rootPane->SetVisible(false);

            utility::FrameController* anm = mpAnim_0x244;
            if (anm != NULL) {
                anm->initFrame();
                anm->restart();
                unk_0x90 = 0;
            } else {
                anm = mpAnim_0x248;
                if (anm != NULL) {
                    anm->initFrame();
                    anm->restart();
                } else {
                    anm = mpAnimContainer_0x240;
                    if (anm != NULL) {
                        anm->initFrame();
                        anm->restart();
                    }
                }
            }

            if (mState != 6) {
                if (System::getChannelManager()->checkUseAltSound(mChannelPage, mChannelIndex) == 0
                    || mSomething_0x384 == 0) {
                    startChanSound();
                }
            }
        }
        void ChannelTitle::startChanSound() {
            typedef ipl::nand::File File;
            if (mpLoadFile[mSomething_0x218] != NULL) {
                if (((File*)mpLoadFile[mSomething_0x218])->checkData() == 1) {
                    void* buf = ((File*)mpLoadFile[mSomething_0x218])->getBuffer();
                    u32 size = System::getChannelManager()->getSoundSize(
                        mChannelPage, mChannelIndex);
                    snd::sSystem.startBannerSound(buf, size, false);
                }
            } else if (mSceneType == 0xB) {
                if (mpLoadUnk[mSomething_0x218] != NULL) {
                    char* diskInfo;
                    bool isChannel = false;
                    System::getBS2Manager()->getDiskInfo(&diskInfo, NULL);
                    u32 val = *(u32*)diskInfo & 0xFFFFFF00;
                    for (int i = 0; i < 2; i++) {
                        ESTitleId entry = lbl_816102E0[i];
                        u32 hi = (u32)entry;
                        u32 lo = (u32)(entry >> 32);
                        u32 check = val ^ hi;
                        if ((check | lo) == 0) {
                            isChannel = true;
                        }
                    }
                    u32 size = System::getChannelManager()->getSoundSize(mChannelPage, mChannelIndex);
                    snd::sSystem.startBannerSound(mpLoadUnk[mSomething_0x218], size, isChannel);
                }
            } else if (mSceneType == 0xD) {
                _pad_0x24C = snd::sSystem.startSE("WIPL_ME_GC_BANNER");
            }
        }
        void ChannelTitle::bindChanAnms() {
            bindChanBaseAnms();
            if (mSomething_0x384 != 0) {
                memset(mTitleDataPath, 0, sizeof(mTitleDataPath));
                ChannelObj::bindRsoAnm(mpLayout_0x23C,
                    (layout::Animator**)mTitleDataPath, "banner");
            }
        }
        void ChannelTitle::bindChanBaseAnms() {
            int i = 0;
            do {
                if (mpLayout_0x23C->searchFile(lbl_8164E968[i])) {
                    if (i == 0) {
                        layout::Animator** slot = (layout::Animator**)((char*)this + i*4 + 0x240);
                        *slot = mpLayout_0x23C->bind(lbl_8164E968[i], true);
                        (*slot)->setAnmType(2);
                    } else if (i == 1) {
                        layout::Animator** slot = (layout::Animator**)((char*)this + i*4 + 0x240);
                        *slot = mpLayout_0x23C->bind(lbl_8164E968[i], true);
                        (*slot)->setAnmType(0);
                    } else {
                        layout::Animator** slot;
                        if (mpAnim_0x244 == NULL) {
                            slot = (layout::Animator**)((u8*)this + i*4 + 0x240);
                            *slot = mpLayout_0x23C->bind(lbl_8164E968[i], true);
                        } else {
                            slot = (layout::Animator**)((u8*)this + i*4 + 0x240);
                            *slot = mpLayout_0x23C->bind(lbl_8164E968[i], false);
                        }
                        (*slot)->setAnmType(2);
                    }
                } else {
                    *(layout::Animator**)((char*)this + i*4 + 0x240) = NULL;
                }
                i++;
            } while (i < 3);
        }
        void ChannelTitle::bindDiskAnms() {
            memset(_pad_0x258, 0, 0xC8);

            ((void**)_pad_0x258)[0xA0 >> 2] = mpLayout_0x254->bind(lbl_8164EAAC[0], true);
            ((void**)_pad_0x258)[0] = mpLayout_0x254->bindToGroup(lbl_8164EAAC[0], lbl_8164E988[0], false, false);

            for (int i = 0, baseOff = 0, groupIdx = 0; i <= 2; i++, baseOff += 0x28, groupIdx++) {
                for (int j = 1; j <= 5; j++) {
                    if (i == 0) {
                        if (j == 5) break;
                    }
                    *(void**)((char*)this + baseOff + 0x258 + j * 4) =
                        mpLayout_0x254->bindToGroup(
                            lbl_8164EAAC[j], lbl_8164E988[groupIdx],
                            false, false);
                }
            }

            for (int j = 6; j <= 9; j++) {
                *(void**)((char*)this + 0x2F8 + j * 4) =
                    mpLayout_0x254->bind(lbl_8164EAAC[j], false);
            }

            *(void**)((char*)this + 0x2E4) = mpLayout_0x254->bindToGroup(
                lbl_8164EAAC[5], lbl_8164E988[3], false, false);
        }
        void ChannelTitle::updateDiskState(int page, int index) {
            s32 state = mState;
            int diskFlag = 0;

            if (((state == 3 && !mpTitleHermite->isPlaying())) || (state == 9 && !mpFrameCtrl_0x1F8->isPlaying())) {
                diskFlag = 1;
            }

            int diskState = System::getBS2Manager()->getIPLState();
            if (state == 1 && diskState == 3) {
                System::getHomeButtonMenu()->disable();
                ((scene::Button*)System::getScene(5))->animation(0x1A);
                ((scene::Button*)System::getScene(5))->animation(0x19);
                if (*(u8*)((char*)System::getBS2Manager() + 0x11) != 0) {
                    System::getDialog()->callBtn1(0x17B, 0x2E);
                } else {
                    System::getDialog()->callBtn1(0x165, 0x2E);
                }
                mState = 0x11;
            }

            if (mState >= 0x11 && mState <= 0x18) return;

            switch (mSceneType) {
            case 0:
                if (!diskFlag && mSceneArg != 0xE) break;
                (*(utility::FrameController**)((char*)this + 0x2F8))->play();
                {
                    nw4r::lyt::GroupContainer* container = *(nw4r::lyt::GroupContainer**)((char*)mpLayout_0x254 + 0x18);
                    nw4r::lyt::Group* group = container->FindGroupByName(lbl_8164E988[0]);
                    typedef nw4r::lyt::PaneLinkList PaneList;
                    for (PaneList::Iterator it = group->GetPaneList().GetBeginIter(); it != group->GetPaneList().GetEndIter(); ++it) {
                        it->mTarget->SetVisible(diskState != 1);
                    }
                }
                if (diskState == 1) {} else {
                    _pad_0x24C = snd::sSystem.startSE("WIPL_ME_NO_DISC_BANNER");
                }
                mSceneType = 1;
                mSceneArg = 0;
                break;

            case 1:
                if ((*(utility::FrameController**)((char*)this + 0x2F8))->isPlaying()) break;
                if ((*(utility::FrameController**)((char*)this + 0x258))->isPlaying()) break;
                mSceneType = 2;
                if (diskState == 1) {
                    nw4r::lyt::GroupContainer* container = *(nw4r::lyt::GroupContainer**)((char*)mpLayout_0x254 + 0x18);
                    nw4r::lyt::Group* group = container->FindGroupByName(lbl_8164E988[0]);
                    typedef nw4r::lyt::PaneLinkList PaneList;
                    for (PaneList::Iterator it = group->GetPaneList().GetBeginIter(); it != group->GetPaneList().GetEndIter(); ++it) {
                        it->mTarget->SetVisible(true);
                    }
                }
                break;

            case 2:
                if (diskState != 1) {
                    for (int i = 0; i < 3; i++) {
                        utility::FrameController* fc = *(utility::FrameController**)((char*)this + 0x25C + i * 0x28);
                        fc->play();
                        mSceneType = 6;
                    }
                }
                break;

            case 6:
                if ((*((utility::FrameController**)(_pad_0x258 + 0x2C)))->isPlaying()) break;
                {
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x30);
                    fc->play();
                    fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x58);
                    fc->play();
                }
                (*((utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 4)))->stop();
                mSceneType = 7;
                break;

            case 7:
                if (diskState == 5) {
                    (*(utility::FrameController**)((char*)this + 0x2B0))->stop();
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x60);
                    fc->play();
                    mSceneType = 9;
                    mSceneArg = 0xA;
                } else if (diskState == 6) {
                    (*(utility::FrameController**)((char*)this + 0x288))->stop();
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x38);
                    fc->play();
                    mSceneType = 0xD;
                    {
                        int idx = mSomething_0x218;
                        mpLoadResult[idx] = (void*)loadLayoutFileAsync(page, index, &mpLoadBuf[idx], (nand::File**)&mpLoadFile[idx], &mpLoadUnk[idx]);
                    }
                    mSceneType = 9;
                    mSceneArg = 0xC;
                } else if (diskState == 7) {
                    (*(utility::FrameController**)((char*)this + 0x288))->stop();
                    (*(utility::FrameController**)((char*)this + 0x2B0))->stop();
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x38);
                    fc->play();
                    fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x60);
                    fc->play();
                    mSceneType = 9;
                    mSceneArg = 0xF;
                } else if (diskState == 1) {
                    (*(utility::FrameController**)((char*)this + 0x288))->stop();
                    (*(utility::FrameController**)((char*)this + 0x2B0))->stop();
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x34);
                    fc->play();
                    fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x5C);
                    fc->play();
                    mSceneType = 8;
                }
                break;

            case 8:
                if ((*(utility::FrameController**)((char*)this + 0x28C))->isPlaying()) break;
                if ((*(utility::FrameController**)((char*)this + 0x2B4))->isPlaying()) break;
                {
                    nw4r::lyt::GroupContainer* container = *(nw4r::lyt::GroupContainer**)((char*)mpLayout_0x254 + 0x18);
                    nw4r::lyt::Group* group = container->FindGroupByName(lbl_8164E988[0]);
                    typedef nw4r::lyt::PaneLinkList PaneList;
                    for (PaneList::Iterator it = group->GetPaneList().GetBeginIter(); it != group->GetPaneList().GetEndIter(); ++it) {
                        it->mTarget->SetVisible(true);
                    }
                }
                (*(utility::FrameController**)((char*)this + 0x258))->play();
                mSceneType = 1;
                break;

            case 9:
                if (mSceneArg == 0xA) {
                    if ((*(utility::FrameController**)((char*)this + 0x2B8))->isPlaying()) break;
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x3C);
                    fc->play();
                    fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0x8C);
                    fc->play();
                    mSceneType = 0xB;
                    {
                        int idx = mSomething_0x218;
                        mpLoadResult[idx] = (void*)loadLayoutFileAsync(page, index, &mpLoadBuf[idx], (nand::File**)&mpLoadFile[idx], &mpLoadUnk[idx]);
                    }
                    createChanLayout();
                    mSceneType = 0xA;
                    mSceneArg = 0;
                } else if (mSceneArg == 0xC) {
                    if ((*(utility::FrameController**)((char*)this + 0x290))->isPlaying()) break;
                    if (mpObject_0x32C == NULL && !((nand::Base*)mpAnimator_0x328)->isFinished()) break;
                    if (!(*(utility::FrameController**)((char*)this + 0x2BC))->isPlaying()) break;
                    (*(utility::FrameController**)((char*)this + 0x2BC))->play();
                    (*(utility::FrameController**)((char*)this + 0x2E4))->play();
                    createChanLayout();
                    mSceneType = 0xC;
                    mSceneArg = 0;
                } else if (mSceneArg == 0xF) {
                    if ((*(utility::FrameController**)((char*)this + 0x290))->isPlaying()) break;
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0xBC);
                    fc->play();
                    snd::sSystem.startSE("WIPL_ME_INVALID_DISC_BANNER");
                    mSceneType = 0xF;
                    mSceneArg = 0;
                }
                break;

            case 0xA:
                if ((*(utility::FrameController**)((char*)this + 0x294))->isPlaying()) break;
                if ((*(utility::FrameController**)((char*)this + 0x2E4))->isPlaying()) break;
                (*(utility::FrameController**)((char*)this + 0x288))->stop();
                mpLayout_0x254->GetRootPane()->SetVisible(false);
                mSceneType = 0xB;
                initChanAnmAndSound();
                changeStartButton();
                break;

            case 0xB:
                if (diskState != 5) {
                    if (*(u8*)((char*)System::getChannelManager() + 0x1B81) == 0) break;
                }
                {
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0xB8);
                    mSceneType = 0xE;
                    fc->play();
                }
                mpLayout_0x254->GetRootPane()->SetVisible(true);
                changeStartButton();
                break;

            case 0xC:
                if ((*(utility::FrameController**)((char*)this + 0x2BC))->isPlaying()) break;
                if ((*(utility::FrameController**)((char*)this + 0x2E4))->isPlaying()) break;
                (*(utility::FrameController**)((char*)this + 0x2B0))->stop();
                mpLayout_0x254->GetRootPane()->SetVisible(false);
                mSceneType = 0xD;
                initChanAnmAndSound();
                changeStartButton();
                break;

            case 0xD:
                if (diskState == 6) break;
                {
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0xB8);
                    mSceneType = 0xE;
                    fc->play();
                }
                mpLayout_0x254->GetRootPane()->SetVisible(true);
                changeStartButton();
                break;

            case 0xE:
                if ((*(utility::FrameController**)((char*)this + 0x310))->isPlaying()) break;
                snd::sSystem.stopBannerSound(0);
                snd::sSystem.stopSE((void*)_pad_0x24C, 0);
                if (mpLayout_0x23C != NULL && mpLayout_0x23C != mpObject_0x32C) {
                    mpLayout_0x23C->destroyHeap();
                    mpLayout_0x23C = NULL;
                }
                {
                    int idx = mSomething_0x218;
                    mpLoadResult[idx] = (void*)loadLayoutFileAsync(page, index, &mpLoadBuf[idx], (nand::File**)&mpLoadFile[idx], &mpLoadUnk[idx]);
                }
                createChanLayout();
                mSceneArg = 0xE;
                break;

            case 0xF:
                if ((*(utility::FrameController**)((char*)this + 0x314))->isPlaying()) break;
                (*(utility::FrameController**)((char*)this + 0x318))->play();
                mSceneType = 0x10;
                break;

            case 0x10:
                if (diskState == 7) break;
                (*(utility::FrameController**)((char*)this + 0x318))->stop();
                {
                    utility::FrameController* fc = *(utility::FrameController**)((char*)&*(utility::FrameController**)((char*)this + 0x258) + 0xC4);
                    fc->play();
                }
                {
                    nw4r::lyt::GroupContainer* container = *(nw4r::lyt::GroupContainer**)((char*)mpLayout_0x254 + 0x18);
                    nw4r::lyt::Group* group = container->FindGroupByName(lbl_8164E988[0]);
                    typedef nw4r::lyt::PaneLinkList PaneList;
                    for (PaneList::Iterator it = group->GetPaneList().GetBeginIter(); it != group->GetPaneList().GetEndIter(); ++it) {
                        it->mTarget->SetVisible(true);
                    }
                }
                mSceneType = 0x11;
                break;

            case 0x11:
                if ((*(utility::FrameController**)((char*)this + 0x31C))->isPlaying()) break;
                mSceneType = 2;
                break;
            }
        }
        BOOL ChannelTitle::isDiskChannelByDraw(int page, int index) {
            BOOL r = FALSE;
            if (System::getChannelManager()->getSceneID(page, index) == channel::SCENE_ID_DISK_CHANNEL) {
                if (mSceneType != 0xB && mSceneType != 0xD) r = TRUE;
            }
            return r;
        }
        BOOL ChannelTitle::checkNetSetting(int page, int index) {
            if (System::getChannelManager()->getSceneID(page, index) == 0xF) return TRUE;

            u8 flag = *(u8*)((u8*)System::getChannelManager() + page * 0x540 + index * 0x70 + 0x24);
            int hasNet;
            if (flag != 0) {
                int hdr = *(int*)((u8*)System::getChannelManager() + page * 0x540 + index * 0x70 + 0x10);
                if (*(int*)((char*)hdr + 0x58) & 0x100) {
                    hasNet = 1;
                } else {
                    hasNet = 0;
                }
            } else {
                hasNet = 0;
            }

            if (hasNet) {
                if (!ipl::ncd::NCDSetting::getConnectEnableFlag()) return FALSE;
            }
            return TRUE;
        }
        BOOL ChannelTitle::isSetParental() {
            SCParentalControlsInfo info;
            if (SCGetParentalControl(&info)) {
                if (info.enable & SC_PARENTAL_FLAG_ENABLED) return TRUE;
            }
            return FALSE;
        }
        void ChannelTitle::startParentalDialog(int idx) {
            if (((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->isArrowVisible(1)) {
                ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x1A);
                ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x19);
            }
            createChildScene(0x1B, this, NULL, (void*)idx);
            unk_0x70 = NULL;
            mState = 0xB;
        }
        int ChannelTitle::searchChannel(int direction, int* pageOut, int* indexOut) const {
            int curPage = mChannelPage;
            int step = 1;
            if (direction == 1) step = -1;
            int curIndex = mChannelIndex;
            int curFlat = curIndex + curPage * 12;
            int maxFlat = mChannelChanIndex * 12;
            int scanFlat = curFlat + step;
            channel::Manager* mgr = System::getChannelManager();
            BOOL result;

            while (true) {
                if (scanFlat < 0) scanFlat = maxFlat - 1;
                if (scanFlat >= maxFlat) scanFlat = 0;
                if (scanFlat == curFlat) {
                    *pageOut = curPage;
                    *indexOut = mChannelIndex;
                    result = FALSE;
                    break;
                }
                int page = scanFlat / 12;
                int index = scanFlat % 12;
                u8 flag = *(u8*)((u8*)mgr + page * 0x540 + index * 0x70 + 0x24);
                if (flag != 0) {
                    *pageOut = page;
                    *indexOut = index;
                    result = TRUE;
                    break;
                }
                scanFlat += step;
            }
            return result;
        }
        void ChannelTitle::startChangeChannel(int page, int index) {
            u32 temp = System::getChannelManager()->getSceneID(page, index) - 0xF;
            temp = __cntlzw(temp);
            temp >>= 5;
            if (temp != 0) {
                int state = System::getBS2Manager()->getIPLState();

                if (state == 5) {
                    mSceneType = 0xB;
                } else if (state == 6) {
                    mSceneType = 0xD;
                } else {
                    utility::FrameController* fcC0 = mpFrameCtrl_0xC0;
                    if (fcC0->getCurrentFrame() == fcC0->getMaxFrame()) {
                        utility::FrameController* fcBC = unk_0xBC;
                        if (fcBC->getCurrentFrame() != fcBC->getMaxFrame()) {
                            fcBC->initFrame();
                            fcBC->restart();
                        }
                    }
                    mSceneType = 0;
                }
            }

            utility::FrameController* fc1F0 = mpFrameCtrl_0x1F0;
            fc1F0->initFrame();
            fc1F0->restart();

            channel::Manager* mgr2 = System::getChannelManager();
            u32 r28 = mgr2->checkNeedUpdate(page, index);
            u32 r3_ = mgr2->checkNeedUpdate(mChannelPage, mChannelIndex);
            if (r3_ != r28) {
                utility::FrameController* fc1C0 = mpFrameCtrl_0x1C0;
                fc1C0->initFrame();
                fc1C0->restart();
            }

            if (mSomething_0x384 != 0) {
            unk_0x388 = 1;
            }

            if (System::getChannelManager()->getUnlockChannelState(mChannelPage, mChannelIndex) == 1) {
                updateLockedMsg(2);
            }

            mChannelPage = page;
            mChannelIndex = index;
            channel::Manager::setCurrentChannel(page, index);
            startGetTicketLimitTask();

            mpLoadResult[1 - mSomething_0x218] = (void*)loadLayoutFileAsync(
                mChannelPage, mChannelIndex,
                &mpLoadBuf[1 - mSomething_0x218],
                (nand::File**)&mpLoadFile[1 - mSomething_0x218],
                &mpLoadUnk[1 - mSomething_0x218]);

            mDispX = mpChannelSelect->getDispChanTrans(mChannelIndex).x;
            mDispY = mpChannelSelect->getDispChanTrans(mChannelIndex).y;

            mState = 8;
        }
        void ChannelTitle::reserveNextScene() {
            int sceneId = System::getChannelManager()->getSceneID(mChannelPage, mChannelIndex);

            unk_0x70 = NULL;
            unk_0x37C = 0;

            u32 temp = System::getChannelManager()->getSceneID(mChannelPage, mChannelIndex) - 0xF;
            temp = __cntlzw(temp);
            temp >>= 5;
            if (temp != 0) {
                if (mSceneType == 0xB) {
                    if (System::getBS2Manager()->checkParentalControl()) {
                        prepareForBoot1(0xF);
                        System::getBS2Manager()->reserveRVLGame();
                        mTmdReady = 0;
                        System::getTask1()->request(iplChannelTitle_813BA684, this, NULL);
                        mState = 0xE;
                    } else {
                        startParentalDialog(0);
                    }
                } else if (mSceneType == 0xD) {
                    prepareForBoot1(0xF);
                    System::getBS2Manager()->reserveGCGame();
                    mTmdReady = 0;
                    System::getTask1()->request(iplChannelTitle_813BA684, this, NULL);
                    mState = 0xE;
                }
                mSubState = 0xF;
                return;
            }

            if (sceneId == 0xE) {
                if (System::getChannelManager()->checkNeedUpdate(mChannelPage, mChannelIndex)) {
                    if (isSetParental()) {
                        startParentalDialog(0);
                    } else {
                        reserveSettingScene(true);
                    }
                    mSubState = 0x12;
                } else {
                    if (checkNetSetting(mChannelPage, mChannelIndex)) {
                        System::getHomeButtonMenu()->disable();
                        mTmdReady = 0;
                        mTmdGroupId = 0;
                        System::getTask1()->request(getTmdTask, this, NULL);
                        if (System::getChannelManager()->getUnlockChannelState(mChannelPage, mChannelIndex) == 0) {
                            mSubState = 0x11;
                        } else {
                            mSubState = 0x21;
                        }
                        mState = 0xE;
                    } else {
                        reserveSettingScene(false);
                        mSubState = 0x12;
                    }
                }
                return;
            }

            reserveAllSceneDestruction(sceneId, NULL);
            if (snd::sBannerSoundPlayer.getUnk_0x00()) {
                snd::sSystem.stopBannerSound(0x1C);
            }
            void* handle = (void*)_pad_0x24C;
            if (handle != NULL && *(int*)handle != 0) {
                snd::sSystem.stopSE(handle, 0x1C);
            }
            mSubState = sceneId;
            mState = 5;
        }
        void ChannelTitle::reserveSettingScene(bool unk) {
            if (System::getChannelManager()->checkNeedUpdate(mChannelPage, mChannelIndex)) {
                reserveAllSceneDestruction(0x12, (void*)3);
                mState = 0x7;
            } else if (unk) {
                reserveAllSceneDestruction(0x12, (void*)1);
                mState = 0x7;
            } else {
                if (((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->isArrowVisible(1)) {
                    ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x1A);
                    ((Button*)System::getSceneManager()->getScene(SCENE_BUTTON))->animation(0x19);
                }
                System::getDialog()->callBtn2(0x143, 0x146, 0x25);
                mState = 0xC;
            }
        }
        void ChannelTitle::prepareForBoot1(int idx) {
            for (int i = 0; i < 4; i++) {
                controller::Interface* ctrl = System::getController(i);
                if (ctrl != NULL) {
                    ctrl->cancelRumbling();
                }
            }

            if (idx == 0xF || idx == 0x11 || idx == 0x21) {
                System::getHomeButtonMenu()->disable();
            }

            if (idx == 0x11 || idx == 0x21) {
                System::getSaveData()->setPrevPage(mChannelPage);
                savedata::Manager* saveMgr = System::getSaveData();

                ESTitleId titleId = System::getChannelManager()->getTitleID(mChannelPage, mChannelIndex);
                iplSavedata_813596B8(saveMgr, titleId);

                mpFlushFile = System::getSaveData()->flushAsync(System::getMem2App());
            }
        }
        void ChannelTitle::prepareForBoot2(int idx) {
            if (idx == 0x11) {
                System::getBS2Manager()->abort();
            }
            __WPADReconnect(1);
            System::stopReceiveSchedule();
            ESTitleId titleId = SCGetTmpTitleID();
            u32 hi = (u32)(titleId >> 32);
            u32 lo = (u32)titleId;
            unk_0x36C = (void*)lo;
            unk_0x368 = (void*)hi;
            if (titleId != 0) {
                SCSetTmpTitleID(0ULL);
                System::getTask1()->request(iplChannelTitle_813BA784, this, NULL);
            } else {
                unk_0x370 = 1;
            }
        }
        void ChannelTitle::rebootSystem() {
            snd::sSystem.stopAllSound(0);
            ((snd::System*)&snd::sSystem)->calc();
            VISetBlack(1);
            VIFlush();
            VIWaitForRetrace();
            while (__OSSyncSram() == 0) {}
            OSRebootSystem();
        }
        BOOL ChannelTitle::isEnableToExecute(int page, int index) {
            if (isDiskChannelByDraw(page, index)) return FALSE;
            BOOL isDisk = System::getChannelManager()->getSceneID(page, index) == channel::SCENE_ID_DISK_CHANNEL;
            if (isDisk) {
                bs2::Manager* mgr = System::getBS2Manager();
                BOOL ok = TRUE;
                int state = mgr->getUnlockedState();
                if (state != 0x45 && state != 0x48) ok = FALSE;
                if (ok) {
                    return FALSE;
                }
            }
            if (isLimitOutChannel()) return FALSE;
            return (u32)(System::getChannelManager()->getUnlockChannelState(page, index) - 1) > 0;
        }
        void ChannelTitle::changeStartButton() {
            int enable = isEnableToExecute(mChannelPage, mChannelIndex);
            utility::FrameController* fc;
            nw4r::lyt::Pane* pane;
            if (enable == 0) {
                if (mStartBtnFlag != 0) {
                    mpFrameCtrl_0x16C->stop();
                    fc = mpFrameCtrl_0x168;
                    fc->initFrame();
                    fc->restart();
                    mStartBtnFlag = 0;
                }
                unk_0x350 = 0;
            } else {
                if (mStartBtnFlag == 0) {
                    mpFrameCtrl_0x168->stop();
                    fc = mpFrameCtrl_0x16C;
                    fc->initFrame();
                    fc->restart();
                    pane = mpLayout_0x208->GetRootPane()->FindPaneByName(lbl_81696A20[1]);
                    mpPaneMgr_0x20C->initPane(pane);
                    unk_0x350 = 0;
                }
                mStartBtnFlag = 1;
            }
        }

        void ChannelTitle::drawPolygonAroundRect(const nw4r::ut::Rect& rect, _GXColor color) {
            nw4r::ut::Rect projRect;
            System::getProjectionRect(&projRect);

            nw4r::ut::Rect rects[4];
            rects[0].left = projRect.left;
            rects[0].top = projRect.bottom;
            rects[0].right = projRect.right;
            rects[0].bottom = rect.top;

            rects[1].left = rect.right;
            rects[1].top = rect.top;
            rects[1].right = projRect.right;
            rects[1].bottom = projRect.top;

            rects[2].left = projRect.left;
            rects[2].top = rect.bottom;
            rects[2].right = rect.right;
            rects[2].bottom = projRect.top;

            rects[3].left = projRect.left;
            rects[3].top = rect.top;
            rects[3].right = rect.left;
            rects[3].bottom = rect.bottom;

            for (int i = 0; i < 4; i++) {
                ipl::utility::Graphics::drawPolygon(rects[i], color);
            }
        }
        void ChannelTitle::setMessage(nw4r::lyt::Pane* pane, u32 msgId, bool unk) {
            wchar_t buf[0x100];
            const wchar_t* msg = System::getMessageManager()->getMessage(msgId);
            wcsncpy(buf, msg, 0x100);
            buf[0xFF] = L'\0';

            nw4r::lyt::TextBox* textBox = (nw4r::lyt::TextBox*)pane;
            if (unk) {
                u16 len = (u16)wcslen(buf);
                textBox->AllocStringBuffer(len);
            }
            textBox->SetString(buf, 0);
        }

        int ChannelTitle::loadLayoutFileAsync(int page, int index, void** bufOut, nand::File** fileOut, void** unkOut) {
            BOOL isDisk = System::getChannelManager()->getSceneID(page, index) == channel::SCENE_ID_DISK_CHANNEL;
            if (isDisk) {
                if (mSceneType == 0xB) {
                    *bufOut = System::getChannelManager()->getDiskBanner(true);
                    *fileOut = NULL;
                    *unkOut = System::getChannelManager()->getDiskSound(true);
                    return 0;
                }
                if (mSceneType == 0xD) {
                    if (mpAnimator_0x328 == NULL) {
                        mpAnimator_0x328 = (layout::Animator*)System::getNandManager()->readLayoutAsync(
                            // TODO: Is the \0 just padding and can be removed?
                            System::getMem2App(), "GCBann.ash\0", false);
                    }
                    *bufOut = NULL;
                    *fileOut = NULL;
                    *unkOut = NULL;
                    return (int)(void*)mpAnimator_0x328;
                }
                *bufOut = NULL;
                *fileOut = NULL;
                *unkOut = NULL;
                mSceneType = 0;
                return mSomething_0x250;
            }

            *bufOut = NULL;
            *fileOut = (nand::File*)System::getChannelManager()->loadSoundAsync(System::getMem2App(), page, index);
            *unkOut = NULL;

            nand::MetaFile* banner = System::getChannelManager()->loadBannerAsync(System::getMem2App(), page, index);
            System::getChannelManager()->loadLockedMsgAsync(page, index);

            nand::LayoutFile* file = new (System::getMem2App(), 4) nand::LayoutFile((nand::File*)banner);
            return (int)(void*)file;
        }
        BOOL ChannelTitle::isTimeLimitedChannel(u32* out1, u32* out2) {
            if (unk_0x374 == 1) {
                *out1 = mRemainingTime / 3600;
                *out2 = (mRemainingTime - *out1 * 3600) / 60;
                return TRUE;
            }
            return FALSE;
        }
        BOOL ChannelTitle::isLimitOutChannel() {
            u32 hours, minutes;
            if (isTimeLimitedChannel(&hours, &minutes)) {
                if (mRemainingTime == 0) return TRUE;
            }
            return FALSE;
        }
        void ChannelTitle::dispTimeLimitWindow() {
            u32 val1, val2;
            if (isTimeLimitedChannel(&val1, &val2)) {
                if (isLimitOutChannel()) {
                    mpLayout_0x210->GetRootPane()->SetVisible(false);
                    mpObject_0x214->GetRootPane()->SetVisible(true);
                } else {
                    mpLayout_0x210->GetRootPane()->SetVisible(true);
                    mpObject_0x214->GetRootPane()->SetVisible(false);

                    wchar_t buf[66];
                    getTimeLimitString(buf, val1, val2);

                    nw4r::lyt::TextBox* textBox = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(
                        mpLayout_0x210->GetRootPane()->FindPaneByName("T_Timer", true));
                    textBox->SetString(buf, 0);
                }
            } else {
                mpLayout_0x210->GetRootPane()->SetVisible(false);
                mpObject_0x214->GetRootPane()->SetVisible(false);
            }
        }
        void ChannelTitle::getTimeLimitString(wchar_t* dest, u32 minutes, u32 seconds) {
            if (minutes >= 100) {
                minutes = 99;
                seconds = 59;
            }

            memset(dest, 0, 0x82);

            const wchar_t* baseMsg = System::getMessageManager()->getMessage(0x16);
            wcscpy(dest, baseMsg);

            wchar_t minBuf[16];
            wchar_t* p = minBuf - 1;
            for (int i = 0; i < 8; i++) {
                *++p = L'\0';
                *++p = L'\0';
            }

            wchar_t secBuf[16];
            p = secBuf - 1;
            for (int i = 0; i < 8; i++) {
                *++p = L'\0';
                *++p = L'\0';
            }

            if (minutes == 0 && seconds == 0) {
                const wchar_t* zeroMsg = System::getMessageManager()->getMessage(0x17);
                swprintf(dest, 0x41, L"%ls %ls", dest, zeroMsg);
                return;
            }

            if ((u32)System::getLanguage() == 1) {
                const wchar_t* msg1F = System::getMessageManager()->getMessage(0x1F);
                swprintf(dest, 0x41, L"%ls %d%ls%02d", dest, minutes, msg1F, seconds);
                return;
            }

            if (System::getLanguage() == 0 || (u32)System::getLanguage() == 9 || (u32)System::getLanguage() == 2) {
                if (minutes == 1) {
                    const wchar_t* msg18 = System::getMessageManager()->getMessage(0x18);
                    swprintf(minBuf, 0x10, L"1%ls", msg18);
                } else if (minutes > 0) {
                    const wchar_t* msg31 = System::getMessageManager()->getMessage(0x1F);
                    swprintf(minBuf, 0x10, L"%d%ls", minutes, msg31);
                }

                if (seconds == 1) {
                    const wchar_t* msg19 = System::getMessageManager()->getMessage(0x19);
                    swprintf(secBuf, 0x10, L"1%ls", msg19);
                } else if (seconds > 0) {
                    const wchar_t* msg20 = System::getMessageManager()->getMessage(0x20);
                    swprintf(secBuf, 0x10, L"%d%ls", seconds, msg20);
                }
            } else if ((u32)System::getLanguage() == 3 || (u32)System::getLanguage() == 5) {
                if (minutes == 1) {
                    const wchar_t* msg18 = System::getMessageManager()->getMessage(0x18);
                    swprintf(minBuf, 0x10, L"01%ls", msg18);
                } else if (minutes > 0) {
                    const wchar_t* msg31 = System::getMessageManager()->getMessage(0x1F);
                    swprintf(minBuf, 0x10, L"%02d%ls", minutes, msg31);
                }

                if (seconds == 1) {
                    const wchar_t* msg19 = System::getMessageManager()->getMessage(0x19);
                    swprintf(secBuf, 0x10, L"01%ls", msg19);
                } else if (seconds > 0) {
                    const wchar_t* msg20 = System::getMessageManager()->getMessage(0x20);
                    swprintf(secBuf, 0x10, L"%02d%ls", seconds, msg20);
                }
            } else {
                if (minutes == 1) {
                    const wchar_t* msg18 = System::getMessageManager()->getMessage(0x18);
                    swprintf(minBuf, 0x10, L"1%ls", msg18);
                } else if (minutes > 0) {
                    const wchar_t* msg31 = System::getMessageManager()->getMessage(0x1F);
                    swprintf(minBuf, 0x10, L"%d%ls", minutes, msg31);
                }

                if (seconds == 1) {
                    const wchar_t* msg19 = System::getMessageManager()->getMessage(0x19);
                    swprintf(secBuf, 0x10, L"01%ls", msg19);
                } else if (seconds > 0) {
                    const wchar_t* msg20 = System::getMessageManager()->getMessage(0x20);
                    swprintf(secBuf, 0x10, L"%02d%ls", seconds, msg20);
                }
            }

            swprintf(dest, 0x41, L"%ls %ls%ls", dest, minBuf, secBuf);
        }
        void ChannelTitle::getTmdTask(void* unk) {
            ChannelTitle* self = (ChannelTitle*)unk;
            if (System::getChannelManager()->getUnlockChannelState(self->mChannelPage, self->mChannelIndex) != 0) {
                if (System::getChannelManager()->getUnlockTicket(self->mChannelPage, self->mChannelIndex, &msUnlockTicket) < 0) {
                    System::getErrorHandler()->log("BS2", 0, "iplChannelTitle.cpp", 0x1282);
                    System::getErrorHandler()->set(ErrorHandler::DEFAULT, 2, NULL, 0, -1);
                }
            }
            ESTitleId titleId = System::getChannelManager()->getTitleID(self->mChannelPage, self->mChannelIndex);
            int tmdResult = utility::ESMisc::GetTmdView(self->mpRsoExHeap, titleId, &self->mpTmdView_0x35C);
            if (tmdResult != 0) {
                System::getErrorHandler()->log("ES", tmdResult, "iplChannelTitle.cpp", 0x128C);
                System::getErrorHandler()->set(ErrorHandler::DEFAULT, 2, NULL, 0, -1);
            }
            self->mTmdReady = 1;
        }
        void ChannelTitle::getTicketLimitTask(void* unk) {
            ChannelTitle* self = (ChannelTitle*)unk;
            ESTitleId titleId = System::getChannelManager()->getTitleID(self->mChannelPage, self->mChannelIndex);
            channel::Manager* mgr = System::getChannelManager();
            int ticketIdx = *(int*)((char*)mgr + self->mChannelPage * 0x540 + self->mChannelIndex * 0x70 + 0x20);
            int ret = utility::ESMisc::GetTicketView(
                System::getMem2App(), titleId, &msTicketView, ticketIdx);
            if (ret != 0) {
                System::getErrorHandler()->log("ES", ret, "iplChannelTitle.cpp", 0x12B5);
                System::getErrorHandler()->set(ErrorHandler::DEFAULT, 2, NULL, 0, -1);
            }
            if (self->isTimeLimitedTicket(&msTicketView, &self->mRemainingTime)) {
                self->unk_0x374 = 1;
            } else {
                self->unk_0x374 = 0;
            }
            self->unk_0x371 = 1;
        }
        BOOL ChannelTitle::isTimeLimitedTicket(ESTicketView* ticket, u32* out) {
            OSGetTick();
            int espResult = ESP_InitLib();
            if (espResult != 0) {
                System::getErrorHandler()->log("ES", espResult, "iplChannelTitle.cpp", 0x12F3);
                System::getErrorHandler()->set(ErrorHandler::DEFAULT, 2, NULL, 0, -1);
            }
            OSPlayTimeType ptt;
            int ptResult = __OSGetPlayTime((ESTicketView*)this, &ptt, (u32*)ticket);
            if (ptResult != 0) {
                System::getErrorHandler()->log("ES", ptResult, "iplChannelTitle.cpp", 0x12FC);
                System::getErrorHandler()->set(ErrorHandler::DEFAULT, 2, NULL, 0, -1);
            }
            if (ptt == 1) return TRUE;
            return FALSE;
        }
        void ChannelTitle::startGetTicketLimitTask() {
            unk_0x374 = 0;
            BOOL isDisk = System::getChannelManager()->getSceneID(mChannelPage, mChannelIndex) == 0xF;
            if (isDisk) {
                unk_0x371 = 1;
            } else {
                unk_0x371 = 0;
                System::getNandManager()->getTask()->request(getTicketLimitTask, this, NULL);
            }
        }
        void ChannelTitle::updateLockedMsg(int idx) {
            if (idx == 0) {
                if (System::getChannelManager()->getUnlockChannelState(mChannelPage, mChannelIndex) == 1) {
                    idx = 1;
                } else if (System::getChannelManager()->getUnlockChannelState(mChannelPage, mChannelIndex) == 2) {
                    idx = 2;
                }
            }

            if (idx == 1) {
                if (unk_0x340 == 0) {
                    if (!isLimitOutChannel()) {
                        if (System::getChannelManager()->getLockedMsg() != NULL) {
                            nw4r::lyt::TextBox* textBox = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(
                                mpLayout_0x334->GetRootPane()->FindPaneByName("T_ChTopMes", true));
                            u16* msg = System::getChannelManager()->getLockedMsg();
                            textBox->SetString((const wchar_t*)msg, 0);
                            mpFrameCtrl_0x33C->stop();
                            mpFrameCtrl_0x338->initFrame();
                            mpFrameCtrl_0x338->restart();
                            unk_0x340 = 1;
                        }
                    }
                }
            } else if (idx == 2) {
                if (unk_0x340 != 0) {
                    mpFrameCtrl_0x338->stop();
                    mpFrameCtrl_0x33C->initFrame();
                    mpFrameCtrl_0x33C->restart();
                    unk_0x340 = 0;
                }
            }
        }
        BOOL ChannelTitle::CheckParentalControl(ESTmdView* tmd) {
            if (!utility::ESMisc::__IsPCEnable()) return TRUE;
            SCParentalControlsInfo info;
            if (!SCGetParentalControl(&info)) return TRUE;
            if (info.enable & SC_PARENTAL_FLAG_ENABLED) {
                BOOL restricted = System::getChannelManager()->isParentalRestricted(mChannelPage, mChannelIndex);
                u32 netRestrict = SCGetNetContentRestrictions();
                if (netRestrict & restricted) return FALSE;
                if (SCGetWwwRestriction()) {
                    if (System::getChannelManager()->isInternetChannel(mChannelPage, mChannelIndex)) return FALSE;
                }
                if (System::getChannelManager()->isInternetChannel(mChannelPage, mChannelIndex)) return TRUE;
                return utility::ESMisc::CheckTmdParentalControl(tmd);
            }
            return TRUE;
        }

        void ChannelTitleEventHandler::onEvent(u32 compId, u32 event, void* data) {
            const char* paneName = (const char*)((char*)((gui::PaneComponent*)mpManager->getComponent(compId))->getPane() + 0xB4);

            switch (event) {
            case 0:
                if (mpInstance->mState == 1 && ((const controller::Interface*)data)->downTrg(0x100800)) {
                    if (strcmp(paneName, lbl_81696A20[0]) == 0) {
                        mpInstance->mpFrameCtrl_0xEC->initFrame();
                        mpInstance->mpFrameCtrl_0xEC->restart();
                        snd::sSystem.startSE("WIPL_SE_BT_PUSH");
                        mpInstance->mState = 0xA;
                        mpInstance->tryToGoBackward();
                    } else if (strcmp(paneName, lbl_81696A20[1]) == 0) {
                        if (mpInstance->mStartBtnFlag == 2) {
                            mpInstance->reserveNextScene();
                            mpInstance->mpFrameCtrl_0x114->initFrame();
                            mpInstance->mpFrameCtrl_0x114->restart();
                            snd::sSystem.startSE("WIPL_SE_DECIDE");
                        } else {
                            snd::sSystem.startSE("WIPL_SE_GRAY_BUTTON");
                        }
                    }
                }
                break;
            case 1:
                for (int i = 0, off = 0; i < 2; i++, off += 4) {
                    if (strcmp(paneName, lbl_81696A20[i]) != 0) continue;
                    if (i != 0 && mpInstance->mStartBtnFlag <= 0) continue;

                    (*(int*)((char*)mpInstance + off + 0x34C))++;
                    if (*(int*)((char*)mpInstance + off + 0x34C) > 1) continue;

                    utility::FrameController* fc = (&mpInstance->mpFc_0x98)[i];
                    fc->initFrame();
                    fc->restart();
                    snd::sSystem.startSE("WIPL_SE_BT_TARGETTING");
                    ((controller::Interface*)data)->rumble(0);
                }
                break;
            case 2:
                for (int i = 0, off = 0; i < 2; i++, off += 4) {
                    if (strcmp(paneName, lbl_81696A20[i]) != 0) continue;
                    if (i != 0 && mpInstance->mStartBtnFlag <= 0) continue;

                    (*(int*)((char*)mpInstance + off + 0x34C))--;
                    if (*(int*)((char*)mpInstance + off + 0x34C) >= 0) continue;

                    utility::FrameController* fc = (&mpInstance->mpFc_0x94)[i];
                    fc->initFrame();
                    fc->restart();
                }
                break;
            }
        }
        void CsChanTtlButtonEventHandler::onEventDerived(u32 compId, u32 event, const controller::Interface* con) {
            gui::PaneComponent* comp = (gui::PaneComponent*)mpManager->getComponent(compId);
            nw4r::lyt::Pane* pane = comp->getPane();
            const char* paneName = (const char*)((char*)pane + 0xB4);

            if (event == 0) {
                ChannelTitle* chTtl = mpInstance;
                if (chTtl->mState == 1 && con->downTrg(0x100800)) {
                    scene::Button* btn = (scene::Button*)System::getSceneManager()->getScene(5);
                    if (strcmp(paneName, Button::mscButtonName[10]) == 0) {
                        int page, index;
                        chTtl->searchChannel(1, &page, &index);
                        btn->animation(0x14);
                        chTtl->startChangeChannel(page, index);
                        snd::sSystem.startSE("WSD_SELECT");
                    } else if (strcmp(paneName, Button::mscButtonName[9]) == 0) {
                        int page, index;
                        chTtl->searchChannel(0, &page, &index);
                        btn->animation(0x13);
                        chTtl->startChangeChannel(page, index);
                        snd::sSystem.startSE("WSD_SELECT");
                    }
                }
            }
        }
    }  // namespace scene
}  // namespace ipl

extern "C" void iplChannelTitle_813BA684(void* arg) {
    *(u8*)((char*)arg + 0x360) = 1;
}

extern "C" void iplChannelTitle_813BA784(void* arg) {
    char* self = (char*)arg;

    if ((*(u32*)(self + 0x368) | *(u32*)(self + 0x36C)) != 0) {
        u32 zero;
        zero = 0;
        u32 ten = 10;
        u32 zeroMul = zero * ten;
        vu32* timer = (vu32*)0x80000000;
        u32 divisor = 1000;
        while (!ipl::System::isReceiveScheduleStopped()) {
            u32 val = *(vu32*)((char*)timer + 0xF8);
            u32 shift = val >> 2;
            u32 tmp = shift / divisor;
            u64 sleepTicks = ((u64)tmp * ten) | ((u64)zeroMul << 32);
            OSSleepTicks(sleepTicks);
        }

        u32 hi = *(u32*)(self + 0x368);
        u32 lo = *(u32*)(self + 0x36C);
        ESTitleId titleId = ((u64)hi << 32) | lo;
        int result = ipl::utility::DeleteTitleContent(ipl::System::getMem2App(), titleId);
        if (result != 0) {
            ipl::System::getErrorHandler()->log("ES", result, "iplChannelTitle.cpp", 0x12D8);
            ipl::System::getErrorHandler()->set(ipl::ErrorHandler::DEFAULT, 2, NULL, 0, -1);
        }
    }
    *(u8*)(self + 0x370) = 1;
}
