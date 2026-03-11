#ifndef PAUTPOD_IOCTL_H__INCLUDED
#define PAUTPOD_IOCTL_H__INCLUDED

/*
 * $Id$
 *
 * db-Pod ioctl definitions
 *
 * Copyright (c) 2008 MEV Ltd., Bell Technology Ltd.
 * All rights reserved.
 *
 * MODULE CONTENTS
 *
 * dbpod_ioctl.h
 *
 * DESCRIPTION
 *
 * ioctls for db-Pod driver.
 *
 * AMENDMENT RECORD
 *
 * $Log$
 *
 */

#include <pautpod/pautpod_wtypes.h>
#include <pautpod/pautpod_coms.h>
#include <linux/ioctl.h>

#if defined(__cplusplus)
extern "C" {
#endif
#if 0
}
#endif

/* Message types in the data buffer. */
#define DBPOD_MTYPE_UT      0xDEADBEEF  /* UT message. */
#define DBPOD_MTYPE_VIDEO   0xDECEA5ED  /* Video capture message. */
#define DBPOD_MTYPE_DUMMY   0xBABEFACE  /* Dummy message. */
#define DBPOD_MTYPE_BAD     0xBADBABE5  /* Bad message (shouldn't see this). */

#define DBPOD_IOCTL_MAGIC   0xDB

/*
 * The "command" ioctl is used to pass a "command" message buffer to the
 * driver and get a "response" message back.
 *
 * The ioctl argument points to a 'struct dbpod_command_response' which
 * contains pointers to the command and response message buffers.
 *
 * The message buffers include a non-inclusive length at the start.
 *
 * The maximum length of the response should be filled in by the caller before
 * calling the ioctl.
 *
 * Not all db-Pod messages are handled by the "command" ioctl mechanism.
 * Those involving external hardware not controlled by the driver (e.g.  the
 * MDU) and those performing UT or video data transfers are not handled by the
 * "command" ioctl.
 */
#define DBPOD_IOCODE_COMMAND            1
#define DBPOD_IOCTL_COMMAND \
    _IOW(DBPOD_IOCTL_MAGIC, DBPOD_IOCODE_COMMAND, struct dbpod_command_response)

struct dbpod_command_response {
    const DBPOD_MSGHDR *cmd;    /* Pointer to command buffer. */
    DBPOD_MSGHDR *rsp;          /* Pointer to response buffer. */
};

/*
 * The "check buffer" ioctl is used to check the buffer size, amount of data
 * in the buffer, the current position in the buffer, the current poll
 * position in the buffer, and the number of buffer resets.  The poll position
 * may be advanced past the current position and is the position used by the
 * select/poll mechanism.
 *
 * The ioctl argument points to a 'struct dbpod_bufpos'.
 *
 * The buffer is cyclic and can be mmap'ed.  The positions in 'struct
 * dbpos_bufpos' are relative to the start of the mmap'ed buffer.
 *
 * On success, the 'struct dbpod_bufpos' is filled in with current
 * information.
 *
 * Positions below are shown with no wraparound of the buffer:
 *
 *                           curpos+
 *  0    curpos  pollpos     bufused   bufsize
 *  |      |       |            |      |
 *  ===================================
 *         |------->
 *          pollused
 *         |-------------------->
 *               bufused
 *
 * Positions below are shown with curpos+bufused wrapped around:
 *
 *      curpos+
 *      bufused-
 *  0   bufsize        curpos  pollpos bufsize
 *  |      |             |       |     |
 *  ===================================
 *                       |------->
 *                        pollused
 *  ------->             |-------------
 *  curpos+                 bufsize
 *  bufused-                -curpos
 *  bufsize
 *
 * Note that various operations will reset the buffer positions and the amount
 * of buffer used to 0.  These operations include commands to abort transfers
 * or perform a soft reset.  When this happens, the 'numresets' counter is
 * incremented.
 */
#define DBPOD_IOCODE_CHECK_BUFFER       2
#define DBPOD_IOCTL_CHECK_BUFFER \
    _IOR(DBPOD_IOCTL_MAGIC, DBPOD_IOCODE_CHECK_BUFFER, struct dbpod_bufpos)

struct dbpod_bufpos {
    ULONG bufsize;      /* Size of buffer. */
    ULONG curpos;       /* Current position. */
    ULONG bufused;      /* Amount of buffer in use. */
    ULONG pollpos;      /* Current poll position. */
    ULONG pollused;     /* Amount from current position to poll position. */
    ULONG numresets;    /* Number of times buffer has been reset. */
};

/*
 * The "advance buffer" ioctl is used to advance the current position in the
 * buffer (reducing the amount of buffer in use).
 *
 * The ioctl argument points to a 'struct dbpod_bufpos' containing the new
 * current position.
 *
 * The amount of advancement is determined by comparing the specified current
 * position with the old current position.  If the position is unchanged, this
 * is interpreted as "no advance" (advance by 0).
 *
 * Attempts to advance the current position beyond the amount of buffer
 * currently in use will be rejected.
 *
 * If the current position is advanced beyond the poll position, the poll
 * position will be set to the new current position.
 *
 * On success, the current position and possibly the poll position are updated
 * and other other parts of the 'struct bufpos' are updated with current
 * information as for the "check buffer" ioctl.
 */
