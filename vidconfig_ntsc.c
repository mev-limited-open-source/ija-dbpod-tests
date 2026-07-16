#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>

#include "dbpod_wtypes.h"
#include "dbpod_coms.h"

static const char *progname;
static int sock;
static ULONG cmdseq;

#define HEADER_LEN (sizeof(DBPOD_MSGHDR) - sizeof(ULONG))
#define DIR_SEND    0
#define DIR_REC     1

#define ENDOF(type, member) \
    (offsetof(type, member) + sizeof(((type *)0)->member))

static void dump_msg(const DBPOD_MSGHDR *hdr, int direction)
{
    const char *dirstr = (direction == DIR_SEND ? "Send" : "Receive");
    const char *msgstr;
    const char *flagstr;

    if (hdr->dwLength < sizeof(DBPOD_MSGHDR) - sizeof(ULONG))
    {
        printf("*** %s Crappy message len %u\n", dirstr, hdr->dwLength);
        return;
    }
    switch (hdr->wCmd & ~DBPOD_MSGFLAG_SYNC_RESPONSE)
    {
    case DBPOD_CMDCODE_GET_CAPABILITIES:
        msgstr = "GET_CAPABILITIES";
        break;
    case DBPOD_CMDCODE_DIAGS:
        msgstr = "DIAGS";
        break;
    case DBPOD_CMDCODE_DUMMY:
        msgstr = "DUMMY";
        break;
    case DBPOD_CMDCODE_START_UT:
        msgstr = "START_UT";
        break;
    case DBPOD_CMDCODE_STOP_UT:
        msgstr = "STOP_UT";
        break;
    case DBPOD_CMDCODE_START_VC:
        msgstr = "START_VC";
        break;
    case DBPOD_CMDCODE_STOP_VC:
        msgstr = "STOP_VC";
        break;
    case DBPOD_CMDCODE_SET_FLASH_PARAMS:
        msgstr = "SET_FLASH_PARAMS";
        break;
    case DBPOD_CMDCODE_GET_FLASH_PARAMS:
        msgstr = "GET_FLASH_PARAMS";
        break;
    case DBPOD_CMDCODE_GET_MAC_ADDRESS:
        msgstr = "GET_MAC_ADDRESS";
        break;
    case DBPOD_CMDCODE_CHAN_CONFIG:
        msgstr = "CHAN_CONFIG";
        break;
    case DBPOD_CMDCODE_DAC_MEMORY_SET:
        msgstr = "DAC_MEMORY_SET";
        break;
    case DBPOD_CMDCODE_ENCODER_CONFIG:
        msgstr = "ENCODER_CONFIG";
        break;
    case DBPOD_CMDCODE_VIDEO_CONFIG:
        msgstr = "VIDEO_CONFIG";
        break;
    case DBPOD_CMDCODE_SCAN_CONFIG:
        msgstr = "SCAN_CONFIG";
        break;
    case DBPOD_CMDCODE_SET_ENCODERS:
        msgstr = "SET_ENCODERS";
        break;
    case DBPOD_CMDCODE_GET_ENCODERS:
        msgstr = "GET_ENCODERS";
        break;
    case DBPOD_CMDCODE_SET_LED:
        msgstr = "SET_LED";
        break;
    case DBPOD_CMDCODE_GET_LED:
        msgstr = "GET_LED";
        break;
    case DBPOD_CMDCODE_MDU_CONFIG:
        msgstr = "MDU_CONFIG";
        break;
    case DBPOD_CMDCODE_MDU_DATA:
        msgstr = "MDU_DATA";
        break;
    case DBPOD_CMDCODE_SYNC_VIDEO:
        msgstr = "SYNC_VIDEO";
        break;
    case DBPOD_CMDCODE_START_ASYNC_VIDEO:
        msgstr = "START_ASYNC_VIDEO";
        break;
    case DBPOD_CMDCODE_STOP_ASYNC_VIDEO:
        msgstr = "STOP_ASYNC_VIDEO";
        break;
    case DBPOD_MSGCODE_ASYNC_VIDEO:
        msgstr = "ASYNC_VIDEO";
        break;
    case DBPOD_CMDCODE_SYNC_RECORD:
        msgstr = "SYNC_RECORD";
        break;
    case DBPOD_CMDCODE_START_ASYNC_RECORD:
        msgstr = "START_ASYNC_RECORD";
        break;
    case DBPOD_CMDCODE_STOP_ASYNC_RECORD:
        msgstr = "STOP_ASYNC_RECORD";
        break;
    case DBPOD_MSGCODE_ASYNC_RECORD:
        msgstr = "ASYNC_RECORD";
        break;
    case DBPOD_CMDCODE_SYNC_DISPLAY:
        msgstr = "SYNC_DISPLAY";
        break;
    case DBPOD_MSGCODE_ASYNC_ERROR:
        msgstr = "ASYNC_ERROR";
        break;
    case DBPOD_CMDCODE_ABORT:
        msgstr = "ABORT";
        break;
    case DBPOD_CMDCODE_SOFT_RESET:
        msgstr = "SOFT_RESET";
        break;
    default:
        msgstr = "unknown";
        break;
    }

    switch (hdr->wCmd & (DBPOD_MSGFLAG_SYNC_RESPONSE |
                DBPOD_MSGFLAG_ASYNC_RESPONSE))
    {
    case 0:
        flagstr = "Command";
        break;
    case DBPOD_MSGFLAG_ASYNC_RESPONSE:
        flagstr = "Async Response";
        break;
    case DBPOD_MSGFLAG_SYNC_RESPONSE:
        flagstr = "Sync Response";
        break;
    default:
        flagstr = "Undefined";
        break;
    }

    printf("*** %s %s %s (0x%04X) len %u seq %u subcode %u\n",
            dirstr, msgstr, flagstr, hdr->wCmd, hdr->dwLength, hdr->dwSequence,
            hdr->wSubCode);

    switch (hdr->wCmd)
    {
    default:
        break;
    }
}

