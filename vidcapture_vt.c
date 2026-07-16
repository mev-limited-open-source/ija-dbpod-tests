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

static int standard = DBPOD_VID_STD_DISABLE;
static int interleave = DBPOD_VID_INTERLEAVE_BOTH;
static int frame_rate = 0;
static int frame_size_x = 0;
static int frame_size_y = 0;
static int origin_x = 0;
static int origin_y = 0;
static int crop_x;
static int crop_y;
static int rate_scale = 1;
static int size_scale = 1;
static int agc = 0;
static int led_threshold = 128;
static int dot_threshold = 128;

#define HEADER_LEN (sizeof(DBPOD_MSGHDR) - sizeof(ULONG))
#define DIR_SEND    0
#define DIR_REC     1

#define ENDOF(type, member) \
    (offsetof(type, member) + sizeof(((type *)0)->member))

static void
dump_video_resp_rawgrey8(const DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8 *rsp)
{
    char fname[sizeof("frameXXXXXXXXXX.pgm")];
    FILE *fp;

    printf("nFormat=%d (rawgrey8), nWidth=%d, nHeight=%d,\n",
            rsp->nFormat, rsp->nWidth, rsp->nHeight);
    printf("nBitsPerPixel=%u, bFlags=0x%X (", rsp->nBitsPerPixel, rsp->bFlags);
    if (rsp->bFlags & DBPOD_VID_FLAG_RAW_TO_BE_INTERLACED)
    {
        printf(" INTERLACED");
    }
    if (rsp->bFlags & DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2)
    {
        printf(" TOPLINEFIELD2");
    }
    printf(" )\n");
    if (rsp->nBitsPerPixel != 8)
    {
        printf("*** nBitsPerPixel should be 8\n");
        return;
    }
    if (rsp->nWidth <= 0 || rsp->nHeight <= 0)
    {
        printf("*** Bad frame dimensions\n");
        return;
    }
    if (rsp->hdr.dwLength < offsetof(DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8,
                bPixels[0]) - sizeof(ULONG) +
            ((size_t)rsp->nWidth * rsp->nHeight) * sizeof(rsp->bPixels[0]))
    {
        printf("*** Video data too short\n");
        return;
    }
    snprintf(fname, sizeof(fname), "frame%010u.pgm", rsp->hdr.dwSequence);
    fp = fopen(fname, "wb");
    if (fp)
    {
        fprintf(fp, "P5 %d %d 255\n", rsp->nWidth, rsp->nHeight);
        if (rsp->bFlags & DBPOD_VID_FLAG_RAW_TO_BE_INTERLACED)
        {
            int row;

            for (row = 0; row < rsp->nHeight; row++)
            {
                int field;
                int irow;

                field = row & 1;
                if ((rsp->bFlags & DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2) != 0)
                {
                    field = !field;
                }
                irow = (field * rsp->nHeight + row) / 2;
                fwrite(rsp->bPixels + (size_t)irow * rsp->nWidth, 1,
                        rsp->nWidth, fp);
            }
        }
        else
        {
            fwrite(rsp->bPixels, 1, (size_t)rsp->nWidth * rsp->nHeight, fp);
        }
        fclose(fp);
    }
}