#define DBPOD_IOCODE_ADVANCE_BUFFER       3
#define DBPOD_IOCTL_ADVANCE_BUFFER \
    _IOWR(DBPOD_IOCTL_MAGIC, DBPOD_IOCODE_ADVANCE_BUFFER, struct dbpod_bufpos)

/*
 * The "advance poll" ioctl is used to advance the poll position in the
 * buffer.
 *
 * The ioctl argument points to a 'struct dbpod_bufpos' containing the new
 * poll position.
 *
 * The amount of advancement is determined by comparing the specified poll
 * position with the old poll position.  If the position is unchanged, this is
 * interpreted as "no advance" (advance by 0).
 *
 * Attempts to advance the poll position beyond the amount of buffer currently
 * in use will be rejected.
 *
 * Advancing the poll position does not affect the main current buffer
 * position or reduce the amount of buffer in use, but does affect subsequent
 * select/poll operations.  It's main purpose is to use the select/poll
 * mechanism to be informed of future additions to the buffer while holding on
 * to old data in the buffer.
 *
 * On success, the poll position is updated and other parts of the 'struct
 * bufpos' are updated with current information as for the "check buffer"
 * ioctl.
 */
#define DBPOD_IOCODE_ADVANCE_POLL       4
#define DBPOD_IOCTL_ADVANCE_POLL \
    _IOWR(DBPOD_IOCTL_MAGIC, DBPOD_IOCODE_ADVANCE_POLL, struct dbpod_bufpos)

/*
 * The "set calibration" ioctl is used to set the hardware calibration values.
 *
 * The ioctl argument points to a 'struct dbpod_calibration' containing the
 * new calibration values.
 */
#define DBPOD_IOCODE_SET_CALIBRATION    5
#define DBPOD_IOCTL_SET_CALIBRATION     _IOW(DBPOD_IOCTL_MAGIC, \
        DBPOD_IOCODE_SET_CALIBRATION, struct dbpod_calibration)

struct dbpod_calibration {
    SHORT null_offset;      /* null offset corrections */
    SHORT gain_ref_2;       /* gain stage 2 reference tweak */
    SHORT gain_ref_3;       /* gain stage 3 reference tweak */
    SHORT gain_mult;        /* DAC gain correction multiplier * 1000 */
    SHORT gain_offset;      /* DAC gain offset in milliBels */
};

/*
 * The "get calibration" ioctl is used to get the current hardware calibration
 * values.
 *
 * The ioctl argument points to a 'struct dbpod_calibration' which will be
 * filled in with the current calibration values.
 */
#define DBPOD_IOCODE_GET_CALIBRATION    6
#define DBPOD_IOCTL_GET_CALIBRATION     _IOR(DBPOD_IOCTL_MAGIC, \
        DBPOD_IOCODE_GET_CALIBRATION, struct dbpod_calibration)

/*
 * The "get video preamble" ioctl is used to get the current video message
 * preamble.  This can be called following a successful video configuration
 * command to get the data to be prepended to the front of the raw data in
 * acquired video messages.
 * 
 *
 * The ioctl argument points to a 'DBPOD_RSPBUF_SYNC_VIDEO_DUMMY' extended
 * to the maximum expected preamble length.  The 'dwLength' field at the start
 * of the header indicates the maximum expected preamble length, not including
 * the length of the 'dwLength' field itself.  On success, the ioctl will fill
 * in the DBPOD_RSPBUF_SYNC_VIDEO_DUMMY and any extended data following, and
 * set the 'dwLength' field to the length of the preamble, not including the
 * 'dwLength field itself.  The length will depend on 'nFormat', the video
 * block format.
 *
 * For example, if 'nFormat' is 'DBPOD_VID_RAWGREY8, the buffer will be filled
 * in as a 'DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8' (the initial part of which
 * matches 'DBPOD_RSPBUF_SYNC_VIDEO_DUMMY') the the 'dwLength' in the header
 * will be set to 'BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8' minus
 * 'sizeof(ULONG)'.
 *
 * Note that the ioctl buffer is variable length and the length encoded in
 * the IOCTL code is only the minimum length.
 */
#define DBPOD_IOCODE_GET_VIDEO_PREAMBLE 7
#define DBPOD_IOCTL_GET_VIDEO_PREAMBLE  _IOWR(DBPOD_IOCTL_MAGIC, \
        DBPOD_IOCODE_GET_VIDEO_PREAMBLE, DBPOD_RSPBUF_SYNC_VIDEO_DUMMY)

#if 0
{
#endif
#if defined(__cplusplus)
}
#endif

/* vi: set ai et sw=4 ts=4: */
#endif
