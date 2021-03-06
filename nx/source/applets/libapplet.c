#include <string.h>
#include "types.h"
#include "result.h"
#include "arm/counter.h"
#include "services/applet.h"
#include "applets/libapplet.h"

void libappletArgsCreate(LibAppletArgs* a, u32 version) {
    memset(a, 0, sizeof(LibAppletArgs));

    a->CommonArgs_version = 1;
    a->CommonArgs_size = sizeof(LibAppletArgs);

    a->LaVersion = version;
    a->ExpectedThemeColor = appletGetThemeColorType();
}

void libappletArgsSetPlayStartupSound(LibAppletArgs* a, bool flag) {
    a->PlayStartupSound = flag!=0;
}

static Result _libappletCreateWriteStorage(AppletStorage* s, const void* buffer, size_t size) {
    Result rc=0;

    rc = appletCreateStorage(s, size);
    if (R_FAILED(rc)) return rc;

    rc = appletStorageWrite(s, 0, buffer, size);
    if (R_FAILED(rc)) {
        appletStorageClose(s);
        return rc;
    }

    return rc;
}

static Result _libappletPushInData(AppletHolder *h, const void* buffer, size_t size) {
    Result rc=0;
    AppletStorage storage;

    rc = _libappletCreateWriteStorage(&storage, buffer, size);
    if (R_FAILED(rc)) return rc;

    return appletHolderPushInData(h, &storage);
}

Result libappletArgsPush(LibAppletArgs* a, AppletHolder *h) {
    //Official sw stores the header in LibAppletArgs seperately (first 8-bytes), but here we're including it with the LibAppletCommonArguments struct.
    //Official sw uses appletStorageWrite twice, for writing the header then the rest of the struct.

    a->tick = armGetSystemTick();

    return _libappletPushInData(h, a, sizeof(LibAppletArgs));
}

static Result _libappletQlaunchRequest(u8* buf, size_t size) {
    Result rc=0;
    AppletStorage storage;

    rc = _libappletCreateWriteStorage(&storage, buf, size);
    if (R_FAILED(rc)) return rc;

    return appletPushToGeneralChannel(&storage);
}

Result libappletPushInData(AppletHolder *h, const void* buffer, size_t size) {
    return _libappletPushInData(h, buffer, size);
}

Result libappletRequestHomeMenu(void) {
    u8 storagedata[0x10] = {0x53, 0x41, 0x4d, 0x53, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};//RequestHomeMenu
    return _libappletQlaunchRequest(storagedata, sizeof(storagedata));
}

Result libappletRequestJumpToSystemUpdate(void) {
    u8 storagedata[0x10] = {0x53, 0x41, 0x4d, 0x53, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};//RequestJumpToSystemUpdate
    return _libappletQlaunchRequest(storagedata, sizeof(storagedata));
}

