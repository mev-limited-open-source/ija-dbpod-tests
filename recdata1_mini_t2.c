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

static void dump_ut_resp(const DBPOD_MSGHDR *hdr)
{
    ULONG chunks, ch;
    ULONG pos, msgend_pos;
    const char *start;

    start = (const char *)hdr;
    pos = 0;
    msgend_pos = hdr->dwLength + sizeof(ULONG);
    if (hdr->dwLength < sizeof(DBPOD_MSGBUF_UT_SOM) - sizeof(ULONG))
    {
        printf("*** Crappy UT response len %u\n", hdr->dwLength);
        return;
    }
    chunks = ((const DBPOD_MSGBUF_UT_SOM *)hdr)->nChunks;
    printf("nChunks=%u,\n", chunks);
    pos += sizeof(DBPOD_MSGBUF_UT_SOM);
    for (ch = 0; ch < chunks; ch++)
    {
        const DBPOD_CHUNK_UT_SOC *soc;
        const DBPOD_CHUNK_UT_EOC *eoc;

        if (pos + sizeof(DBPOD_CHUNK_UT_SOC) > msgend_pos)
        {
            printf("*** No room for start of chunk %u\n", ch);
            return;
        }
        soc = (const DBPOD_CHUNK_UT_SOC *)(start + pos);
        printf("Chunk[%u]: dwLength=%u, dwSeq=%u, lQuadPos[0]=%d, lQuadPos[1]=%d,\n",
                ch, soc->dwLength, soc->dwSeq, soc->lQuadPos[0],
                soc->lQuadPos[1]);
        printf("  uYPos=%u, uXPos=%u, uAmp=%u, (dwVt=0x%X),\n",
                soc->vt.uYPos, soc->vt.uXPos, soc->vt.uAmp, soc->dwVt);
        if (soc->dwLength < sizeof(DBPOD_CHUNK_UT_SOC) - sizeof(ULONG)
                + sizeof(DBPOD_CHUNK_UT_EOC))
        {
            printf("  *** Chunk length too small\n");
            return;
        }
        if (soc->dwLength > msgend_pos
                || soc->dwLength + sizeof(ULONG) > msgend_pos
                || pos + soc->dwLength + sizeof(ULONG) > msgend_pos)
        {
            printf("  *** Chunk doesn't fit in message\n");
            return;
        }
        pos += soc->dwLength + sizeof(ULONG) - sizeof(DBPOD_CHUNK_UT_EOC);
        eoc = (const DBPOD_CHUNK_UT_EOC *)(start + pos);
        printf("  fLossSig=%d, Interface=(%d,%d),\n",
                eoc->fLossSig, eoc->Interface.Amplitude,
                eoc->Interface.Position);
        printf("  Peak[0..15]={(%d,%d),(%d,%d),(%d,%d),(%d,%d),\n",
                eoc->Peak[0].Amplitude, eoc->Peak[0].Position,
                eoc->Peak[1].Amplitude, eoc->Peak[1].Position,
                eoc->Peak[2].Amplitude, eoc->Peak[2].Position,
                eoc->Peak[3].Amplitude, eoc->Peak[3].Position);
        printf("    (%d,%d),(%d,%d),(%d,%d),(%d,%d),\n",
                eoc->Peak[4].Amplitude, eoc->Peak[4].Position,
                eoc->Peak[5].Amplitude, eoc->Peak[5].Position,
                eoc->Peak[6].Amplitude, eoc->Peak[6].Position,
                eoc->Peak[7].Amplitude, eoc->Peak[7].Position);
        printf("    (%d,%d),(%d,%d),(%d,%d),(%d,%d),\n",
                eoc->Peak[8].Amplitude, eoc->Peak[8].Position,
                eoc->Peak[9].Amplitude, eoc->Peak[9].Position,
                eoc->Peak[10].Amplitude, eoc->Peak[10].Position,
                eoc->Peak[11].Amplitude, eoc->Peak[11].Position);
        printf("    (%d,%d),(%d,%d),(%d,%d),(%d,%d)}\n",
                eoc->Peak[12].Amplitude, eoc->Peak[12].Position,
                eoc->Peak[13].Amplitude, eoc->Peak[13].Position,
                eoc->Peak[14].Amplitude, eoc->Peak[14].Position,
                eoc->Peak[15].Amplitude, eoc->Peak[15].Position);
        pos += sizeof(DBPOD_CHUNK_UT_EOC);
    }
}

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
    case DBPOD_MSGCODE_ASYNC_RECORD:
    case DBPOD_RSPCODE_SYNC_RECORD:
    case DBPOD_RSPCODE_SYNC_DISPLAY:
        dump_ut_resp(hdr);
        break;
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

