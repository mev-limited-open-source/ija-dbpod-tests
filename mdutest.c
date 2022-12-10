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

static ULONG baud = 9600;
static UCHAR data_bits = 8;
static UCHAR parity = DBPOD_MDU_PARITY_NONE;
static UCHAR stop_bits = 1;
static UCHAR mdu_port = 0;
static ULONG timeout = 0;
static CHAR continuation = 0;
static char *mdu_data = NULL;

#define HEADER_LEN (sizeof(DBPOD_MSGHDR) - sizeof(ULONG))
#define DIR_SEND    0
#define DIR_REC     1

#define ENDOF(type, member) \
    (offsetof(type, member) + sizeof(((type *)0)->member))

static void dump_mdu_config_cmd(const DBPOD_MSGHDR *hdr)
{
    const DBPOD_CMDBUF_MDU_CONFIG *cmd = (const DBPOD_CMDBUF_MDU_CONFIG *)hdr;
    const char *paritystr;

    switch (cmd->nParity)
    {
    case DBPOD_MDU_PARITY_NONE:
        paritystr = "None";
        break;
    case DBPOD_MDU_PARITY_ODD:
        paritystr = "Odd";
        break;
    case DBPOD_MDU_PARITY_EVEN:
        paritystr = "Even";
        break;
    default:
        paritystr = "?";
        break;
    }
    printf("dwSpeed=%u, nDataBits=%u, nParity=%u (%s), nStopBits=%u, nPort=%u\n",
            (unsigned)cmd->dwSpeed, cmd->nDataBits, cmd->nParity, paritystr,
            cmd->nStopBits, cmd->nPort);
}

static void print_dqescape(const char *s, size_t len)
{
    size_t n;

    for (n = 0; n < len; n++)
    {
        int c = s[n];
        int uc = (unsigned char)s[n];

        if (isprint(uc) && c != '"' && c != '\\')
        {
            putc(c, stdout);
        }
        else
        {
            int e = EOF;

            switch (c)
            {
            case '"':
            case '\\':
                e = c;
                break;
            case '\a':
                e = 'a';
                break;
            case '\b':
                e = 'b';
                break;
            case '\f':
                e = 'f';
                break;
            case '\n':
                e = 'n';
                break;
            case '\r':
                e = 'r';
                break;
            case '\t':
                e = 't';
                break;
            case '\v':
                e = 'v';
                break;
            }
            if (e == EOF)
            {
                printf("\\x%02X", uc);
            }
            else
            {
                printf("\\%c", e);
            }
        }
    }
}

static void dump_mdu_data(const DBPOD_MSGHDR *hdr)
{
    /* Command and response have same format. */
    const DBPOD_CMDBUF_MDU_DATA *msg;
    unsigned datalen;

    /* Might be too short due to error response. */
    if (hdr->dwLength <
            offsetof(DBPOD_CMDBUF_MDU_DATA, bData[0]) - sizeof(ULONG))
    {
        printf("*** Crappy MDU data command/response message len %u\n",
                hdr->dwLength); 
        return;
    }
    msg = (const DBPOD_CMDBUF_MDU_DATA *)hdr;
    datalen = msg->wLength;
    printf("dwTimeout=%u ms, wLength=%u, nPort=%u, fContinuation=%d,\n",
            (unsigned)msg->dwTimeout, msg->wLength, msg->nPort,
            msg->fContinuation);
    if (msg->hdr.dwLength <
            offsetof(DBPOD_CMDBUF_MDU_DATA, bData[datalen]) - sizeof(ULONG))
    {
        printf("*** MDU data too short\n");
        datalen = msg->hdr.dwLength + sizeof(ULONG) -
            offsetof(DBPOD_CMDBUF_MDU_DATA, bData[0]);
    }
    printf("bData[]=\"");
    print_dqescape((const char *)msg->bData, datalen);
    printf("\"\n");
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
    case DBPOD_CMDCODE_MDU_CONFIG:
        dump_mdu_config_cmd(hdr);
        break;
    case DBPOD_CMDCODE_MDU_DATA:
    case DBPOD_RSPCODE_MDU_DATA:
        dump_mdu_data(hdr);
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

    memset(&cmdhdr, 0, sizeof(cmdhdr));
    cmdhdr.dwLength = HEADER_LEN;
    cmdhdr.dwSequence = cmdseq++;
    cmdhdr.wCmd = cmdcode;
    return do_cmd(&cmdhdr);
}

static int do_mdu_config(void)
{
    DBPOD_CMDBUF_MDU_CONFIG cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.dwLength = sizeof(cmd) - sizeof(ULONG);
    cmd.hdr.dwSequence = cmdseq++;
    cmd.hdr.wCmd = DBPOD_CMDCODE_MDU_CONFIG;
    cmd.dwSpeed = baud;
    cmd.nDataBits = data_bits;
    cmd.nParity = parity;
    cmd.nStopBits = stop_bits;
    cmd.nPort = mdu_port;
    return do_cmd(&cmd.hdr);
}

static int do_mdu_data(void)
{
    DBPOD_CMDBUF_MDU_DATA *cmd;
    ULONG datalen;
    ULONG msglen;
    int err;

    datalen = strlen(mdu_data);
    if (datalen > 0xFFFF)
    {
        datalen = 0xFFFF;
    }
    msglen = offsetof(DBPOD_CMDBUF_MDU_DATA, bData[datalen]);
    cmd = malloc(msglen);
    if (!cmd)
    {
        errno = ENOMEM;
        return -1;
    }
    memset(cmd, 0, offsetof(DBPOD_CMDBUF_MDU_DATA, bData[0]));
    cmd->hdr.dwLength = msglen - sizeof(ULONG);
    cmd->hdr.dwSequence = cmdseq++;
    cmd->hdr.wCmd = DBPOD_CMDCODE_MDU_DATA;
    cmd->dwTimeout = timeout;
    cmd->wLength = datalen;
    cmd->nPort = mdu_port;
    cmd->fContinuation = continuation;
    memcpy(cmd->bData, mdu_data, datalen);
    err = do_cmd(&cmd->hdr);
    free(cmd);
    return err;
}

