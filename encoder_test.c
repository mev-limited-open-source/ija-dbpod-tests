#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>

#include "dbpod_wtypes.h"
#include "dbpod_coms.h"

static const char *progname;
static int sock;
static ULONG cmdseq;

static LONG setval[4];
static USHORT set_axes;
static USHORT differential;
static USHORT invert_sense;
static USHORT ext_reset;
static USHORT reset_source;

#define HEADER_LEN (sizeof(DBPOD_MSGHDR) - sizeof(ULONG))
#define DIR_SEND    0
#define DIR_REC     1

#define ENDOF(type, member) \
    (offsetof(type, member) + sizeof(((type *)0)->member))

#define ARRAY_SIZE(arr) (sizeof arr / sizeof arr[0])

static void dump_get_encoders_resp(const DBPOD_MSGHDR *hdr)
{
    const DBPOD_RSPBUF_GET_ENCODERS *rsp;
    unsigned int max_axes;
    unsigned int n_axes;
    unsigned int n;

    if (hdr->dwLength < offsetof(DBPOD_RSPBUF_GET_ENCODERS, lReading[0]) -
            sizeof(ULONG))
    {
        printf("*** Crappy message get encoders response len %u\n",
                hdr->dwLength);
        return;
    }
    rsp = (DBPOD_RSPBUF_GET_ENCODERS *)hdr;
    printf("nAxes=%u\n", rsp->nAxes);
    n_axes = rsp->nAxes;
    max_axes = (hdr->dwLength + sizeof(ULONG) -
            offsetof(DBPOD_RSPBUF_GET_ENCODERS, lReading[0])) / sizeof(LONG);
    for (n = 0; n < n_axes && n < max_axes; n++)
    {
        printf("[%u]=%ld ", n, (long)rsp->lReading[n]);
    }
    printf("\n");
    if (n_axes > max_axes)
    {
        printf("*** Axes %u to %u missing from response!\n",
                n_axes, max_axes - 1);
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
    case DBPOD_RSPCODE_GET_ENCODERS:
        dump_get_encoders_resp(hdr);
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

static int get_all_messages(void)
{
    DBPOD_MSGHDR *msghdr;

    while (1)
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

static int do_encoder_config(void)
{
    DBPOD_CMDBUF_ENCODER_CONFIG cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.dwLength = sizeof(cmd) - sizeof(ULONG);
    cmd.hdr.dwSequence = cmdseq++;
    cmd.hdr.wCmd = DBPOD_CMDCODE_ENCODER_CONFIG;
    cmd.wDifferential = differential;
    cmd.wResetEnable = ext_reset;
    cmd.wInvertSense = invert_sense;
    cmd.wResetSource = reset_source;
    return do_cmd(&cmd.hdr);
}

static int do_set_encoders(void)
{
    struct {
        DBPOD_CMDBUF_SET_ENCODERS cmd;
        LONG xtrareadings[ARRAY_SIZE(setval)];
    } m;
    unsigned int a, r;

    memset(&m, 0, sizeof(m));
    m.cmd.nAxisBits = set_axes;
    r = 0;
    for (a = 0; a < ARRAY_SIZE(setval); a++)
    {
        if ((set_axes & (1U << a)) != 0)
        {
            m.cmd.lReading[r++] = setval[a];
        }
    }
    m.cmd.hdr.dwLength =
        offsetof(DBPOD_CMDBUF_SET_ENCODERS, lReading[0]) - sizeof(ULONG) +
        r * sizeof(m.cmd.lReading[0]);
    m.cmd.hdr.dwSequence = cmdseq++;
    m.cmd.hdr.wCmd = DBPOD_CMDCODE_SET_ENCODERS;
    return do_cmd(&m.cmd.hdr);
}

static int do_test(void)
{
    int err;

    err = do_encoder_config();
    if (err < 0)
    {
        return err;
    }
    if (set_axes != 0)
    {
        err = do_set_encoders();
        if (err < 0)
        {
            return err;
        }
    }
    while (err == 0)
    {
        usleep(250000);
        err = do_short_cmd(DBPOD_CMDCODE_GET_ENCODERS);
    }
    return err;
}

int main(int argc, char *argv[])
{
    static const struct option longopts[] =
    {
        { "differential", required_argument, NULL, 'd' },
        { "extern-reset", required_argument, NULL, 'e' },
        { "invert-sense", required_argument, NULL, 'i' },
        { "reset-source", required_argument, NULL, 'r' },
        { "set-0", required_argument, NULL, '0' },
        { "set-1", required_argument, NULL, '1' },
        { "set-2", required_argument, NULL, '2' },
        { "set-3", required_argument, NULL, '3' },
        { 0 },
    };
    static const char opts[] = "d:e:i:r:0:1:2:3:";
    const char *node, *service;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int err = 0;

    progname = argv[0];
    
    while (1)
    {
        char *end;
        char *t;
        int opt;
        unsigned long int tmpu;
        long int tmps;
        int tmperr;

        opt = getopt_long(argc, argv, opts, longopts, NULL);
        if (opt == -1)
        {
            break;
        }
        switch (opt)
        {
        case '0':   /* --set-0=V */
        case '1':   /* --set-1=V */
        case '2':   /* --set-2=V */
        case '3':   /* --set-3=V */
            errno = 0;
            tmps = strtol(optarg, &end, 0);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --set-%c='%s' invalid\n", opt, optarg);
                err = 1;
            }
            else
            {
                unsigned int n = opt - '0';

                setval[n] = tmps;
                set_axes |= 1 << n;
            }
            break;
        case 'd':   /* --differential=BITMASK */
            errno = 0;
            tmperr = 0;
            tmpu = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --differential='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmpu > 15)
            {
                fprintf(stderr, "Differential bits %lu out of range\n", tmpu);
                err = 1;
            }
            else
            {
                differential = tmpu;
            }
            break;
        case 'e':   /* --extern-reset=BITMASK */
            errno = 0;
            tmperr = 0;
            tmpu = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --extern-reset='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmpu > 15)
            {
                fprintf(stderr, "External reset enable bits %lu out of range\n", tmpu);
                err = 1;
            }
            else
            {
                ext_reset = tmpu;
            }
            break;
        case 'i':   /* --invert-sense=BITMASK */
            errno = 0;
            tmperr = 0;
            tmpu = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --invert-sense='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmpu > 15)
            {
                fprintf(stderr, "Invert sense bits %lu out of range\n", tmpu);
                err = 1;
            }
            else
            {
                invert_sense = tmpu;
            }
            break;
        case 'r':   /* --reset-source=VAL */
            errno = 0;
            tmperr = 0;
            tmpu = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --reset-source='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmpu > 65535)
            {
                fprintf(stderr, "Reset source %lu out of range\n", tmpu);
                err = 1;
            }
            else
            {
                reset_source = tmpu;
            }
            break;
        default:
            err = 1;
            break;
        }
    }

    if (err || argc - optind != 2)
    {
        fprintf(stderr,
                "usage: %s [OPTIONS] HOST PORT\n"
                "Options:\n"
                " -d, --differential=BITMASK\n"
                " -e, --extern-reset=BITMASK\n"
                " -i, --invert-source=BITMASK\n"
                " -r, --reset-source=N\n"
                " -0, --set-0=VAL\n"
                " -1, --set-1=VAL\n"
                " -2, --set-2=VAL\n"
                " -3, --set-3=VAL\n",
                progname);
        return 2;
    }

    node = argv[optind];
    service = argv[optind + 1];
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

