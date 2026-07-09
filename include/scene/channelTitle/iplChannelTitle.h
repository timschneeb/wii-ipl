#ifndef IPL_SCENE_CHANNEL_TITLE_H
#define IPL_SCENE_CHANNEL_TITLE_H

#include "iplSceneHeader.h"
#include "scene/button/iplButton.h"
#include "layout/GUIManager.h"
#include "utility/iplFrameController.h"
#include "utility/iplCapture.h"
#include "scene/channelSelect/iplChannelSelect.h"
#include "system/iplChannelScriptManager.h"
#include <decomp.h>

struct RSOObjectHeader;

namespace ipl {
    namespace channel { class RsoThread; }
    namespace scene {
        class ChannelSelect;

        class ChannelTitleEventHandler;
        class CsChanTtlButtonEventHandler;

        FADER_SCENE_CLASS(ChannelTitle) {
        public:
            ChannelTitle(EGG::Heap* heap, ChannelSelect* chanSel);
            virtual ~ChannelTitle();

            BOOL isFirstCall() const;
            BOOL isResetAcceptable() const;
            BOOL isInLaunching() const;
            BOOL isEnableAppStart();
            void getRsoExBufData(void* rsoExBuf) const;
            void setRsoExBufData(const void* rsoExBuf);
            void* allocFromRsoExHeap(unsigned long size, int align);
            void freeToRsoExHeap(void* buffer);
            unsigned long getAllocatableSizeForRsoExHeap() const;
            layout::Animator* getRsoAnimator(int idx) const;

            FaderSceneCommand calcFadein();
            FaderSceneCommand calcNormal();
            FaderSceneCommand calcFadeout();

            int searchChannel(int direction, int* pageOut, int* indexOut) const;
            void startChangeChannel(int page, int index);
            void reserveNextScene();
            void prepareForBoot1(int);
            void prepareForBoot2(int);
            void create();
            void destroy();
            void draw();
            void calcCommon();
            void updateDiskState(int, int);
            void setDebugRsoInterval(u32);

            static ESTicketView msTicketView;
            static ESTicketView msUnlockTicket;

        private:
            void initCalcFadeout();
            void getRsoTitleDataPath(char* path) const;
            BOOL isStartAnimFinished() const;
            void startResetScene();
            void startResetting();
            void createChanLayout();
            void createDiskChanLayout();
            void createModuleChanLayout();
            void calcModuleChannel();
            void calcNormalNormal();
            void calcNormalChangeWait();
            void calcNormalChangeNext();
            void calcNormalParentalDialog();
            void calcNormalSettingDialog();
            void calcNormalControllerDialog();
            void calcNormalWaitTmd();
            void calcNormalWaitLockedTitle();
            void calcNormalLockedDiskDialog();
            void calcNormalUpdateDialog();
            void calcNormalUpdateAcceptDialog();
            void calcNormalUpdating();
            void calcNormalUpdateSucceeded();
            void calcNormalUpdateUnk1();
            void calcNormalUpdateUnk0();
            void calcNormalUpdateWaiting();
            void tryToStartScene();
            void tryToGoBackward();
            void loadModule(int page, int index);
            void calcChannelCS();
            void calcChannelRso();
            void prepare();
            void calcModuleInit();
            void calcCSWait();
            void calcCSFinish();
            void initChanAnmAndSound();
            void startChanSound();
            void bindChanAnms();
            void bindChanBaseAnms();
            void bindDiskAnms();
            BOOL isDiskChannelByDraw(int page, int index);
            BOOL checkNetSetting(int page, int index);
            BOOL isSetParental();
            void startParentalDialog(int idx);
            void reserveSettingScene(bool unk);
            void rebootSystem();
            BOOL isEnableToExecute(int page, int index);
            void changeStartButton();
            void drawPolygonAroundRect(const nw4r::ut::Rect& rect, _GXColor color);
            void setMessage(nw4r::lyt::Pane* pane, u32 msgId, bool unk);
            int loadLayoutFileAsync(int page, int index, void** bufOut, nand::File** fileOut, void** unkOut);
            BOOL isTimeLimitedChannel(u32* out1, u32* out2);
            BOOL isLimitOutChannel();
            void dispTimeLimitWindow();
            void getTimeLimitString(wchar_t* dest, u32 minutes, u32 seconds);
            static void getTmdTask(void* unk);
            static void getTicketLimitTask(void* unk);
            BOOL isTimeLimitedTicket(ESTicketView* ticket, u32* out);
            void startGetTicketLimitTask();
            void updateLockedMsg(int idx);
            BOOL CheckParentalControl(ESTmdView* tmd);


            s32 mState;                                  // 0x58
            EGG::ExpHeap* mpRsoExHeap;                   // 0x5C
            int mChannelPage;                            // 0x60
            int mChannelIndex;                           // 0x64
            int mChannelChanIndex;                       // 0x68
            int mSubState;                               // 0x6C
            void* unk_0x70;                              // 0x70
            int mSceneType;                              // 0x74
            int mSceneArg;                               // 0x78
            f32 mDispX;                                  // 0x7C
            f32 mDispY;                                  // 0x80
            f32 mDispZ;                                  // 0x84
            int unk_0x88;                                // 0x88
            int mStartBtnFlag;                           // 0x8C
            u8 unk_0x90;                                 // 0x90
            utility::FrameController* mpFc_0x94;         // 0x94
            utility::FrameController* mpFc_0x98;         // 0x98
            utility::FrameController* mpFc_0x9C;         // 0x9C
            u8 _pad_0xA0[0x1C];                          // 0xA0-0xBB
            utility::FrameController* unk_0xBC;          // 0xBC
            utility::FrameController* mpFrameCtrl_0xC0;  // 0xC0
            void* unk_0xC4;                              // 0xC4
            void* unk_0xC8;                              // 0xC8
            u8 _pad_0xCC[0x20];                          // 0xCC-0xEB
            utility::FrameController* mpFrameCtrl_0xEC;  // 0xEC
            u8 _pad_0xF0[0x24];                          // 0xF0-0x113
            utility::FrameController* mpFrameCtrl_0x114; // 0x114
            u8 _pad_0x118[0x50];                         // 0x118-0x167
            utility::FrameController* mpFrameCtrl_0x168; // 0x168
            utility::FrameController* mpFrameCtrl_0x16C; // 0x16C
            u8 _pad_0x170[0x50];                         // 0x170-0x1BF
            utility::FrameController* mpFrameCtrl_0x1C0; // 0x1C0
            utility::FrameController* mpFrameCtrl_0x1C4; // 0x1C4
            u8 _pad_0x1C8[0x28];                         // 0x1C8-0x1EF
            utility::FrameController* mpFrameCtrl_0x1F0; // 0x1F0
            utility::FrameController* mpFrameCtrl_0x1F4; // 0x1F4
            utility::FrameController* mpFrameCtrl_0x1F8; // 0x1F8
            math::HermiteIntp<f32>* mpTitleHermite;       // 0x1FC
            ChannelSelect* mpChannelSelect;              // 0x200
            nand::LayoutFile* mpLayoutFile;              // 0x204
            layout::Object* mpLayout_0x208;              // 0x208
            gui::PaneManager* mpPaneMgr_0x20C;           // 0x20C
            layout::Object* mpLayout_0x210;              // 0x210
            layout::Object* mpObject_0x214;              // 0x214
            int mSomething_0x218;                        // 0x218
            void* mpLoadResult[2];                       // 0x21C
            void* mpLoadBuf[2];                          // 0x224
            void* mpLoadFile[2];                         // 0x22C
            void* mpLoadUnk[2];                          // 0x234
            layout::Object* mpLayout_0x23C;              // 0x23C
            utility::FrameController* mpAnimContainer_0x240; // 0x240
            layout::Animator* mpAnim_0x244;              // 0x244
            layout::Animator* mpAnim_0x248;              // 0x248
            int _pad_0x24C;                              // 0x24C
            int mSomething_0x250;                        // 0x250
            layout::Object* mpLayout_0x254;              // 0x254
            u8 _pad_0x258[0xCC];                         // 0x258-0x323
            u8 unk_0x324;                                // 0x324
            layout::Animator* mpAnimator_0x328;          // 0x328
            layout::Object* mpObject_0x32C;              // 0x32C
            utility::FrameController* mpFrameCtrl_0x330; // 0x330
            layout::Object* mpLayout_0x334;              // 0x334
            utility::FrameController* mpFrameCtrl_0x338; // 0x338
            utility::FrameController* mpFrameCtrl_0x33C; // 0x33C
            u8 unk_0x340;                                // 0x340
            nand::File* mpFlushFile;                     // 0x344
            u32 unk_0x348;                               // 0x348
            u32 unk_0x34C;                               // 0x34C
            int unk_0x350;                               // 0x350
            ::gui::EventHandler* mpButtonEventHandler;   // 0x354
            utility::Capture* mpCapture_0x358;           // 0x358
            ESTmdView* mpTmdView_0x35C;                  // 0x35C
            u8 mTmdReady;                                // 0x360
            u8 unk_0x361;                                // 0x361
            u16 mTmdGroupId;                             // 0x362
            void* unk_0x364;                             // 0x364
            void* unk_0x368;                             // 0x368
            void* unk_0x36C;                             // 0x36C
            u8 unk_0x370;                                // 0x370
            u8 unk_0x371;                                // 0x371
            u8 _pad_0x372[2];                            // 0x372-0x373
            u32 unk_0x374;                               // 0x374
            u32 mRemainingTime;                          // 0x378
            u8 unk_0x37C;                                // 0x37C
            int mAnmState;                               // 0x380
            int mSomething_0x384;                        // 0x384
            u8 unk_0x388;                                // 0x388
            u8 unk_0x389;                                // 0x389
            u8 _pad_0x38A[2];                            // 0x38A-0x38B
            int mCounter_0x38C;                          // 0x38C
            int mCounterLimit_0x390;                     // 0x390
            nand::File* mpRsoLoadFile;                   // 0x394
            EGG::ExpHeap* mpRsoHeapArr[2];               // 0x398
            int mHeapIndex;                              // 0x3A0
            channel::RsoThread* mpRsoThread;             // 0x3A4
            int mTitleType;                              // 0x3A8
            char mTitleDataPath[0x40];                   // 0x3AC
            RSOObjectHeader* mpRsoHeader;                // 0x3EC
            void* unk_0x3F0;                             // 0x3F0
            void* unk_0x3F4;                             // 0x3F4
            u8 unk_0x3F8;                                // 0x3F8
            u8 mbIsFirstCall;                            // 0x3F9
            u8 _pad_0x3FA[2];                            // 0x3FA-0x3FB
            int mRsoPage;                                // 0x3FC
            int mRsoIndex;                               // 0x400
            EGG::ExpHeap* mDebugRsoInterval;             // 0x404

            friend class ChannelTitleEventHandler;
            friend class CsChanTtlButtonEventHandler;
        };

        class ChannelTitleEventHandler : public ::gui::EventHandler {
        public:
            ChannelTitleEventHandler(ChannelTitle* instance) : ::gui::EventHandler() { mpInstance = instance; }
            void onEvent(u32 compId, u32 event, void* data);
        private:
            ChannelTitle* mpInstance;
        };

        class CsChanTtlButtonEventHandler : public ButtonEventHandlerBase {
        public:
            CsChanTtlButtonEventHandler(ChannelTitle* instance) : ButtonEventHandlerBase() { mpInstance = instance; }
            void onEventDerived(u32 compId, u32 event, const controller::Interface* con);
        private:
            ChannelTitle* mpInstance;
        };
    }  // namespace scene
}  // namespace ipl

#endif  // IPL_SCENE_CHANNEL_TITLE_H
