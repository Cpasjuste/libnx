#include <string.h>
#include <switch.h>

//This implements the version of Android IGraphicBufferProducer used by Switch, hence names/params etc here are based on Android IGraphicBufferProducer.cpp.

//Based on an old version of the enum from the above .cpp.
//Unknown whether all of these are correct for Switch.
enum {
    REQUEST_BUFFER = BINDER_FIRST_CALL_TRANSACTION, //0x1
    SET_BUFFER_COUNT, //0x2
    DEQUEUE_BUFFER, //0x3
    DETACH_BUFFER, //0x4
    DETACH_NEXT_BUFFER, //0x5
    ATTACH_BUFFER, //0x6
    QUEUE_BUFFER, //0x7
    CANCEL_BUFFER, //0x8
    QUERY, //0x9
    CONNECT, //0xA
    DISCONNECT, //0xB
    SET_SIDEBAND_STREAM, //0xC
    ALLOCATE_BUFFERS, //0xD
    TEGRA_BUFFER_INIT, //0xE (Custom Switch-specific command - unofficial name)
};

static char g_gfxproducer_InterfaceDescriptor[] = "android.gui.IGraphicBufferProducer";

static binderSession *g_gfxproducerBinderSession;

Result gfxproducerInitialize(binderSession *session) {
    g_gfxproducerBinderSession = session;

    return 0;
}

void gfxproducerExit() {
    g_gfxproducerBinderSession = NULL;
}

Result gfxproducerRequestBuffer(s32 bufferIdx) {
    Result rc;
    parcelContext parcel, parcel_reply;

    if (g_gfxproducerBinderSession==NULL) return MAKERESULT(MODULE_LIBNX, LIBNX_NOTINITIALIZED);

    parcelInitializeContext(&parcel);
    parcelInitializeContext(&parcel_reply);

    parcelWriteInterfaceToken(&parcel, g_gfxproducer_InterfaceDescriptor);
    parcelWriteInt32(&parcel, bufferIdx);

    rc = parcelTransact(g_gfxproducerBinderSession, REQUEST_BUFFER, &parcel, &parcel_reply);
    if (R_FAILED(rc)) return rc;

    //TODO: parse reply

    return 0;
}

Result gfxproducerDequeueBuffer(bool async, u32 width, u32 height, s32 format, u32 usage, s32 *buf) {
    Result rc;
    parcelContext parcel, parcel_reply;

    if (g_gfxproducerBinderSession==NULL) return MAKERESULT(MODULE_LIBNX, LIBNX_NOTINITIALIZED);

    parcelInitializeContext(&parcel);
    parcelInitializeContext(&parcel_reply);

    parcelWriteInterfaceToken(&parcel, g_gfxproducer_InterfaceDescriptor);

    parcelWriteInt32(&parcel, async);
    parcelWriteUInt32(&parcel, width);
    parcelWriteUInt32(&parcel, height);
    parcelWriteInt32(&parcel, format);
    parcelWriteUInt32(&parcel, usage);

    rc = parcelTransact(g_gfxproducerBinderSession, DEQUEUE_BUFFER, &parcel, &parcel_reply);
    if (R_FAILED(rc)) return rc;

    //TODO: parse reply

    *buf = parcelReadInt32(&parcel_reply);

    return 0;
}

Result gfxproducerDetachBuffer(s32 slot) {
    Result rc;
    parcelContext parcel, parcel_reply;

    if (g_gfxproducerBinderSession==NULL) return MAKERESULT(MODULE_LIBNX, LIBNX_NOTINITIALIZED);

    parcelInitializeContext(&parcel);
    parcelInitializeContext(&parcel_reply);

    parcelWriteInterfaceToken(&parcel, g_gfxproducer_InterfaceDescriptor);
    parcelWriteInt32(&parcel, slot);

    rc = parcelTransact(g_gfxproducerBinderSession, DETACH_BUFFER, &parcel, &parcel_reply);
    if (R_FAILED(rc)) return rc;

    //TODO: parse reply

    return 0;
}