static int get_bytes(void *buf, size_t len)
{
    while (len)
    {
        ssize_t ret = read(sock, buf, len);

        if (ret < 0)
        {
            perror("read socket");
            return -1;
        }
        if (ret == 0)
        {
            fprintf(stderr, "EOF on socket\n");
            return -1;
        }
        len -= ret;
        buf = (char *)buf + ret;
    }
    return 0;
}

static DBPOD_MSGHDR *get_message(void)
{
    DBPOD_MSGHDR *msg;
    ULONG dwLength;
    int ret;

    ret = get_bytes(&dwLength, sizeof(dwLength));
    if (ret < 0)
    {
        return NULL;
    }
    msg = malloc(dwLength + sizeof(dwLength));
    if (!msg)
    {
        fprintf(stderr, "Failed to allocate memory for message\n");
        return NULL;
    }
    msg->dwLength = dwLength;
    ret = get_bytes((char *)msg + sizeof(dwLength), dwLength);
    if (ret < 0)
    {
        free(msg);
        return NULL;
    }
    dump_msg(msg, DIR_REC);
    if (msg->dwLength < sizeof(DBPOD_MSGHDR) - sizeof(dwLength))
    {
        free(msg);
        return NULL;
    }
    return msg;
}

static int expect_reply(USHORT msgcode, ULONG msgseq)
{
    DBPOD_MSGHDR *msghdr;
    int found = 0;

    while (!found)
    {
        msghdr = get_message();
        if (!msghdr)
        {
            break;
        }
        if (msghdr->wCmd == msgcode && msghdr->dwSequence == msgseq)
        {
            found = 1;
        }
        free(msghdr);
    }
    return found ? 0 : -1;
}

static int get_all_or_n_messages(int continuous, unsigned int n)
{
    DBPOD_MSGHDR *msghdr;

    while (continuous || n--)
    {
        msghdr = get_message();
        if (!msghdr)
        {
            return -1;
        }
        free(msghdr);
    }
    return 0;
}

static int get_n_messages(unsigned int n)
{
    return get_all_or_n_messages(0, n);
}

static int get_all_messages(void)
{
    return get_all_or_n_messages(1, 0);
}

static int do_cmd(const DBPOD_MSGHDR *cmdhdr)
{
    ULONG left = sizeof(ULONG) + cmdhdr->dwLength;
    const char *s = (const char *)cmdhdr;

    /* send command */
    dump_msg(cmdhdr, DIR_SEND);
    while (left)
    {
        ssize_t ret = write(sock, s, left);

        if (ret < 0)
        {
            perror("write socket");
            return -1;
        }
        left -= ret;
        s += ret;
    }
    return expect_reply(cmdhdr->wCmd | DBPOD_MSGFLAG_SYNC_RESPONSE,
            cmdhdr->dwSequence);
}

static int do_short_cmd(USHORT cmdcode)
{
    DBPOD_MSGHDR cmdhdr;
    int err;

    memset(&cmdhdr, 0, sizeof(cmdhdr));
    cmdhdr.dwLength = HEADER_LEN;
    cmdhdr.dwSequence = cmdseq++;
    cmdhdr.wCmd = cmdcode;
    return do_cmd(&cmdhdr);
}

static int do_video_config(void)
{
    DBPOD_CMDBUF_VIDEO_CONFIG cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.dwLength = sizeof(cmd) - sizeof(ULONG);
    cmd.hdr.dwSequence = cmdseq++;
    cmd.hdr.wCmd = DBPOD_CMDCODE_VIDEO_CONFIG;
    cmd.nStandard = DBPOD_VID_STD_NTSC;
    cmd.nInterleave = DBPOD_VID_INTERLEAVE_BOTH;
    cmd.nTrkFrameRate = 30;
    cmd.nXTrkFrameSize = 640;
    cmd.nYTrkFrameSize = 480;
    cmd.nXTrkOrigin = 0;
    cmd.nYTrkOrigin = 0;
    cmd.nXTrkCropSize = 640;
    cmd.nYTrkCropSize = 480;
    cmd.nStrmSizeScale = 1;
    cmd.nStrmRateScale = 1;
    cmd.fAGC = TRUE;
    cmd.nAGCThreshold = 128;
    cmd.nFormat = DBPOD_VID_RAWGREY8;
    cmd.nNoDotThreshold = 128;
    return do_cmd(&cmd.hdr);
}

static int do_test(void)
{
    int err;

    err = do_video_config();
    return err;
}

int main(int argc, char *argv[])
{
    const char *node, *service;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int err;

    progname = argv[0];
    if (argc < 3)
    {
        fprintf(stderr, "usage: %s HOST PORT\n", progname);
        exit(2);
    }
    node = argv[1];
    service = argv[2];
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    err = getaddrinfo(node, service, &hints, &result);
    if (err)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(EXIT_FAILURE);
    }
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1)
        {
            continue;
        }
        if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
        {
            break;  /* success */
        }
        close(sock);
    }
    if (!rp)
    {
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);
    printf("Connected to %s:%s\n", node, service);
    err = do_test();
    close(sock);
    if (err < 0)
    {
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
    return 0;
}

/* vi: set ai et sw=4 ts=4: */