static void
dump_video_resp_rawgrey8vt(const DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT *rsp)
{
    char fname[sizeof("frameXXXXXXXXXX.pgm")];
    FILE *fp;

    printf("nFormat=%d (rawgrey8vt), nWidth=%d, nHeight=%d,\n",
            rsp->nFormat, rsp->nWidth, rsp->nHeight);
    printf("nBitsPerPixel=%u, bFlags=0x%X (", rsp->nBitsPerPixel, rsp->bFlags);
    if (rsp->bFlags & DBPOD_VID_FLAG_RAW_TO_BE_INTERLACED)
    {
        printf(" INTERLACED");
    }
    if (rsp->bFlags & DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2)
    {
        printf(" TOPLINEFIELD2");
    }
    printf(" ),\n");
    printf("vt.uYPos=%u, vt.uXPos=%u, vt.uAmp=%u (dwVT=0x%X)\n",
           rsp->vt.uYPos, rsp->vt.uXPos, rsp->vt.uAmp, rsp->dwVt);
    if (rsp->nBitsPerPixel != 8)
    {
        printf("*** nBitsPerPixel should be 8\n");
        return;
    }
    if (rsp->nWidth <= 0 || rsp->nHeight <= 0)
    {
        printf("*** Bad frame dimensions\n");
        return;
    }
    if (rsp->hdr.dwLength < offsetof(DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT,
                bPixels[0]) - sizeof(ULONG) +
            ((size_t)rsp->nWidth * rsp->nHeight) * sizeof(rsp->bPixels[0]))
    {
        printf("*** Video data too short\n");
        return;
    }
    snprintf(fname, sizeof(fname), "frame%010u.pgm", rsp->hdr.dwSequence);
    fp = fopen(fname, "wb");
    if (fp)
    {
        fprintf(fp, "P5 %d %d 255\n", rsp->nWidth, rsp->nHeight);
        if (rsp->bFlags & DBPOD_VID_FLAG_RAW_TO_BE_INTERLACED)
        {
            int row;

            for (row = 0; row < rsp->nHeight; row++)
            {
                int field;
                int irow;

                field = row & 1;
                if ((rsp->bFlags & DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2) != 0)
                {
                    field = !field;
                }
                irow = (field * rsp->nHeight + row) / 2;
                fwrite(rsp->bPixels + (size_t)irow * rsp->nWidth, 1,
                        rsp->nWidth, fp);
            }
        }
        else
        {
            fwrite(rsp->bPixels, 1, (size_t)rsp->nWidth * rsp->nHeight, fp);
        }
        fclose(fp);
    }
}