static int do_test(void)
{
    int err;

    err = do_mdu_config();
    if (err < 0)
    {
        return err;
    }
    if (mdu_data)
    {
        err = do_mdu_data();
        if (err < 0)
        {
            return err;
        }
    }
    return err;
}

int main(int argc, char *argv[])
{
    enum
    {
        OPT_HELP = 1000
    };
    static const struct option longopts[] =
    {
        { "baud", required_argument, NULL, 'b' },
        { "continuation", no_argument, NULL, 'c' },
        { "data-bits", required_argument, NULL, 'd' },
        { "mdu-port", required_argument, NULL, 'm' },
        { "parity", required_argument, NULL, 'p' },
        { "stop-bits", required_argument, NULL, 's' },
        { "timeout", required_argument, NULL, 't' },
        { "help", no_argument, NULL, OPT_HELP },
    };
    static const char opts[] = "b:cd:m:p:s:t:";
    const char *node, *service;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int err = 0;
    int help = 0;

    progname = argv[0];

    while (1)
    {
        char *end;
        char *t;
        int opt;
        unsigned long int tmp, tmp2;
        int tmperr;

        opt = getopt_long(argc, argv, opts, longopts, NULL);
        if (opt == -1)
        {
            break;
        }
        switch (opt)
        {
        case 'b':   /* --baud=SPEED */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --baud='%s' invalid\n", optarg);
                err = 1;
            }
            else
            {
                switch (tmp)
                {
                default:
                    fprintf(stderr, "Unsupported baud rate %lu\n", tmp);
                    err = 1;
                    break;
                case 50:
                case 75:
                case 110:
                case 134:
                case 150:
                case 200:
                case 300:
                case 600:
                case 1200:
                case 1800:
                case 2400:
                case 4800:
                case 9600:
                case 19200:
                case 38400:
                case 57600:
                case 115200:
                case 230400:
                    baud = tmp;
                    /* baud_specified = 1; */
                    break;
                }
            }
            break;
        case 'c':   /* --continuation */
            continuation = 1;
            break;
        case 'd':   /* --data-bits=N */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --data-bits='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp < 5 || tmp > 8)
            {
                fprintf(stderr, "Data bits %lu out of range\n", tmp);
                err = 1;
            }
            else
            {
                data_bits = tmp;
            }
            /* data_bits_specified = 1; */
            break;
        case 'm':   /* --mdu-port=MDUPORT */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --mdu='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 0xFF)
            {
                fprintf(stderr, "MDU port %lu out of range\n", tmp);
                err = 1;
            }
            mdu_port = tmp;
            /* mdu_port_specified = 1; */
            break;
        case 'p':   /* --parity={none|odd|even} */
            if (!strcasecmp(optarg, "none") || !strcasecmp(optarg, "n"))
            {
                parity = DBPOD_MDU_PARITY_NONE;
            }
            else if (!strcasecmp(optarg, "odd") || !strcasecmp(optarg, "o"))
            {
                parity = DBPOD_MDU_PARITY_ODD;
            }
            else if (!strcasecmp(optarg, "even") || !strcasecmp(optarg, "e"))
            {
                parity = DBPOD_MDU_PARITY_EVEN;
            }
            else
            {
                fprintf(stderr, "Option --parity='%s' invalid\n", optarg);
                err = 1;
            }
            break;
        case 's':   /* --stop-bits=N */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --stop-bits='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp < 1 || tmp > 2)
            {
                fprintf(stderr, "Stop bits %lu out of range\n", tmp);
                err = 1;
            }
            else
            {
                stop_bits = tmp;
            }
            /* stop_bits_specified = 1; */
            break;
        case 't':   /* --timeout=MILLISECONDS */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --timeout='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 10000000)
            {
                fprintf(stderr, "Timeout %lu out of range\n", tmp);
                err = 1;
            }
            else
            {
                timeout = tmp;
                /* timeout_specified = 1; */
            }
            break;
        case OPT_HELP:
            help = 1;
            break;
        default:
            err = 1;
            break;
        }
    }

    if (!help && (argc - optind < 2 || argc - optind > 3))
    {
        err = 1;
    }

    if (err || help)
    {
        fprintf(help ? stdout : stderr,
                "usage: %s [OPTIONS] HOST PORT [MESSAGE]\n"
                "Options:\n"
                " -b, --baud={50|75|110|134|150|200|300|600|1200|1800|2400|4800|9600|19200|\n"
                "             38400|57600|115200|230400}\n"
                " -c, --continuation\n"
                " -d, --data-bits={5|6|7|8}\n"
                " -m, --mdu=MDUPORTNUM\n"
                " -p, --parity={none|n|odd|o|even|e}\n"
                " -s, --stop-bits={1|2}\n"
                " -t, --timeout=MILLISECONDS\n",
                progname);
        exit(help ? EXIT_SUCCESS : 2);
    }

    node = argv[optind];
    service = argv[optind + 1];
    if (argc - optind >= 3)
    {
        mdu_data = argv[optind + 2];
    }
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