Result gfxproducerQueueBuffer(s32 buf, u8 input[0x5c]) {
    Result rc;
    parcelContext parcel, parcel_reply;

    if (g_gfxproducerBinderSession==NULL) return MAKERESULT(MODULE_LIBNX, LIBNX_NOTINITIALIZED);

    parcelInitializeContext(&parcel);
    parcelInitializeContext(&parcel_reply);

    parcelWriteInterfaceToken(&parcel, g_gfxproducer_InterfaceDescriptor);

    parcelWriteInt32(&parcel, buf);
    parcelWriteData(&parcel, input, 0x5c);

    rc = parcelTransact(g_gfxproducerBinderSession, QUEUE_BUFFER, &parcel, &parcel_reply);
    if (R_FAILED(rc)) return rc;

    //TODO: parse reply

    return 0;
}

Result gfxproducerQuery(s32 what, s32* value) {
    Result rc;
    parcelContext parcel, parcel_reply;

    if (g_gfxproducerBinderSession==NULL) return MAKERESULT(MODULE_LIBNX, LIBNX_NOTINITIALIZED);

    parcelInitializeContext(&parcel);
    parcelInitializeContext(&parcel_reply);

    parcelWriteInterfaceToken(&parcel, g_gfxproducer_InterfaceDescriptor);

    parcelWriteInt32(&parcel, what);

    rc = parcelTransact(g_gfxproducerBinderSession, QUERY, &parcel, &parcel_reply);
    if (R_FAILED(rc)) return rc;

    *value = parcelReadInt32(&parcel_reply);
    rc = parcelReadInt32(&parcel_reply);
    if (rc) rc = MAKERESULT(MODULE_LIBNX, LIBNX_PARCEL_ERRBASE);

    return rc;
}

Result gfxproducerConnect(s32 api, bool producerControlledByApp) {
    Result rc;
    parcelContext parcel, parcel_reply;

    if (g_gfxproducerBinderSession==NULL) return MAKERESULT(MODULE_LIBNX, LIBNX_NOTINITIALIZED);

    parcelInitializeContext(&parcel);
    parcelInitializeContext(&parcel_reply);

    parcelWriteInterfaceToken(&parcel, g_gfxproducer_InterfaceDescriptor);

    //Hard-code this as if listener==NULL, since that's not known to be used officially.
    parcelWriteInt32(&parcel, 0);

    parcelWriteInt32(&parcel, api);
    parcelWriteInt32(&parcel, producerControlledByApp);

    rc = parcelTransact(g_gfxproducerBinderSession, CONNECT, &parcel, &parcel_reply);
    if (R_FAILED(rc)) return rc;

    //TODO: parse reply (contains 32bit width and height)

    return 0;
}

Result gfxproducerDisconnect(s32 api) {
    Result rc;
    parcelContext parcel, parcel_reply;

    if (g_gfxproducerBinderSession==NULL) return MAKERESULT(MODULE_LIBNX, LIBNX_NOTINITIALIZED);

    parcelInitializeContext(&parcel);
    parcelInitializeContext(&parcel_reply);

    parcelWriteInterfaceToken(&parcel, g_gfxproducer_InterfaceDescriptor);

    parcelWriteInt32(&parcel, api);

    rc = parcelTransact(g_gfxproducerBinderSession, DISCONNECT, &parcel, &parcel_reply);
    if (R_FAILED(rc)) return rc;

    //TODO: parse reply

    return 0;
}

Result gfxproducerTegraBufferInit(s32 buf, u8 input[0x178]) {
    Result rc;
    parcelContext parcel, parcel_reply;

    if (g_gfxproducerBinderSession==NULL) return MAKERESULT(MODULE_LIBNX, LIBNX_NOTINITIALIZED);

    parcelInitializeContext(&parcel);
    parcelInitializeContext(&parcel_reply);

    parcelWriteInterfaceToken(&parcel, g_gfxproducer_InterfaceDescriptor);

    parcelWriteInt32(&parcel, buf);
    parcelWriteData(&parcel, input, 0x178);

    rc = parcelTransact(g_gfxproducerBinderSession, TEGRA_BUFFER_INIT, &parcel, &parcel_reply);
    if (R_FAILED(rc)) return rc;

    //TODO: parse reply

    return 0;
}