static void dump_video_resp(const DBPOD_MSGHDR *hdr)
{
    const DBPOD_RSPBUF_SYNC_VIDEO_DUMMY *rsp;

    if (hdr->dwLength < offsetof(DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT,
                bData[0]) - sizeof(ULONG))
    {
        printf("*** Crappy message video response len %u\n", hdr->dwLength);
        return;
    }
    rsp = (const DBPOD_RSPBUF_SYNC_VIDEO_DUMMY *)hdr;
    switch (rsp->nFormat)
    {
    case DBPOD_VID_RAWGREY8:
        dump_video_resp_rawgrey8((const DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8 *)hdr);
        break;
    case DBPOD_VID_RAWGREY8VT:
        dump_video_resp_rawgrey8vt((const DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT *)hdr);
        break;
    default:
        printf("nFormat=%d (unsupported), nWidth=%d, nHeight=%d\n",
                rsp->nFormat, rsp->nWidth, rsp->nHeight);
        break;
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
    case DBPOD_MSGCODE_ASYNC_VIDEO:
    case DBPOD_RSPCODE_SYNC_VIDEO:
        dump_video_resp(hdr);
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

static int do_video_config(void)
{
    DBPOD_CMDBUF_VIDEO_CONFIG cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.dwLength = sizeof(cmd) - sizeof(ULONG);
    cmd.hdr.dwSequence = cmdseq++;
    cmd.hdr.wCmd = DBPOD_CMDCODE_VIDEO_CONFIG;
    cmd.nStandard = standard;
    cmd.nInterleave = interleave;
    cmd.nTrkFrameRate = frame_rate;
    cmd.nXTrkFrameSize = frame_size_x;
    cmd.nYTrkFrameSize = frame_size_y;
    cmd.nXTrkOrigin = origin_x;
    cmd.nYTrkOrigin = origin_y;
    cmd.nXTrkCropSize = crop_x;
    cmd.nYTrkCropSize = crop_y;
    cmd.nStrmSizeScale = size_scale;
    cmd.nStrmRateScale = rate_scale;
    cmd.fAGC = agc;
    cmd.nAGCThreshold = led_threshold;
    cmd.nFormat = DBPOD_VID_RAWGREY8VT;
    cmd.nNoDotThreshold = dot_threshold;
    return do_cmd(&cmd.hdr);
}

static int do_start_vc(void)
{
    DBPOD_CMDBUF_START_VC cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.dwLength = sizeof(cmd) - sizeof(ULONG);
    cmd.hdr.dwSequence = cmdseq++;
    cmd.hdr.wCmd = DBPOD_CMDCODE_START_VC;
    return do_cmd(&cmd.hdr);
}

static int do_start_async_video(void)
{
    DBPOD_CMDBUF_START_VC cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.dwLength = sizeof(cmd) - sizeof(ULONG);
    cmd.hdr.dwSequence = cmdseq++;
    cmd.hdr.wCmd = DBPOD_CMDCODE_START_ASYNC_VIDEO;
    return do_cmd(&cmd.hdr);
}

static int do_test(void)
{
    int err;

    err = do_video_config();
    if (err < 0)
    {
        return err;
    }
    err = do_start_vc();
    if (err < 0)
    {
        return err;
    }
    err = do_start_async_video();
    if (err < 0)
    {
        return err;
    }
    return get_all_messages();
}

int main(int argc, char *argv[])
{
    static const struct option longopts[] =
    {
        { "standard", required_argument, NULL, 's' },
        { "interleave", required_argument, NULL, 'i' },
        { "frame-rate", required_argument, NULL, 'r' },
        { "frame-size", required_argument, NULL, 'f' },
        { "origin", required_argument, NULL, 'o' },
        { "crop-size", required_argument, NULL, 'c' },
        { "rate-scale", required_argument, NULL, 'R' },
        { "size-scale", required_argument, NULL, 'S' },
        { "agc", no_argument, NULL, 'a' },
        { "led-threshold", required_argument, NULL, 'l' },
        { "dot-threshold", required_argument, NULL, 'd' },
        { 0 },
    };
    static const char opts[] = "s:i:r:f:o:c:R:S:al:d:";
    const char *node, *service;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int err = 0;
    int frame_rate_specified = 0;
    int frame_size_specified = 0;
    int crop_specified = 0;

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
        case 's':   /* --standard=enum */
            if (!strcasecmp(optarg, "pal"))
            {
                standard = DBPOD_VID_STD_PAL;
            }
            else if (!strcasecmp(optarg, "ntsc"))
            {
                standard = DBPOD_VID_STD_NTSC;
            }
            else
            {
                fprintf(stderr, "Option --standard='%s' invalid\n", optarg);
                err = 1;
            }
            break;
        case 'i':   /* --interleave=enum */
            if (!strcasecmp(optarg, "odd"))
            {
                interleave = DBPOD_VID_INTERLEAVE_ODD;
            }
            else if (!strcasecmp(optarg, "even"))
            {
                interleave = DBPOD_VID_INTERLEAVE_EVEN;
            }
            else if (!strcasecmp(optarg, "both"))
            {
                interleave = DBPOD_VID_INTERLEAVE_BOTH;
            }
            else
            {
                fprintf(stderr, "Option --interleave='%s' invalid\n", optarg);
                err = 1;
            }
            break;
        case 'r':   /* --frame-rate=n */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --frame-rate='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 32767)
            {
                fprintf(stderr, "Frame rate %lu out of range\n", tmp);
                err = 1;
            }
            else
            {
                frame_rate = (int)tmp;
                frame_rate_specified = 1;
            }
            break;
        case 'f':   /* --frame-size=W,H */
            errno = 0;
            tmperr = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end != ',')
            {
                tmperr = 1;
            }
            else
            {
                t = end + 1;
                tmp2 = strtoul(t, &end, 10);
                if (errno || !isdigit(*t) || *end)
                {
                    tmperr = 1;
                }
            }
            if (tmperr)
            {
                fprintf(stderr, "Option --frame-size='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 32767 || tmp2 > 32767)
            {
                fprintf(stderr, "Frame size %lu,%lu out of range\n", tmp, tmp2);
                err = 1;
            }
            else
            {
                frame_size_x = (int)tmp;
                frame_size_y = (int)tmp2;
                frame_size_specified = 1;
            }
            break;
        case 'o':   /* --origin=X,Y */
            errno = 0;
            tmperr = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end != ',')
            {
                tmperr = 1;
            }
            else
            {
                t = end + 1;
                tmp2 = strtoul(t, &end, 10);
                if (errno || !isdigit(*t) || *end)
                {
                    tmperr = 1;
                }
            }
            if (tmperr)
            {
                fprintf(stderr, "Option --origin='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 32767 || tmp2 > 32767)
            {
                fprintf(stderr, "Origin %lu,%lu out of range\n", tmp, tmp2);
                err = 1;
            }
            else
            {
                origin_x = (int)tmp;
                origin_y = (int)tmp2;
                /* origin_specified = 1; */
            }
            break;
        case 'c':   /* --crop-size=W,H */
            errno = 0;
            tmperr = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end != ',')
            {
                tmperr = 1;
            }
            else
            {
                t = end + 1;
                tmp2 = strtoul(t, &end, 10);
                if (errno || !isdigit(*t) || *end)
                {
                    tmperr = 1;
                }
            }
            if (tmperr)
            {
                fprintf(stderr, "Option --crop-size='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 32767 || tmp2 > 32767)
            {
                fprintf(stderr, "Crop size %lu,%lu out of range\n", tmp, tmp2);
                err = 1;
            }
            else
            {
                crop_x = (int)tmp;
                crop_y = (int)tmp2;
                crop_specified = 1;
            }
            break;
        case 'R':   /* --rate-scale=n */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --rate-scale='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 32767)
            {
                fprintf(stderr, "Rate scale %lu out of range\n", tmp);
                err = 1;
            }
            else
            {
                rate_scale = (int)tmp;
                /* rate_scale_specified = 1; */
            }
            break;
        case 'S':   /* --size-scale=n */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --size-scale='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 32767)
            {
                fprintf(stderr, "Size scale %lu out of range\n", tmp);
                err = 1;
            }
            else
            {
                size_scale = (int)tmp;
                /* size_scale_specified = 1; */
            }
            break;
        case 'a':   /* --agc */
            agc = 1;
            break;
        case 'l':   /* --led-threshold=n */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --led-threshold='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 32767)
            {
                fprintf(stderr, "LED threshold %lu out of range\n", tmp);
                err = 1;
            }
            else
            {
                led_threshold = (int)tmp;
                /* led_threshold_specified = 1; */
            }
            break;
        case 'd':   /* --dot-threshold=n */
            errno = 0;
            tmp = strtoul(optarg, &end, 10);
            if (errno || !isdigit(*optarg) || *end)
            {
                fprintf(stderr, "Option --dot-threshold='%s' invalid\n", optarg);
                err = 1;
            }
            else if (tmp > 32767)
            {
                fprintf(stderr, "Dot threshold %lu out of range\n", tmp);
                err = 1;
            }
            else
            {
                dot_threshold = (int)tmp;
                /* dot_threshold_specified = 1; */
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
                " -s, --standard=pal|ntsc\n"
                " -i, --interleave=odd|even|both\n"
                " -r, --frame-rate=N\n"
                " -f, --frame-size=W,H\n"
                " -o, --origin=X,Y\n"
                " -c, --crop-size=W,H\n"
                " -R, --rate-scale=N\n"
                " -S, --size-scale=N\n"
                " -a, --agc\n"
                " -l, --led-threshold=N\n"
                " -d, --dot-threshold=N\n",
                progname);
        return 2;
    }

    switch (standard)
    {
    case DBPOD_VID_STD_PAL:
        if (!frame_rate_specified)
        {
            frame_rate = 25;
        }
        if (!frame_size_specified)
        {
            frame_size_x = 768;
            frame_size_y = 576;
        }
        break;
    case DBPOD_VID_STD_NTSC:
        if (!frame_rate_specified)
        {
            frame_rate = 30;
        }
        if (!frame_size_specified)
        {
            frame_size_x = 640;
            frame_size_y = 480;
        }
        break;
    default:
        break;
    }
    if (!crop_specified)
    {
        crop_x = frame_size_x - origin_x;
        if (crop_x < 0)
        {
            crop_x = 0;
        }
        crop_y = frame_size_y - origin_y;
        if (crop_y < 0)
        {
            crop_y = 0;
        }
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