static int do_chan_configs(void)
{
    int ret;
    struct {
        DBPOD_CMDBUF_CHAN_CONFIG cmd;
        DBPOD_GATECFG xtragates[5];
    } m;

    memset(&m, 0, sizeof(m));
    m.cmd.hdr.wCmd = DBPOD_CMDCODE_CHAN_CONFIG;

    m.cmd.nIndex = 0;
    m.cmd.nDigBits = 8;
    m.cmd.dwDelay = 10000;
    m.cmd.nRepeat = 1;
    m.cmd.nTx = 0;
    m.cmd.nRx = 0;
    m.cmd.nVoltage = 100;
    m.cmd.nPulseWidth = 50;
    m.cmd.nGain = 0;
    m.cmd.nHPF = 0;
    m.cmd.nLPF = 0;
    m.cmd.RectifierType = DBPOD_RECTIFIER_NONE;
    m.cmd.nRFF = 0;
    m.cmd.nSampleFreq = 200000000;
    m.cmd.dwDACStart = 0;
    m.cmd.dwDACLength = 0;
    m.cmd.nDACDivisor = 2;
    m.cmd.DacTrigger = 0;
    m.cmd.nGates = 2;
    m.cmd.Gate[0].GateType = DBPOD_GATETYPE_AVERAGING;
    m.cmd.Gate[0].Reference = DBPOD_GATEREF_ABSOLUTE;
    m.cmd.Gate[0].nThreshold = 0;
    m.cmd.Gate[0].lStart = 0;
    m.cmd.Gate[0].lWidth = 1000;
    m.cmd.Gate[0].nControl = 0;
    m.cmd.Gate[0].lParameter = 0;
    m.cmd.Gate[1].GateType = DBPOD_GATETYPE_RECORD;
    m.cmd.Gate[1].Reference = DBPOD_GATEREF_ABSOLUTE;
    m.cmd.Gate[1].nThreshold = 0;
    m.cmd.Gate[1].lStart = 0;
    m.cmd.Gate[1].lWidth = 1000;
    m.cmd.Gate[1].nControl = 0;
    m.cmd.Gate[1].lParameter = 0;

    m.cmd.hdr.dwLength =
        offsetof(DBPOD_CMDBUF_CHAN_CONFIG, Gate[0]) - sizeof(ULONG) +
        m.cmd.nGates * sizeof(m.cmd.Gate[0]);
    m.cmd.hdr.dwSequence = cmdseq++;
    // index 0
    m.cmd.nRx = 0;
    ret = do_cmd(&m.cmd.hdr);
    if (ret)
    {
        return ret;
    }
    // index 1
    m.cmd.nIndex++;
    m.cmd.nRx = 0;
    m.cmd.Gate[0].lStart = 200; //XXX
    m.cmd.Gate[0].lWidth = 0; // XXX
    m.cmd.Gate[1].lStart = 200; //XXX
    m.cmd.Gate[1].lWidth = 0; // XXX
    m.cmd.hdr.dwSequence = cmdseq++;
    ret = do_cmd(&m.cmd.hdr);
    if (ret)
    {
        return ret;
    }
#if 1
    // index 2
    m.cmd.nIndex++;
    m.cmd.nRx = 0;
    m.cmd.Gate[0].lStart = 0;
    m.cmd.Gate[0].lWidth = 1000;
    m.cmd.Gate[1].lStart = 0;
    m.cmd.Gate[1].lWidth = 1000;
    m.cmd.hdr.dwSequence = cmdseq++;
    ret = do_cmd(&m.cmd.hdr);
#endif
    return ret;
}

static int do_scan_config(void)
{
    DBPOD_CMDBUF_SCAN_CONFIG cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.dwLength = sizeof(cmd) - sizeof(ULONG);
    cmd.hdr.dwSequence = cmdseq++;
    cmd.hdr.wCmd = DBPOD_CMDCODE_SCAN_CONFIG;
    cmd.ScanType = DBPOD_SCAN_FREERUN;
    return do_cmd(&cmd.hdr);
}

static int do_start_ut(void)
{
    DBPOD_CMDBUF_START_UT cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.dwLength = sizeof(cmd) - sizeof(ULONG);
    cmd.hdr.dwSequence = cmdseq++;
    cmd.hdr.wCmd = DBPOD_CMDCODE_START_UT;
    cmd.nStartIndex = 0;
    cmd.nEndIndex = 2;
    cmd.fMaster = 1;
    cmd.dwFlags = 0;
    return do_cmd(&cmd.hdr);
}

static int do_start_async_record(void)
{
    DBPOD_CMDBUF_START_ASYNC_RECORD cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.dwLength = sizeof(cmd) - sizeof(ULONG);
    cmd.hdr.dwSequence = cmdseq++;
    cmd.hdr.wCmd = DBPOD_CMDCODE_START_ASYNC_RECORD;
    cmd.dwBufferSize = 10000;
    return do_cmd(&cmd.hdr);
}

static int do_test(void)
{
    int err;

    err = do_chan_configs();
    if (err < 0)
    {
        return err;
    }
    err = do_scan_config();
    if (err < 0)
    {
        return err;
    }
    err = do_start_ut();
    if (err < 0)
    {
        return err;
    }
    err = do_start_async_record();
    if (err < 0)
    {
        return err;
    }
    return get_all_messages();
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
