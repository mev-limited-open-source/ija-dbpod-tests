#ifndef DBPOD_COMS_H__INCLUDED
#define DBPOD_COMS_H__INCLUDED

/*
 * $Id$
 *
 * db-Pod Communications Messages
 *
 * Copyright (c) 2008-2024 MEV Ltd., Bell Technology Ltd.
 * All rights reserved.
 *
 * MODULE CONTENTS
 *
 * dbpod_coms.h
 *
 * DESCRIPTION
 *
 * Communications messages structure for db-Pod.
 *
 * AMENDMENT RECORD
 *
 * $Log$
 *
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * General format info:
 *
 * 1. Multi-byte numbers are transmitted in "little-endian" order.
 *
 * 2. Structures are defined to keep the alignment of elementary structure
 *    members the same as their size.
 *
 * 3. Integer types are 2's complement.
 *
 * 4. The elementary types used are as follows:
 *
 *      CHAR   - plain 8-bit character
 *      INT8   - signed 8-bit integer
 *      UCHAR  - unsigned 8-bit integer
 *      SHORT  - signed 16-bit integer
 *      USHORT - unsigned 16-bit integer
 *      LONG   - signed 32-bit integer
 *      ULONG  - unsigned 32-bit integer
 *
 * 5. The 'FILETIME' type is defined elsewhere as follows:
 *
 *      typedef struct _FILETIME
 *      {
 *          DWORD dwLowDateTime;
 *          DWORD dwHighDateTime;
 *      };
 *
 *    where 'DWORD' is also a 32-bit integer.  Together, 'dwLowDateTime' and
 *    'dwHighDateTime' form the lower and upper 32-bits of a 64-bit unsigned
 *    value representing the number of 100 ns intervals since January 1, 1601
 *    (UTC).
 *
 * 6. The 'FIELD_OFFSET()' macro (defined elsewhere) is similar to the C
 *    standard 'offsetof()' macro except that the result has type 'LONG'.
 *
 * 7. All padding fields in command and response messages are reserved for
 *    future use and should be set to 0.
 */

/* Pack on 4-byte boundaries */
/* Probably redundant, as we keep our structure members "naturally aligned". */
#if defined(_MSC_VER) || defined(__GNUC__)
//#pragma pack(push, 4)
#endif /* _MSC_VER */

/* Remember current setting & disable warning message for 0-length array members */
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4200)
#endif /* _MSC_VER */

/* Define minimum array length. */
#if defined(_MSC_VER) || defined(__GNUC__)
/* Zero-length array for Microsoft C or GNU C. */
#define DBPOD_ANYLENGTH 0
#else
/* Zero-length array is non-standard, so use 1. */
#define DBPOD_ANYLENGTH 1
#endif

/*
 * Current packet version.
 * Upper 16 bits is major version, lower 16 bits is minor version.
 *
 * =====================================================================
 * Various incompatible changes occurred during development in packet
 * version 0x30000.
 *
 * =====================================================================
 * Packet version 0x40000
 * ----------------------
 *
 * 1.  DBPOD_CMDBUF_MDU_CONFIG has the 'UCHAR nPort' field replacing
 *     the unused 'CHAR pad1' field.
 *
 * 2.  DBPOD_CMDBUF_MDU_DATA has the 'USHORT wLength', 'UCHAR nPort'
 *     and 'CHAR fContinuation' fields replacing the 'ULONG dwLength'
 *     field.
 *
 * =====================================================================
 * Packet version 0x40001
 * ----------------------
 *
 * 1. DBPOD_RSPBUF_GET_CAPABILITIES has the 'USHORT nChanConfigs' field
 *    replacing the unused 'SHORT pad2' field.  This holds the number
 *    of channel configurations supported by the pod.
 *
 * =====================================================================
 * Packet version 0x40002
 * ----------------------
 *
 * 1.  DBPOD_RSPBUF_GET_CAPABILITIES has the following changes which
 *     should be backwards compatible:
 *
 *     (a) The 'dwRFFScaleFreq' field has been renamed to
 *         'dwFiltScaleFreq'.  It now either applies to, or does not
 *         apply to, the LPF, HPF and RFF filter breakpoints, depending
 *         on the other fields defined below.
 *
 *     (b) The 'LONG fVideoTracking' field has been split into
 *         'CHAR fVideoTracking', 'CHAR fScaleLPF', 'CHAR fScaleHPF',
 *         and 'CHAR fScaleRFF' fields.  'fVideoTracking' was previously
 *         set to TRUE.
 *
 *         If 'fScaleLPF' is FALSE, the low-pass filter breakpoint
 *         frequencies in 'anLPF[]' are absolute (except that a value
 *         of 0 means "broadband").  If 'fScaleLPF' is TRUE, the values
 *         in 'anLPF[]' need to be multiplied by the digitization
 *         frequency ('nSampleFreq' in the channel configuration) and
 *         divided by the 'dwFiltScaleFreq' value.
 *
 *         If 'fScaleHPF' is FALSE, the high-pass filter breakpoint
 *         frequencies in 'anHPF[]' are absolute.  If 'fScaleHPF' is
 *         TRUE, the values in 'anHPF[]' need to be multiplied by the
 *         digitization frequency ('nSampleFreq' in the channel
 *         configuration) and divided by the 'dwFiltScaleFreq' value.
 *
 *         If 'fScaleRFF' is FALSE, the post-rectification filter
 *         breakpoint frequencies in 'anRFF[]' are absolute.  If
 *         'fScaleHPF' is TRUE, the values in 'anRFF[]' need to be
 *         multiplied by the digitization frequency ('nSampleFreq' in
 *         the channel configuration) and divided by the
 *         'dwFiltScaleFreq' value.
 *
 *         NOTE: See changes in packet version 0x50000 that affect how
 *         'dwFiltScaleFreq' is used.
 *
 * 2.  DBPOD_CMDBUF_CHAN_CONFIG has the 'USHORT wCompatLevel' field
 *     replacing the 'SHORT pad2' field.
 *
 *     If 'wCompatLevel' is 0 (old application), the 'nHPF', 'nLPF',
 *     and 'nRFF' values will be matched against supported values
 *     listed in the capabilities as-is, with no scaling, regardless of
 *     the 'fScaleHPF', 'fScaleLPF' and 'fScaleRFF' flags in the
 *     capabilities.  This may result in the filter cut-off frequencies
 *     that are actually applied being different from the specified
 *     frequencies.
 *
 *     If 'wCompatLevel' is 1 (new application), the 'nHPF', 'nLPF',
 *     and 'nRFF' values will be matched against unscaled or scaled
 *     (depending on the 'fScaleHPF', 'fScaleLPF', and 'fScaleRFF'
 *     settings in the capabilities) versions of the supported values
 *     listed in the capabilities.
 *
 *     NOTE: See changes in packet version 0x50000 for additional
 *     'wCompatLevel' value.
 *
 * =====================================================================
 * Packet version 0x40003
 * ----------------------
 *
 * 1.  DBPOD_CMDBUF_CHAN_CONFIG has the 'USHORT wSpecial' field
 *     replacing the 'SHORT pad3' field.  This is for setting a
 *     hardware-specific configuration value, e.g. for setting input
 *     impedance and pre-amplification.
 *
 * 2.  DBPOD_RSPBUF_GET_CAPABILITIES has the 'UCHAR bHardType' and
 *     'UCHAR bAFEType' fields replacing the 'SHORT pad3' field:
 *
 *     bHardType values:
 *
 *       0 (DBPOD_HARDTYPE_ORIG_ETH) = Classic Ethernet Pod (Nios II).
 *       1 (DBPOD_HARDTYPE_FAST_ETH) = "Fast" Ethernet Pod (ARM).
 *       2 (DBPOD_HARDTYPE_MINI_ETH) = Ethernet Mini Pod (ARM).
 *       3 (DBPOD_HARDTYPE_MINI_USB) = USB3.0 Mini Pod (FTDI).
 *
 *     bAFEType values:
 *
 *       0 (DBPOD_AFETYPE_ORIGINAL)  = Classic Ethernet Pod or Fast Pod.
 *       1 (DBPOD_AFETYPE_MAX2077)   = MAX2077 - MiniPod (Ether or USB).
 *
 * =====================================================================
 * Packet version 0x40004
 * ----------------------
 *
 * Packet version 0x40004 has changes to the encoder configuration
 * command for configuring external resets, and supports a "maximum"
 * mode for the average gate, a "sorted peaks" mode for the peaks gate,
 * and a new scan type for use with "maximum" mode.  There is also an
 * option for including readings from all four quadrature encoders in
 * the UT data, replacing some of the peaks in the message.
 *
 * 1.  DBPOD_CMDBUF_ENCODER_CONFIG has the 'USHORT wDifferential',
 *     'USHORT wResetEnable', 'USHORT wInvertSense', and 'USHORT
 *     wResetSource' fields replacing the 'ULONG dwDifferential' and
 *     'ULONG dwInvertSense' fields, adn is the same size as before.
 *
 * 2.  For channel configuration commands, the "averaging" gate supports
 *     a new "maximum" mode.  This produces the extreme values (those
 *     with largest absolute value) at each sample position from a set
 *     of passes instead of the usual "mean" value.  The "maximum" mode
 *     is enabled by setting the 'nControl' field of the averaging gate
 *     to DBPOD_AVERAGING_MAX.
 *
 *     Averaging gate nControl values:
 *
 *       0 (DBPOD_AVERAGING_MEAN) - produces the mean values
 *       1 (DBPOD_AVERAGING_MAX)  - (new) produces the maximum (extreme)
 *                                  values
 *
 *     "Maximum" mode works for Ethernet MiniPod FPGA version 7 or later
 *     (as reported by the 'wHardVersion' fields of the 'Get
 *     Capabilities' response) and for all(?) versions of the USB
 *     MiniPod.
 *
 * 3.  Also for channel configuration commands, the "peaks" gate
 *     supports a new "sorted peaks" mode.  This sorts the peaks in
 *     descending order of size.  The "sorted peaks" mode is enabled by
 *     setting the peak type in bits 15 to 8 of the 'nControl' field of
 *     the peaks gate to DBPOD_PEAKTYPE_SORTED.  Note that bits 7 to 0
 *     of the 'nControl' field sets the number of peaks required, but
 *     this is currently ignored.
 *
 *     Peak gate nControl values:
 *
 *       DBPOD_PEAKS_NCONTROL(num, type)
 *
 *         num  - number of peaks (currently ignored)
 *         type - peak type
 *
 *         Peak type values (bits 15 to 8 of nControl):
 *
 *           0 (DBPOD_PEAKTYPE_FIRST_N_POS) - first n positive peaks
 *           1 (DBPOD_PEAKTYPE_FIRST_N)     - first n bipolar peaks
 *           2 (DBPOD_PEAKTYPE_SORTED)      - (new) sorted peaks
 *
 *         (Note that current hardware always produces bipolar peaks
 *         and the "number of peaks" value is ignored.)
 *
 *     "Sorted peaks" mode works for Ethernet MiniPod FPGA version 7 or
 *     later (as reported by the 'wHardVersion' field of the 'Get
 *     Capabilities' response) and for all(?) versions of the USB
 *     MiniPod.
 *
 * 4.  The 'ScanType' field of DBPOD_CMDBUF_SCAN_CONFIG supports an
 *     additional value, DBPOD_SCAN_FREEPOSITION.  This is like free-run
 *     mode, but changes in scan position may have additional effects.
 *     This is for use with the new "maximum" UT mode, where it results
 *     in production of maximised UT data from all sweeps since the
 *     previous change in scan position.
 *
 *     Scan configuration 'ScanType' values:
 *
 *       0 (DBPOD_SCAN_FREERUN)        - free-run mode
 *       1 (DBPOD_SCAN_TIEDTOPOSITION) - tied-to-position mode
 *       2 (DBPOD_SCAN_FREEPOSITION)   - (new) "free position" mode
 *
 *     "Free position" mode works for Ethernet MiniPod FPGA version 7 or
 *     later (as reported by the 'wHardVersion' field of the 'Get
 *     Capabilities' response) and for all(?) versions of the USB
 *     MiniPod.
 *
 * 5.  The 'dwFlags' field of DBPOD_CMDBUF_START_UT has an additional
 *     bit-mask flag value DBPOD_START_UT_EXTRA_ENCS.  OR-ing
 *     DBPOD_START_UT_EXTRA into 'dwFlags' has the effect of making
 *     encoder readings beyond encoders 0 and 1 appearing at the end of
 *     the UT data in place of some of the peaks.
 *
 *     'dwFlags' bit-mask values:
 *
 *       0x0001 (DBPOD_START_UT_DESPARKLE)  - turn on desparkler
 *       0x0002 (DBPOD_START_UT_COMPRESS)   - turn on compression
 *       0x0004 (DBPOD_START_UT_EXTRA_ENCS) - (new) extra encoders in UT
 *
 *     (Note: the "compress" option is reserved for future use.  The
 *     "desparkle" option is a legacy option which may have no effect on
 *     current hardware.)
 *
 *     This works for Ethernet MiniPod FPGA version 7 or later (as
 *     reported by the 'wHardVersion' field of the 'Get Capabilities'
 *     response) and for all(?) versions of the USB MiniPod.
 *
 * =====================================================================
 * Packet version 0x50000
 * ----------------------
 *
 * 1.  DBPOD_RSPBUF_GET_CAPABILITIES has the 'UCHAR bPAType' field
 *     replacing the first half of the 'SHORT pad4' field, which is now
 *     'UCHAR pad4'. There is also an additional value 4 for the
 *     'bHardType' field, and additional value 2 for the 'bAFEType
 *     field:
 *
 *     bHardType values:
 *
 *       0 (DBPOD_HARDTYPE_ORIG_ETH)   = Classic Ethernet Pod (Nios II).
 *       1 (DBPOD_HARDTYPE_FAST_ETH)   = "Fast" Ethernet Pod (ARM).
 *       2 (DBPOD_HARDTYPE_MINI_ETH)   = Ethernet Mini Pod (ARM).
 *       3 (DBPOD_HARDTYPE_MINI_USB)   = USB3.0 Mini Pod (FTDI).
 *       4 (DBPOD_HARDTYPE_MINIPA_ETH) = Ethernet Mini Phased Array
 *                                       (ARM).
 *
 *     bAFEType values:
 *
 *       0 (DBPOD_AFETYPE_ORIGINAL)  = Classic Ethernet Pod or Fast Pod.
 *       1 (DBPOD_AFETYPE_MAX2077)   = MAX2077 - Mini Pod (Ethernet or
 *                                     USB).
 *       2 (DBPOD_AFETYPE_VCA5807)   = VCA5807 - Mini Phased Array.
 *
 *     bPAType values:
 *
 *       0 (DBPOD_PATYPE_NONE)     = No phased array.
 *       1 (DBPOD_PATYPE_MINI_PBF) = MiniPod Phased Array partial beam
 *                                   former.
 *
 * 2.  There is an extended version of the 'Get Capabilities' command
 *     using the same command code, but including a 'ULONG
 *     dwMaxCapLength' field.  If this field is included in the 'Get
 *     Capabilities' command, it specifies the maximum length of the
 *     'Get Capabilities' response, not including the message header
 *     (i.e., it specifies the maximum length from the start of the
 *     'szHwName' field onwards in DBPOD_RSPBUF_GET_CAPABILITIES).
 *     However, 'dwMaxCapLength' values less than 512 will be increased
 *     to 512 for backwards compatibility.
 *
 *     All capability fields from 'szHwName' to 'dwMaxPRF' inclusive
 *     (corresponding to a 'dwMaxCapLength' value of 512) will be
 *     included in the 'Get Capabilities' response regardless of the
 *     presence of the 'dwMaxCapLength' field or its value.  Higher
 *     values of the 'dwMaxCapLength' field allow additional capability
 *     fields to be included in the response.
 *
 * 3.  The extended 'Get Capabilities' response includes the following
 *     additional fields if the 'dwMaxCapLength' value in the command
 *     is high enough:
 *
 *     (a) 'USHORT wScaleMaskLPF' is a bit-mask indicating which of the
 *         low-pass filter breakpoint frequencies in 'anLPF[]' are
 *         scaled and which are absolute.  This is more informative
 *         than the 'fScaleLPF' field.  A '1' bit at a bit-position in
 *         the mask indicates that the entry at the corresponding index
 *         in 'anLPF[]' needs to be multiplied by the digitization
 *         frequency ('nSampleFreq' in the channel configuration) and
 *         divided by the 'dwFiltScaleFreq' value.  A '0' bit at a
 *         bit-position in the mask indicates that the entry at the
 *         corresponding index in 'anLPF[]' is absolute (except that a
 *         value of 0 means "broadband").  The value is only taken into
 *         account during channel configuration if the 'wCompatLevel'
 *         value in the channel configuration is at least 2.
 *
 *     (b) 'USHORT wScaleMaskHPF' is a bit-mask indicating which of the
 *         high-pass filter breakpoint frequencies in 'anHPF[]' are
 *         scaled and which are absolute.  This is more informative
 *         than the 'fScaleHPF' field.  A '1' bit at a bit-position in
 *         the mask indicates that the entry at the corresponding index
 *         in 'anHPF[]' needs to be multiplied by the digitization
 *         frequency ('nSampleFreq' in the channel configuration) and
 *         divided by the 'dwFiltScaleFreq' value.  A '0' bit at a
 *         bit-position in the mask indicates that the entry at the
 *         corresponding index in 'anHPF[]' is absolute.  The value is
 *         only taken into account during channel configuration if the
 *         'wCompatLevel' value in the channel configuration is at least
 *         2.
 *
 *     (c) 'USHORT wScaleMaskRFF' is a bit-mask indicating which of the
 *         post-rectification filter breakpoint frequencies in 'anRFF[]'
 *         are scaled and which are absolute.  This is more informative
 *         than the 'fScaleRFF' field.  A '1' bit at a bit-position in
 *         the mask indicates that the entry at the corresponding index
 *         in 'anRFF[]' needs to be multiplied by the digitization
 *         frequency ('nSampleFreq' in the channel configuration) and
 *         divided by the 'dwFiltScaleFreq' value.  A '0' bit at a
 *         bit-position in the mask indicates that the entry at the
 *         corresponding index in 'anRFF[]' is absolute.  The value is
 *         only taken into account during channel configuration if the
 *         'wCompatLevel' value in the channel configuration is at least
 *         2.
 *
 *     (d) 'USHORT wMaxAperture' is the maximum number of parallel ADCs
 *         for phased array.
 *
 *     (e) 'USHORT wMaxElements' is the maximum number of phased array
 *         elements.
 *
 *     (f) 'USHORT wTxFocusGranularity' is the TX focus delay
 *         granularity (TX focus step size) in nanoseconds for phased
 *         array.
 *
 *     (g) 'USHORT wRxFocusGranularity' is the RX focus delay
 *         granularity (RX focus step size) in nanoseconds for phased
 *         array.
 *
 *     (h) 'USHORT wMaxFocusSteps' is the maximum number of RX/TX focus
 *         delay steps for phased array.  So the maximum TX focus delay
 *         in nanoseconds is 'wMaxFocusSteps * wTxFocusGranularity',
 *         and the maximum RX focus delay in nanoseconds is
 *         'wMaxFocusSteps * wRxFocusGranularity'.
 *
 * 4.  The interpretation of the 'dwFiltScaleFreq' field value (in
 *     DBPOD_RSPBUF_GET_CAPABILITIES) and its effect on the scaling of
 *     the filter frequencies has changed when the value of
 *     'dwFiltScaleFreq' is less than the maximum digitization rate in
 *     'adwDigFreq[0]'.  Currently, this only affects Mini Phased Array
 *     devices because the MiniPod and FastPod have 'dwFiltScaleFreq'
 *     equal to 'adwDigFreq[0]'.  However, the interpretation of the
 *     new 'wScaleMaskLPF', 'wScaleMaskHPF', and 'wScaleMaskRFF'
 *     capabilities also affects the MiniPod devices.
 *
 *     The scaling of filter frequencies will be disabled for channels
 *     that have digitization rate ('nSampleFreq' in the channel
 *     configuration) set higher than (or equal to) the
 *     'dwFiltScaleFreq' value.
 *
 *     For backwards compatibility, when checking the configured filter
 *     frequencies in the channel configuration command, the pod
 *     software will only apply the "'nSampleFreq' higher than
 *     'dwFiltScaleFreq'" rule, and will only use the 'wScaleMaskHPF',
 *     'wScaleMaskLPF', and 'wScaleMaskRFF' bit-mask values when in the
 *     capabilities when the 'wCompatLevel' value  in the channel
 *     configuration is at least 2.
 *
 *     If 'wCompatLevel' is 0 (old application), the 'nHPF', 'nLPF',
 *     and 'nRFF' values will be matched against supported values
 *     listed in the capabilities as-is, with no scaling.  The
 *     'fScaleHPF', 'fScaleLPF', and 'fScaleRFF' flags, and the
 *     'wScaleMaskHPF', 'wScaleMaskLPF', and 'wScaleMaskRFF' bit-mask
 *     values are ignored.  This may result in the filter cut-off
 *     frequencies that are actually applied being different from the
 *     specified frequencies.
 *
 *     If 'wCompatLevel' is 1 (less old application), the 'nHPF',
 *     'nLPF', and 'nRFF' values will be matched against unscaled or
 *     scaled versions of the supported values listed in the
 *     capabilities.  It is assumed that the client is aware of the
 *     'fScaleHPF', 'fScaleLPF', and 'fScaleRFF' flags in the
 *     capabilities, but is unaware of the 'wScaleMaskHPF',
 *     'wScaleMaskLPF', and 'wScaleMaskRFF' bit-mask values in the
 *     capabiilities, and is also unaware that scaling should only be
 *     applied for digitization rates higher than the 'dwFiltScaleFreq'
 *     value in the capabilities.  It is assumed that the client has
 *     set the 'nHPF' value in the channel configuration message to
 *     match a scaled 'anHPF[]' entry if the 'fScaleHPF' value in the
 *     capabilities is TRUE, regardless of the 'wScaleMaskHPF' bit-mask
 *     value in the capabilities, and regardless of whether the
 *     digitization rate ('nSampleFreq' in the channel configuration) is
 *     lower or higher than the 'dwFiltScaleFreq' value in the
 *     capabilities.  Similarly, it is assumed that the client set the
 *     'nLPF' value in the channel configuration to match a scaled
 *     'anLPF[]' entry if the 'fScaleLPF' value in the capabilities is
 *     TRUE, regardless of the 'wScaleMaskLPF' value and the
 *     digitization rate, and it is assumed that the client has set the
 *     'nRFF' value in the channel configuration to match a scaled
 *     'anRFF[]' entry if the 'fScaleRFF' value in the capabilities is
 *     TRUE, regardless of the 'wScaleMaskRFF' value and the
 *     digitization rate).  This may result in the filter cut-off
 *     frequencies that are actually applied being different from the
 *     specified frequencies when the digitization rate is higher than
 *     the 'dwFiltScaleFreq' value or when only some of the supported
 *     filter cut-off frequencies are actually scaled.
 *
 *     If 'wCompatLevel' is 2 (new application), the 'nHPF', 'nLPF',
 *     and 'nRFF' values will be matched against unscaled or scaled
 *     versions of the supported values listed in the capabilities.
 *     It is assumed that the client is aware of the 'wScaleMaskHPF',
 *     'wScaleMaskLPF', and 'wScaleMaskRFF' bit-mask capabilities, and
 *     is aware of the rule that scaling should be disabled when the
 *     digitization rate ('nSampleFreq' in the channel configuration)
 *     is higher than the 'dwFiltScaleFreq' value in the capabilities.
 *     It is assumed that the client has set the 'nHPF' value in the
 *     channel configuration message to match a scaled 'anHPF[]' entry
 *     if and only if the corresponding bit of the 'wScaleMaskHPF'
 *     value in the capabilities is '1' and the digitization rate
 *     ('nSampleFreq' in the channel configuration) is lower than (or
 *     equal to the 'dwFiltScaleFreq' value in the capabilities.
 *     Similarly, it is assumed that the client set the 'nLPF' value in
 *     the channel configuration to match a scaled 'anLPF[]' entry if
 *     and only if the corresponding bit of the 'wScaleMaskLPF' value in
 *     the capabilities is '1' and the digitization rate is lower than
 *     (or equal to) the 'dwFiltScaleFreq' value, and has set the 'nRFF'
 *     value in the channel configuration to match a scaled 'anRFF[]'
 *     entry if and only if the corresponding bit of the 'wScaleMaskRFF'
 *     value in the capabilities is '1' and the digitization rate is
 *     lower than (or equal to) the 'dwFiltScaleFreq' value.
 *
 * 5.  Added 'Phased Array Channel Element/Delay' command and response
 *     (DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY and
 *     DBPOD_RSPBUF_PA_CHAN_ELEM_DELAY).  This command configures
 *     various parts of the phased array as determined by the value of
 *     the 'bTypeMask' field, and may be used several times to
 *     configure different parts.
 *
 *     Some parts are specific to a single sequence table entry,
 *     specified by the 'nIndex' field, and some parts are global.  The
 *     'nIndex' field can be set to -1 to apply sequence table entry
 *     specific parts to all sequence table entries.  The 'nIndex' field
 *     value is ignored when configuring global parts.
 *
 *     If the values for a particular type are identical (e.g. identical
 *     lists of TX and RX element numbers for a sequence table entry)
 *     then they can be configured in a single message by setting the
 *     'bTypeMask' field to a combination of bits.
 *
 *     The sequence table entry specific parts are the list of TX
 *     element numbers, the list of RX element numbers, the list of
 *     TX element delays, and the list of RX element delays.  The
 *     pod software will impose restrictions on the sets of element
 *     numbers that can be included in the list due to hardware
 *     limitations (e.g. no gaps in the element numbers in the list).
 *
 *     The global parts are a list containing the balancing gain for
 *     each physical element, and a list containing the balancing delay
 *     for each physical element.
 *
 * 6.  The 'nTx' and 'nRx' fields in the 'Channel Configuration' command
 *     structure (DBPOD_CMDBUF_CHAN_CONFIG) can be set to the special
 *     value -2 to specify that the sequence table entry will use the
 *     phased array elements.
 *
 */
#define DBPOD_CURRENT_PACKET_VERSION    0x50000

/*
 * Message Header.
 */
typedef struct TAG_DBPOD_MSGHDR
{
    ULONG       dwLength;       /* Length of packet (not including length) */
    ULONG       dwSequence;     /* Sequence number of command (copied to synchronous response) */
    USHORT      wCmd;           /* Command/Response code. */
    USHORT      wSubCode;       /* Sub code for response. */
} DBPOD_MSGHDR;

/********************************************
 *
 * Note that a synchronous error response to any command consists of
 * just a message header with the appropriate response code and a non-zero
 * sub-code.
 *
 ********************************************/

/* This flag in wCmd marks a synchronous response to a command. */
#define DBPOD_MSGFLAG_SYNC_RESPONSE         0x8000

/* This bit in wCmd marks the message as an asynchronous response with no
 * matching command. */
#define DBPOD_MSGFLAG_ASYNC_RESPONSE        0x4000

/********************************************
 *
 * 'Get Capabilities' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_GET_CAPABILITIES      0x0000
#define DBPOD_RSPCODE_GET_CAPABILITIES      0x8000

/* 'Get Capabilities' command buffer is just a header. */
typedef struct TAG_DBPOD_CMDBUF_GET_CAPABILITIES
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_GET_CAPABILITIES;

/*
 * Extended 'Get Capabilities' command buffer, including a 'dwMaxCapLength'
 * field for setting the maximum length of the capabilities, not including
 * the message header.
 */
typedef struct TAG_DBPOD_CMDBUF_GET_CAPABILITIES_EXT
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG dwMaxCapLength;           /* Maximum length of capabilities data,
                                       not including the message header. */
} DBPOD_CMDBUF_GET_CAPABILITIES_EXT;

/* Sample format for capabilities. */
typedef struct TAG_DBPOD_SAMPLE_FORMAT
{
    UCHAR nDigBits;                 /* Sample width in bits. */
    UCHAR nSpanBits;                /* Sample span in bits. */
    UCHAR nSamplesPerWord;          /* Samples per 32-bit word. */
    UCHAR bFlags;                   /* Flags (see below). */
} DBPOD_SAMPLE_FORMAT;

/* Sample format flags. */
#define DBPOD_SAMPLE_FLAG_SIGNED    0x01    /* 2's complement, not offset binary. */

/* 'Get Capabilities' response buffer. */
typedef struct TAG_DBPOD_RSPBUF_GET_CAPABILITIES
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    CHAR        szHwName[80];       /* Hw name. */
    ULONG       dwPodVersion;       /* Overall version of db-Pod system: HIWORD major, LOWORD minor. */
    ULONG       dwPacketVersion;    /* Version of packet structure: HIWORD major, LOWORD minor. */
    USHORT      wHardVersion;       /* Version of hardware. */
    USHORT      wDrvVersion;        /* Version of driver: HIBYTE major, LOBYTE minor. */
    LONG        fPower2Avg;         /* Flag: averager values must be a power of 2. */
    LONG        fGlobalAvg;         /* Flag: All firings must use same averaging value. */
    LONG        fOverlapGates;      /* Flag: Overlapping gates supported. */
    LONG        fDelayTiedToDig;    /* Flag: Delay generation clocked at digitiser. */
    SHORT       nSampleFormats;     /* Number of sample formats supported. */
    SHORT       fGlobalSampleFormat;/* Flag: Sample format must be configured alike for all channels. */
    DBPOD_SAMPLE_FORMAT SampleFormat[4]; /* Sample formats. */
    ULONG       dwMaxPoints;        /* Maximum number of points per firing. */
    SHORT       nDigFreq;           /* Number of digitisation frequencies supported. */
    USHORT      nChanConfigs;       /* Number of Channel/Sequence table entries supported by pod. */
    ULONG       adwDigFreq[16];     /* List of digitisation frequencies in Hz supported by pod. */
    ULONG       dwMaxDelay;         /* Maximum delay before digitiser in nanoseconds. */
    ULONG       dwMaxRange;         /* Maximum digitiser range in nanoseconds. */
    SHORT       nMaxAvg;            /* Maximum averaging value. */
    UCHAR       bHardType;          /* Hardware type code. */
    UCHAR       bAFEType;           /* Hardware AFE type code. */
    ULONG       dwFastMemSize;      /* ADC fast memory size in bytes. */
    ULONG       dwFifoMemSize;      /* ADC results FIFO size in bytes. */
    ULONG       dwDacMemSize;       /* DAC memory size in DAC curve elements. */
    ULONG       dwDacMemPageSize;   /* DAC memory page size in DAC curve elements. */
    LONG        fPower2DacDivisor;  /* Flag: DAC sample rate divisor must be a power of 2. */
    SHORT       nMinDacDivisor;     /* Minimum DAC sample rate divisor. */
    SHORT       nMaxDacDivisor;     /* Maximum DAC sample rate divisor. */
    SHORT       nMinHT;             /* Minimum HT voltage in volts. */
    SHORT       nMaxHT;             /* Maximum HT voltage in volts. */
    SHORT       nHTResolution;      /* HT voltage resolution in volts. */
    UCHAR       bPAType;            /* Phased array type. */
    UCHAR       pad4;               /* (padding = 0) */
    LONG        fGlobalHT;          /* Flag: HT voltage must be configured alike for all channels. */
    LONG        fGlobalPW;          /* Flag: pulse width must be configured alike for all channels. */
    SHORT       nMinPW;             /* Minimum pulse width in nanoseconds. */
    SHORT       nMaxPW;             /* Maximum pulse width in nanoseconds. */
    SHORT       nPWResolution;      /* Pulse width resolution in nanoseconds. */
    SHORT       nMaxPulserPower;    /* Maximum pulser output power in Watts. */
    SHORT       nMinGain;           /* Minimum gain in millibels (100 mB = 1 dB). */
    SHORT       nMaxGain;           /* Maximum gain in millibels. */
    SHORT       nGainResolution;    /* Gain resolution in millibels. */
    SHORT       nLowPass;           /* Number of low-pass filter breakpoints. A -1 indicates a continuous range. */
    LONG        anLPF[16];          /* Low-pass filter breakpoints, or continuous range from anLPF[0] to anLPF[1], in Hz. */
    LONG        fGlobalLPF;         /* Flag: low-pass filters must be configured alike for all channels. */
    SHORT       nHighPass;          /* Number of high-pass filter breakpoints. A -1 indicates a continuous range. */
    SHORT       pad5;               /* (padding = 0) */
    LONG        anHPF[16];          /* High-pass filter breakpoints, or continuous range from anHPF[0] to anHPF[1], in Hz. */
    LONG        fGlobalHPF;         /* Flag: high-pass filters must be configured alike for all channels. */
    SHORT       nRectFilters;       /* Number of post-rectification filter breakpoints. A -1 indicates a continuous range. */
    SHORT       pad6;               /* (padding = 0) */
    LONG        anRFF[16];          /* Post-rectification filter breakpoints, or continuous range from anRFF[0] to anRFF[1], in Hz. */
    ULONG       dwFiltScaleFreq;    /* Filter scale frequency.
                                       Used in conjunction with fScaleLPF,
                                       fScaleHPF, and fScaleRFF.  If those
                                       are all FALSE, dwFiltScaleFreq is
                                       unused and may be 0.  Otherwise, it
                                       holds the denomimator of the scaling
                                       factor (generally the highest
                                       digitization frequency). */
    LONG        fGlobalRFF;         /* Flag: post-rectification filters must be configured alike for all channels. */
    SHORT       nMinTrigPulse;      /* Minimum pulse width of the external trigger input in microseconds. */
    SHORT       nChannels;          /* Number of conventional physical
                                       transducer channels (not phased array). */
    SHORT       nDACs;              /* Maximum number of DAC curves. */
    SHORT       nGates;             /* Maximum number of gates per channel. */
    SHORT       nEncoders;          /* Number of encoder axes. */
    SHORT       nPots;              /* Number of potentiometer axes. */
    SHORT       nPotBits;           /* Number of bits on the potentiometer ADC. */
    USHORT      wProjNum;           /* Project number for hardware specials. */
    CHAR        fVideoTracking;     /* Video tracking available. */
    CHAR        fScaleLPF;          /* Low-pass filter breakpoint scaling.
                                       If FALSE, values in anLPF[] are
                                       absolute.  If TRUE, values in anLPF[]
                                       should be multiplied by the digitization
                                       frequency and divided by
                                       dwFiltScaleFreq.  */
    CHAR        fScaleHPF;          /* High-pass filter breakpoint scaling.
                                       If FALSE, values in anHPF[] are
                                       absolute.  If TRUE, values in anHPF[]
                                       should be multiplied by the digitization
                                       frequency and divided by
                                       dwFiltScaleFreq.  */
    CHAR        fScaleRFF;          /* Post-rectification filter breakpoint
                                       scaling.
                                       If FALSE, values in anRFF[] are
                                       absolute.  If TRUE, values in anRFF[]
                                       should be multiplied by the digitization
                                       frequency and divided by
                                       dwFiltScaleFreq.  */
    ULONG       dwMinPRF;           /* Minimum PRF in microseconds. */
    ULONG       dwMaxPRF;           /* Maximum PRF in microseconds. */
    /*
     * The non-extended 'Get Capabilities' response ends at this point.
     * Remaining fields will only be sent if the extended version of
     * the 'Get Capabilities' command is used with a large enough
     * 'dwMaxCapLength' value.  The 'dwMaxCapLength' value specifies the
     * maximum length of the 'Get Capabilities' response not including the
     * message header (i.e. starting from the 'szHwName' field onwards).
     *
     * The length from the start of the 'szHwName' field up to this point
     * is 512 bytes.
     */
    USHORT      wScaleMaskLPF;      /* Low-pass filter breakpoint scaling
                                       bit-mask.  Indicates which entries in
                                       anLPF[] are scaled and which ones are
                                       absolute.  A '1' in a bit position
                                       indicates that the entry at the
                                       corresponding index in anLPF[] is scaled
                                       and a '0' in a bit position indicates
                                       that the entry at the corresponding
                                       index in anLPF[] is absolute.  This
                                       is more informative than fScaleLPF.  */
    USHORT      wScaleMaskHPF;      /* High-pass filter breakpoint scaling
                                       bit-mask.  Indicates which entries in
                                       anHPF[] are scaled and which ones are
                                       absolute.  A '1' in a bit position
                                       indicates that the entry at the
                                       corresponding index in anHPF[] is scaled
                                       and a '0' in a bit position indicates
                                       that the entry at the corresponding
                                       index in anHPF[] is absolute.  This
                                       is more informative than fScaleHPF.  */
    USHORT      wScaleMaskRFF;      /* Post-rectification filter breakpoint
                                       scaling bit-mask.  Indicates which
                                       entries in anRFF[] are scaled and which
                                       ones are absolute.  A '1' in a bit
                                       position indicates that the entry at the
                                       corresponding index in anRFF[] is scaled
                                       and a '0' in a bit position indicates
                                       that the entry at the corresponding
                                       index in anRFF[] is absolute.  This
                                       is more informative than fScaleRFF.  */
    USHORT      wMaxAperture;       /* Phased array maximum aperture size
                                       (maximum number of parallel ADCs.) */
    USHORT      wMaxElements;       /* Phased array max number of elements. */
    USHORT      wTxFocusGranularity;/* TX focus (delays) granularity in ns. */
    USHORT      wRxFocusGranularity;/* RX focus (delays) granularity in ns. */
    USHORT      wMaxFocusSteps;     /* Maximum number of TX/RX delay steps
                                       that the hardware supports.  So the
                                       maximum RX delay in nanoseconds is
                                       wMaxFocusSteps * wRxFocusGranularity,
                                       and the maximum TX delay in nanoseconds
                                       is wMaxFocusSteps * wTxFocusGranularity. */
    /*
     * The length from the start of the 'szHwName' field up to this point
     * is 528 bytes.
     */
} DBPOD_RSPBUF_GET_CAPABILITIES;

/* Hardware type codes. */
#define DBPOD_HARDTYPE_ORIG_ETH     0   /* Classic Ethernet Pod (Nios II). */
#define DBPOD_HARDTYPE_FAST_ETH     1   /* Fast Ethernet Pod (ARM). */
#define DBPOD_HARDTYPE_MINI_ETH     2   /* Ethernet Mini Pod (ARM). */
#define DBPOD_HARDTYPE_MINI_USB     3   /* USB3.0 Mini Pod (FTDI). */
#define DBPOD_HARDTYPE_MINIPA_ETH   4   /* Ethernet Mini Phased Array (ARM). */

/* Hardware AFE type codes. */
#define DBPOD_AFETYPE_ORIGINAL      0   /* Classic Ethernet Pod or Fast Pod. */
#define DBPOD_AFETYPE_MAX2077       1   /* MAX2077 - Mini Pod (Ether or USB). */
#define DBPOD_AFETYPE_VCA5807       2   /* VCA5807 - Mini Phased Array. */

/* Hardware phased array type codes. */
#define DBPOD_PATYPE_NONE           0   /* No phased array. */
#define DBPOD_PATYPE_MINI_PBF       1   /* Mini PA partial beam former. */

/********************************************
 *
 * 'Diagnostic Tests' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_DIAGS                 0x0001
#define DBPOD_RSPCODE_DIAGS                 0x8001

/* 'Diagnostic Tests' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_DIAGS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       nCommand;           /* Diagnostic command to perform. */
    ULONG       nDiagTests;         /* Bitmask of diagnostic tests. */
} DBPOD_CMDBUF_DIAGS;

/*
 * nCommand
 */
#define DBPOD_DIAGS_COMMAND_GETTEST     0   /* Get test results so far. */
#define DBPOD_DIAGS_COMMAND_START       1   /* Start tests. */
#define DBPOD_DIAGS_COMMAND_START_VERBOSE 2 /* Start tests verbosely. */

/*
 * nDiagTests
 */
/* XXX TO BE DEFINED! XXX */

/* 'Diagnostic Tests' response buffer. */
typedef struct TAG_DBPOD_RSPBUF_DIAGS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       nDiagTests;         /* Bitmask of diagnostic tests. */
    ULONG       nDiagComplete;      /* Bitmask of completed tests. */
    ULONG       nDiagPassed;        /* Bitmask of passed tests. */
    char        szResult[128];      /* Text string giving description of a test outcome. */
    /* XXX TODO - Append extra information, e.g. analog offsets. */
} DBPOD_RSPBUG_DIAGS;

/********************************************
 *
 * 'Dummy' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_DUMMY                 0x0002
#define DBPOD_RSPCODE_DUMMY                 0x8002

/* 'Dummy' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_DUMMY
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       dwReplyLen;         /* Length of data to send in reply. */
} DBPOD_CMDBUF_DUMMY;

/* 'Dummy' response buffer. */
typedef struct TAG_DBPOD_RSPBUF_DUMMY
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    UCHAR       bDummyData[DBPOD_ANYLENGTH];    /* Dummy data bytes. */
} DBPOD_RSPBUF_DUMMY;
/* Basic size of DBPOD_RSPBUF_DUMMY without bDummyData[]. */
#define BASE_SIZE_DBPOD_RSPBUF_DUMMY \
    FIELD_OFFSET(DBPOD_RSPBUF_DUMMY, bDummyData[0])
/* Followed by array UCHAR bDummyData[dwReplyLen] plus possible padding
 * (where dwReplyLen is from the 'Dummy' command). */

/********************************************
 *
 * 'Start UT' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_START_UT              0x0010
#define DBPOD_RSPCODE_START_UT              0x8010

/* 'Start UT' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_START_UT
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    USHORT      nStartIndex;        /* Sequence table index to start at. */
    USHORT      nEndIndex;          /* Sequence table index to stop after. */
    LONG        fMaster;            /* TRUE if pod is master, FALSE if slave. */
    ULONG       dwFlags;            /* Various flags. */
} DBPOD_CMDBUF_START_UT;

/* dwFlags values. */
#define DBPOD_START_UT_DESPARKLE  0x00000001  /* Turn on desparkler. */
#define DBPOD_START_UT_COMPRESS   0x00000002  /* Turn on compression. */
#define DBPOD_START_UT_EXTRA_ENCS 0x00000004  /* Extra encoders. */
/*
 * Note: see the description of DBPOD_CHUNK_UT_EOC for the effect of setting
 * DBPOD_START_UT_EXTRA_ENCS.
 */

/* 'Start UT' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_START_UT
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_START_UT;

/********************************************
 *
 * 'Stop UT' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_STOP_UT               0x0011
#define DBPOD_RSPCODE_STOP_UT               0x8011

/* 'Stop UT' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_STOP_UT
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_STOP_UT;

/* 'Stop UT' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_STOP_UT
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_STOP_UT;

/********************************************
 *
 * 'Start VC' (video capture) command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_START_VC              0x0012
#define DBPOD_RSPCODE_START_VC              0x8012

/* 'Start VC' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_START_VC
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_START_VC;

/* 'Start VC' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_START_VC
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_START_VC;

/********************************************
 *
 * 'Stop VC' (video capture) command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_STOP_VC               0x0013
#define DBPOD_RSPCODE_STOP_VC               0x8013

/* 'Stop VC' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_STOP_VC
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_STOP_VC;

/* 'Stop VC' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_STOP_VC
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_STOP_VC;

/********************************************
 *
 * 'Set Flash Parameters' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_SET_FLASH_PARAMS      0x0020
#define DBPOD_RSPCODE_SET_FLASH_PARAMS      0x8020

/* 'Set Flash Parameters' command buffer. */
/* (Same as 'Get Flash Parameters' response buffer.) */
typedef struct TAG_DBPOD_CMDBUF_SET_FLASH_PARAMS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    FILETIME    CalibTime;          /* Calibration timestamp in 100 ns intervals
                                     * since January 1, 1601 (UTC) */
    ULONG       ValidChans;         /* Bit-mask of valid channels */
    SHORT       NullOffset;         /* Null Offset corrections */
    SHORT       GainRef2;           /* Gain Stage 2 reference tweak */
    SHORT       GainRef3;           /* Gain Stage 3 reference tweak */
    SHORT       GainMult;           /* DAC gain correction multiplier * 1000 */
    SHORT       GainOffset;         /* DAC gain correction offset in db * 100 */
    SHORT       pad1;               /* (padding = 0) */
} DBPOD_CMDBUF_SET_FLASH_PARAMS;

/* 'Set Flash Parameters' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_SET_FLASH_PARAMS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_SET_FLASH_PARAMS;

/********************************************
 *
 * 'Get Flash Parameters' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_GET_FLASH_PARAMS      0x0021
#define DBPOD_RSPCODE_GET_FLASH_PARAMS      0x8021

/* 'Get Flash Parameters' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_GET_FLASH_PARAMS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_GET_FLASH_PARAMS;

/* 'Get Flash Parameters' response buffer is same as 'Set Flash Parameters' command buffer. */
typedef DBPOD_CMDBUF_SET_FLASH_PARAMS   DBPOD_RSPBUF_GET_FLASH_PARAMS;

/********************************************
 *
 * 'Get MAC Address' command and response.
 *
 * Note: The MAC address should be unique to a particular pod.)
 *
 ********************************************/
#define DBPOD_CMDCODE_GET_MAC_ADDRESS       0x0022
#define DBPOD_RSPCODE_GET_MAC_ADDRESS       0x8022

/* 'Get MAC Address' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_GET_MAC_ADDRESS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_GET_MAC_ADDRESS;

/* 'Get MAC Address' response buffer. */
typedef struct TAG_DBPOD_RSPBUF_GET_MAC_ADDRESS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    UCHAR       MacAddress[6];      /* MAC address. */
    SHORT       pad1;               /* (padding = 0) */
} DBPOD_RSPBUF_GET_MAC_ADDRESS;

/********************************************
 *
 * 'Channel/Sequence Configuration' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_CHAN_CONFIG           0x0030
#define DBPOD_RSPCODE_CHAN_CONFIG           0x8030

/*
 * A Gate configuration tacked onto the 'Channel Configuration' structure.
 */
typedef struct TAG_DBPOD_GATECFG
{
    UCHAR       GateType;           /* Gate type. */
    UCHAR       Reference;          /* Gate reference. */
    SHORT       pad1;               /* (padding = 0) */
    LONG        nThreshold;         /* Threshold percentage x 10 (not applicable to all gates). */
    LONG        lStart;             /* Sample at which gate starts in digitiser clock periods. */
    LONG        lWidth;             /* Number of samples in gate in digitiser clock periods. */
    LONG        nControl;           /* Gate type dependent value */
    LONG        lParameter;         /* Parameter (gate type dependent value):- */
                                    /* I/F Gate: Interface Gain in millibels (100 mB = 1 dB) */
} DBPOD_GATECFG;

/*
 * GateType
 */
#define DBPOD_GATETYPE_AVERAGING    0   /* Average gate */
#define DBPOD_GATETYPE_INTERFACE    1   /* Interface gate */
#define DBPOD_GATETYPE_DISPLAY      2   /* Display gate */
#define DBPOD_GATETYPE_RECORD       3   /* Record gate */
#define DBPOD_GATETYPE_PEAK         4   /* Peak gate */
#define DBPOD_GATETYPE_LOSS_SIG     5   /* Loss Of Signal gate */
#define DBPOD_GATETYPE_GAIN_CTRL    6   /* Gain Control Interface gate */

/*
 * GateReference
 */
#define DBPOD_GATEREF_ABSOLUTE      0   /* Absolute reference */
#define DBPOD_GATEREF_INTERFACE     1   /* Interface reference */
#define DBPOD_GATEREF_PREVGATE      2   /* Reference previous gate */

/*
 * nControl depends on GateType
 */

 /* nControl for DBPOD_GATETYPE_AVERAGING. */
#define DBPOD_AVERAGING_MEAN        0
#define DBPOD_AVERAGING_MAX         1

/* nControl for DBPOD_GATETYPE_INTERFACE and DBPOD_GATETYPE_LOSS_SIG is polarity. */
#define DBPOD_POLARITY_UNIPOLAR     0
#define DBPOD_POLARITY_BIPOLAR      1

/* nControl for DBPOD_GATETYPE_PEAK is split into number of peaks (bits 7 to 0)
 * and peak type (bits 15 to 8).  Only SORTED is currently supported by db-Pod */
#define DBPOD_PEAKS_NCONTROL(num, type) (((num) & 255) | ((type) << 8))
#define DBPOD_PEAKS_NUM(nControl)       ((nControl) & 255)
#define DBPOD_PEAKS_TYPE(nControl)      (((nControl) >> 8) & 255)
#define DBPOD_PEAKTYPE_FIRST_N_POS      0   /* First n Positive peaks */
#define DBPOD_PEAKTYPE_FIRST_N          1   /* First n Bipolar (pos&neg) peaks*/
#define DBPOD_PEAKTYPE_SORTED           2   /* Sorted peaks number ignored */

/*
 * lParameter depends on GateType
 */

/* lParameter for DBPOD_GATETYPE_INTERFACE is initial gain in milliBels */

/* 'Channel/Sequence Configuration' command buffer (portion before the gates). */
/* (Note: db-Pod does not support nRepeat values other than 1.) */
typedef struct TAG_DBPOD_CMDBUF_CHAN_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    USHORT      nIndex;             /* Sequence table index i.e. channel. */
    SHORT       nDigBits;           /* Sample size in bits. */
    ULONG       dwDelay;            /* Time period in microseconds before stepping or repeating. */
    SHORT       nRepeat;            /* Number of repeat firings with dwDelay interval. */
    SHORT       nTx;                /* Physical transmit channel to pulse:
                                       0 to nChannels-1 = use specified channel.
                                       -1 = do not pulse.
                                       -2 = use phased array. */
    SHORT       nRx;                /* Physical receiver input channel:
                                       0 to nChannels-1 = use specified channel.
                                       -1 = disable receiver.
                                       -2 = use phased array. */
    SHORT       nVoltage;           /* Pulser voltage in volts. */
    SHORT       nPulseWidth;        /* Pulse Width in nanoseconds. */
    SHORT       nGain;              /* Gain in millibels (100 mB = 1 dB). */
    LONG        nHPF;               /* High pass filter in Hz, 0=disable. */
    LONG        nLPF;               /* Low pass filter in Hz, 0=disable. */
    USHORT      RectifierType;      /* Rectification type. */
    USHORT      wCompatLevel;       /* Compatibility level.
                                       If wCompatLevel=0 (old application),
                                       filter frequency scaling is disabled;
                                       the 'fScaleLPF', 'fScaleHPF', and
                                       'fScaleRFF' flags in the capabilities
                                       response will be ignored for backwards
                                       compatibility reasons so 'nHPF', 'nLPF'
                                       and 'nRFF' will be matched against the
                                       absolute values in the capabilities;
                                       this may result in the wrong filter
                                       frequencies being applied.
                                       If wCompatLevel=1 (new application),
                                       filter frequency scaling is enabled;
                                       the 'nHPF', 'nLPF' and 'nRFF' values
                                       are assumed to be set to scaled or
                                       unscaled values depending on the
                                       'fScaleHPF', 'fScaleLPF', and
                                       'fScaleRFF' values in the capabilities
                                       response; if the digitization rate is
                                       above the 'dwFiltScaleFreq' value in
                                       the capabilities then incorrect filter
                                       frequencies will be applied.
                                       If wCompatLevel=2, filter frequency
                                       scaling is enabled only for
                                       digitization rates below the
                                       'dwFiltScaleFreq' value in the
                                       capabilities; the 'nHPF', 'nLPF' and
                                       'nRFF' values are assumed to be set to
                                       scaled or unscaled values depending on
                                       the 'fScaledHPF', 'fScaledLPF', and
                                       'fScaledRFF' values in the capabilities
                                       response, and on the digitization rate;
                                       the scaling is not applied for
                                       digitization rates above the
                                       'dwFiltScaleFreq' value in the
                                       capabilities response.
                                       */
    LONG        nRFF;               /* Post rectifier filter (low pass) in Hz. */
    ULONG       nSampleFreq;        /* Digitisation rate (Hz). */
    ULONG       dwDACStart;         /* DAC curve start element.  Must be a multiple of DAC memory page size. */
    ULONG       dwDACLength;        /* DAC curve length. 0=no DAC */
    USHORT      nDACDivisor;        /* DAC output frequency divisor relative to Dig Freq */
    USHORT      DacTrigger;         /* DAC trigger, 0=TX pulse, 1=Interface trigger. */
    SHORT       nGates;             /* Number of gates defined. */
    USHORT      wSpecial;           /* Hardware-specific setting. */
    DBPOD_GATECFG  Gate[DBPOD_ANYLENGTH]; /* Gates. */
} DBPOD_CMDBUF_CHAN_CONFIG;
/* Basic size of DBPOD_CMDBUF_CHAN_CONFIG type without Gate[]. */
#define BASE_SIZE_DBPOD_CMDBUF_CHAN_CONFIG  \
    FIELD_OFFSET(DBPOD_CMDBUF_CHAN_CONFIG, Gate[0])
/* Followed by array DBPOD_GATECFG Gate[nGates]. */

/*
 * RectifierType
 */
#define DBPOD_RECTIFIER_NONE    0
#define DBPOD_RECTIFIER_NEG     1
#define DBPOD_RECTIFIER_POS     2
#define DBPOD_RECTIFIER_FULL    3

/*
 * DacTrigger
 */
#define DBPOD_DACTRIG_TXPULSE   0
#define DBPOD_DACTRIG_IFTRIG    1

/* 'Channel/Sequence Config' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_CHAN_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_CHAN_CONFIG;

/********************************************
 *
 * 'DAC Memory Set' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_DAC_MEMORY_SET        0x0031
#define DBPOD_RSPCODE_DAC_MEMORY_SET        0x8031

/* 'DAC Memory Set' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_DAC_MEMORY_SET
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       dwStart;            /* Start index.  Must be a multiple of DAC memory page size. */
    ULONG       dwLength;           /* Number of gain values */
    SHORT       nGain[DBPOD_ANYLENGTH]; /* Gain values in dB x 100 i.e. 1/100ths of dB */
} DBPOD_CMDBUF_DAC_MEMORY_SET;
/* Basic size of DBPOD_CMDBUF_DAC_MEMORY_SET type without nGain[]. */
#define BASE_SIZE_DBPOD_CMDBUF_DAC_MEMORY_SET   \
    FIELD_OFFSET(DBPOD_CMDBUF_DAC_MEMORY_SET, nGain[0])
/* Followed by array of SHORT nGain[dwLength] : Gain in deciBels x 100,
 * i.e. 1/100ths of dB. */

/* 'DAC Memory Set' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_DAC_MEMORY_SET
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_DAC_MEMORY_SET;

/********************************************
 *
 * 'Encoder Config' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_ENCODER_CONFIG        0x0032
#define DBPOD_RSPCODE_ENCODER_CONFIG        0x8032

/* 'Encoder Config' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_ENCODER_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    USHORT      wDifferential;      /* Differential inputs - bitmask (bit == axis). */
    USHORT      wResetEnable;       /* Enable ext. resets - bitmask (bit == axis). */
    USHORT      wInvertSense;       /* Invert encoder - bitmask (bit == axis). */
    USHORT      wResetSource;       /* External reset source. */
} DBPOD_CMDBUF_ENCODER_CONFIG;

/* 'Encoder Config' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_ENCODER_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_ENCODER_CONFIG;

/********************************************
 *
 * 'Video Config' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_VIDEO_CONFIG          0x0033
#define DBPOD_RSPCODE_VIDEO_CONFIG          0x8033

/* 'Video Config' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_VIDEO_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    SHORT       nStandard;          /* Video std (0=PAL, 1=NSTC, -1=disabled) */
    SHORT       nInterleave;        /* Interleave (1=odd, 2=even, 3=both) */
    SHORT       nTrkFrameRate;      /* Frame Rate for video tracking (4 to 25 or 30) */
    SHORT       nXTrkFrameSize;     /* Total number of Horz pixels in frame for tracking */
    SHORT       nYTrkFrameSize;     /* Total number of Vert pixels in frame for tracking */
    SHORT       nXTrkOrigin;        /* Digitisation origin (x co-ord) for tracking */
    SHORT       nYTrkOrigin;        /* Digitisation origin (y co-ord) for tracking */
    SHORT       nXTrkCropSize;      /* Number of x-pixels to digitise for tracking */
    SHORT       nYTrkCropSize;      /* Number of y-pixels to digitise for tracking */
    SHORT       nStrmSizeScale;     /* Video stream frame size reduction factor compared to tracking frame (1) */
                                    /* Note: Microplus-II supported
                                     * nStrmSizeScale values of 1 and 2.
                                     * db-Pod only supports value 1.
                                     *
                                     * Note: nStrmSizeScale is the horizontal
                                     * size reduction factor.
                                     *
                                     * Note: vertically, db-Pod returns two
                                     * half-frame fields for the video data,
                                     * whereas Microplus-II returned a single
                                     * field.
                                     */
    SHORT       nStrmRateScale;     /* Video stream frame rate reduction
                                     * factor compared to tracking (1 to 16)
                                     * or 0 to disable video capture. */
    SHORT       pad1;               /* (padding = 0) */
    LONG        fAGC;               /* LED feedback gain control active */
    SHORT       nAGCThreshold;      /* LED threshold (if AGC enabled) */
    SHORT       nFormat;            /* Video Stream format */
    SHORT       nNoDotThreshold;    /* "No dot" threshold. */
    SHORT       pad2;               /* (padding = 0) */
} DBPOD_CMDBUF_VIDEO_CONFIG;

/*
 * nStandard
 */
#define DBPOD_VID_STD_DISABLE       (-1)    /* "Unconfigure" video. */
#define DBPOD_VID_STD_PAL           0
#define DBPOD_VID_STD_NTSC          1

/*
 * nInterleave
 */
#define DBPOD_VID_INTERLEAVE_ODD    1
#define DBPOD_VID_INTERLEAVE_EVEN   2
#define DBPOD_VID_INTERLEAVE_BOTH   3

/*
 * nStrmSizeScale
 *
 * Note: This is the horizontal size reduction factor.  The vertical size
 * reduction factor is always 2 because fields are captured, not frames.
 *
 * Note: db-Pod does not currently support horizontal scaling.
 */
#define DBPOD_VID_STRMSIZESCALE_FULL    1
#define DBPOD_VID_STRMSIZESCALE_HALF    2   /* Not currently supported on db-Pod. */

/*
 * nFormat
 */
#define DBPOD_VID_UNKNOWN_FORMAT    (-1)
#define DBPOD_VID_RAWGREY8          0
#define DBPOD_VID_RAWGREY8VT        1

/* 'Video Config' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_VIDEO_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_VIDEO_CONFIG;

/********************************************
 *
 * 'Scan Config' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_SCAN_CONFIG           0x0034
#define DBPOD_RSPCODE_SCAN_CONFIG           0x8034

/* 'Scan Config' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_SCAN_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    USHORT      ScanType;           /* Free run or tied to position. */
    USHORT      nScanAxis;          /* Axis to monitor when tied to position. */
    LONG        lScanStart;         /* Scan start position. */
    LONG        lScanEnd;           /* Scan end position. */
    ULONG       ScanStep;           /* Step between scans 16.16 fraction. */
} DBPOD_CMDBUF_SCAN_CONFIG;

/* Scan types. */
#define DBPOD_SCAN_FREERUN          0
#define DBPOD_SCAN_TIEDTOPOSITION   1
#define DBPOD_SCAN_FREEPOSITION     2

/* 'Scan Config' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_SCAN_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_SCAN_CONFIG;

/********************************************
 *
 * 'Phased Array Channel Element/Delay' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_PA_CHAN_ELEM_DELAY    0x0035
#define DBPOD_RSPCODE_PA_CHAN_ELEM_DELAY    0x8035

/* 'Phased Array Channel Element/Delay' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    SHORT       nIndex;             /* Sequence table index.
                                       If set to -1 it applies to all
                                       sequence table entries.
                                       If set in range 0 to nChanConfigs-1
                                       it applies to a specific sequence
                                       table entry.
                                       nIndex is ignored for balancing
                                       gains and balancing delays. */
    UCHAR       bTypeMask;          /* Type of data being configured. */
    CHAR        pad1;               /* (padding = 0) */
    USHORT      nCount;             /* Length of dwValue array. */
    SHORT       pad2;               /* (padding = 0) */
    ULONG       dwValue[DBPOD_ANYLENGTH]; /* Element numbers or delays (ns). */
} DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY;

/*
 * bTypeMask
 *
 * Specifies whether the 'Phased Array Channel Element/Delay' command is
 * configuring a list of TX element numbers, RX element numbers,
 * TX delays, or RX delays.
 *
 * It is possible to combine TX element numbers and RX element numbers into
 * a single list, or combine TX and RX delays into a single list by setting
 * multiple bits in the bTypeMask value.
 *
 * Also used for lists of element balancing gains and element balancing delays.
 * The nIndex value is ignored for element balancing gains and element
 * balancing delays.
 */
#define DBPOD_PATYPE_TX_ELEM    0x01    /* TX element numbers */
#define DBPOD_PATYPE_RX_ELEM    0x02    /* RX element numbers */
#define DBPOD_PATYPE_TX_DELAY   0x04    /* TX delays (nanoseconds) */
#define DBPOD_PATYPE_RX_DELAY   0x08    /* RX delays (nanoseconds) */
#define DBPOD_PATYPE_BAL_GAIN   0x10    /* Balancing gain (millibels) */
#define DBPOD_PATYPE_BAL_DELAY  0x20    /* Balancing delay (nanoseconds) */

/*
 * 'Phased Array Channel Element/Delay' response buffer is just a
 * message header.
 */
typedef struct TAG_DBPOD_RSPBUF_PA_CHAN_ELEM_DELAY
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_PA_CHAN_ELEM_DELAY;

/********************************************
 *
 * 'Set Encoders' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_SET_ENCODERS          0x0040
#define DBPOD_RSPCODE_SET_ENCODERS          0x8040

typedef struct TAG_DBPOD_CMDBUF_SET_ENCODERS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    USHORT      nAxisBits;          /* Axes to set position. */
    SHORT       pad1;               /* (padding = 0) */
    LONG        lReading[DBPOD_ANYLENGTH]; /* New settings for each specified axis. */
} DBPOD_CMDBUF_SET_ENCODERS;
/* Basic size of DBPOD_CMDBUF_SET_ENCODERS without lReading[]. */
#define BASE_SIZE_DBPOD_CMDBUF_SET_ENCODERS \
    FIELD_OFFSET(DBPOD_CMDBUF_SET_ENCODERS, lReading[0])
/* Followed by array LONG lReading[n], where n is the number of 1 bits set
 * in nAxisBits. */

/* 'Set Encoders' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_SET_ENCODERS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_SET_ENCODERS;

/********************************************
 *
 * 'Get Encoders' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_GET_ENCODERS          0x0041
#define DBPOD_RSPCODE_GET_ENCODERS          0x8041

/* 'Get Encoders' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_GET_ENCODERS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_GET_ENCODERS;

/* 'Get Encoders' response buffer. */
typedef struct TAG_DBPOD_RSPBUF_GET_ENCODERS
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    USHORT      nAxes;              /* Number of axes that follow. */
    SHORT       pad1;               /* (padding = 0) */
    LONG        lReading[DBPOD_ANYLENGTH]; /* Readings for axes 0 to nAxes-1. */
} DBPOD_RSPBUF_GET_ENCODERS;
/* Basic size of DBPOD_RSPBUF_GET_ENCODERS without lReading[]. */
#define BASE_SIZE_DBPOD_RSPBUF_GET_ENCODERS \
    FIELD_OFFSET(DBPOD_RSPBUF_GET_ENCODERS, lReading[0])
/* Followed by array LONG lReading[nAxes]. */
/* For db-Pod, lReading[2] is VT X coordinate, lReading[3] is VT Y coordinate. */

/********************************************
 *
 * 'Set LED' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_SET_LED               0x0042
#define DBPOD_RSPCODE_SET_LED               0x8042

/* 'Set LED' command buffer. */
/* (Same as 'Get LED' response buffer.) */
typedef struct TAG_DBPOD_CMDBUF_SET_LED
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    SHORT       nDrive;             /* LED intensity (0..255). */
    SHORT       nNoDotThreshold;    /* Video "no dot" threshold (0..255). */
} DBPOD_CMDBUF_SET_LED;

/* 'Set LED' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_SET_LED
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_SET_LED;

/********************************************
 *
 * 'Get LED' command and response.
 *
 * Note: It actually gets the last peak video value.
 *       'nDrive' is set to the last peak video value.
 *       'nNoDotThreshold' is set to the configured "no dot" threshold.
 *
 ********************************************/
#define DBPOD_CMDCODE_GET_LED               0x0043
#define DBPOD_RSPCODE_GET_LED               0x8043

/* 'Get LED' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_GET_LED
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_GET_LED;

/* 'Get LED' response buffer is same as 'Set LED' command buffer. */
typedef DBPOD_CMDBUF_SET_LED    DBPOD_RSPBUF_GET_LED;

/********************************************
 *
 * 'MDU Config' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_MDU_CONFIG            0x0050
#define DBPOD_RSPCODE_MDU_CONFIG            0x8050

/* 'MDU Config' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_MDU_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       dwSpeed;            /* Bit rate. */
    UCHAR       nDataBits;          /* Data bits (5, 6, 7, 8). */
    UCHAR       nParity;            /* Parity (0=None, 1=Odd, 2=Even). */
    UCHAR       nStopBits;          /* Stop bits (1, 2). */
    UCHAR       nPort;              /* MDU port number. */
} DBPOD_CMDBUF_MDU_CONFIG;

/*
 * nParity
 */
#define DBPOD_MDU_PARITY_NONE       0
#define DBPOD_MDU_PARITY_ODD        1
#define DBPOD_MDU_PARITY_EVEN       2

/* 'MDU Config' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_MDU_CONFIG
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_MDU_CONFIG;

/********************************************
 *
 * 'MDU Data' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_MDU_DATA              0x0051
#define DBPOD_RSPCODE_MDU_DATA              0x8051

/* 'MDU Data' command buffer. */
/* (Same as 'MDU Data' response buffer.) */
typedef struct TAG_DBPOD_CMDBUF_MDU_DATA
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       dwTimeout;          /* Time to wait for reply from MDU in milliseconds. */
                                    /* 0=no reply expected. */
    USHORT      wLength;            /* Number of bytes to send to MDU. */
                                    /* (For response, number of bytes received.) */
    UCHAR       nPort;              /* MDU port number. */
    CHAR        fContinuation;      /* If 0, old rx data is discarded. */
                                    /* If non-0, old rx data is read (can be
                                     * used with dwTimeout==0 to read rx data
                                     * from previous command). */
    UCHAR       bData[DBPOD_ANYLENGTH];    /* Data bytes. */
} DBPOD_CMDBUF_MDU_DATA;
/* Basic size of DBPOD_CMDBUF_MDU_DATA without bData[]. */
#define BASE_SIZE_DBPOD_CMDBUF_MDU_DATA \
    FIELD_OFFSET(DBPOD_CMDBUF_MDU_DATA, bData[0])
/* Followed by array UCHAR bData[dwLength] plus possible padding. */

/* 'MDU Data' response buffer is same as 'MDU Data' command buffer except that
 * dwLength is the number of bytes received. */
typedef DBPOD_CMDBUF_MDU_DATA  DBPOD_RSPBUF_MDU_DATA;
#define BASE_SIZE_DBPOD_RSPBUF_MDU_DATA BASE_SIZE_DBPOD_CMDBUF_MDU_DATA

/********************************************
 *
 * 'Synchronous Video Data' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_SYNC_VIDEO            0x0060
#define DBPOD_RSPCODE_SYNC_VIDEO            0x8060

/* 'Synchronous Video Data' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_SYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_SYNC_VIDEO;

/* 'Synchronous Video Data' response buffer comes in various flavours
 * depending on the format of the video data, but they all start with the
 * same header. */

/* A "dummy" 'Synchronous Video Data' response buffer which is just used as
 * a type-cast to get at the information common to all the video data frame
 * formats. */
typedef struct TAG_DBPOD_RSPBUF_SYNC_VIDEO_DUMMY
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    SHORT       nFormat;            /* Video block format. */
    SHORT       nWidth;             /* Width of frame in pixels. */
    SHORT       nHeight;            /* Height of frame in pixels. */
} DBPOD_RSPBUF_SYNC_VIDEO_DUMMY;

/* An "unknown format" 'Synchronous Video Data' response buffer.
 * nFormat == DBPOD_VID_UNKNOWN_FORMAT (== -1). */
typedef struct TAG_DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    SHORT       nFormat;            /* Video block format. */
    SHORT       nWidth;             /* Width of frame in pixels. */
    SHORT       nHeight;            /* Height of frame in pixels. */
    SHORT       pad1;               /* (padding = 0) */
    UCHAR       bData[DBPOD_ANYLENGTH]; /* Data. */
} DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT;
/* Basic size of DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT without bData[]. */
#define BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT    \
    FIELD_OFFSET(DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT, bData[0])
/* Followed by array UCHAR bData[]. */

/* A "raw 8-bit grey" 'Synchronous Video Data' response buffer.
 * nFormat == DBPOD_VID_RAWGREY8 (== 0). */
typedef struct TAG_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    SHORT       nFormat;            /* Video block format. 0 = 8-bit raw grey. */
    SHORT       nWidth;             /* Width of frame in pixels. */
    SHORT       nHeight;            /* Height of frame in pixels. */
    /* nBitsPerPixel will appear in all the 'raw' frame formats. */
    UCHAR       nBitsPerPixel;      /* Number of bits per pixel (8). */
    UCHAR       bFlags;             /* Flags (see below). */
    UCHAR       bPixels[DBPOD_ANYLENGTH]; /* 8-bit pixel data in TV raster order. */
    /* N.B. the pixel data consists of two half-frame fields which the
     * application should interleave to form a full-frame image.  */
} DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8;
/* Basic size of DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8 without bPixels[]. */
#define BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8  \
    FIELD_OFFSET(DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8, bPixels[0])
/* Followed by array UCHAR bPixels[nWidth*nHeight]. */

/* Video tracking position as a little-endian bitfield structure.
* This also includes a flag to indicate whether the UT data is compressed
* and a flag to indicate whether UT data is 8 or 16 bits.
* Warning: This is non-portable.  Suitable for little-endian Microsoft C. */
typedef struct TAG_DBPOD_VIDTRK
{
    unsigned    uYPos : 10;         /* Video Tracking Y position */
    unsigned    uXPos : 10;         /* Video Tracking X position */
    unsigned    fUTCompressed : 1;  /* Flag indicating UT data is compressed */
    unsigned    fUT16Bit : 1;       /* Flag indicating 16-bit UT data */
    unsigned     : 2;               /* (padding = 0) */
    unsigned    uAmp : 8;           /* Video Tracking dot amplitude */
} DBPOD_VIDTRK;

/* A "raw 8-bit grey" 'Synchronous Video Data' response buffer.
* nFormat == DBPOD_VID_RAWGREY8VT. */
typedef struct TAG_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    SHORT       nFormat;            /* Video block format. 0 = 8-bit raw grey. */
    SHORT       nWidth;             /* Width of frame in pixels. */
    SHORT       nHeight;            /* Height of frame in pixels. */
                                    /* nBitsPerPixel will appear in all the 'raw' frame formats. */
    UCHAR       nBitsPerPixel;      /* Number of bits per pixel (8). */
    UCHAR       bFlags;             /* Flags (see below). */
    union
    {
        DBPOD_VIDTRK    vt;         /* Video tracking info as a bitfield. */
        ULONG           dwVt;       /* Video tracking info as ULONG. */
    };                              /* Anonymous union of 'vt' and 'dwVt'. */
    UCHAR       bPixels[DBPOD_ANYLENGTH]; /* 8-bit pixel data in TV raster order. */
                                          /* N.B. the pixel data consists of two half-frame fields which the
                                          * application should interleave to form a full-frame image.  */
} DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT;
/* Basic size of DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT without bPixels[]. */
#define BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT  \
    FIELD_OFFSET(DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT, bPixels[0])
/* Followed by array UCHAR bPixels[nWidth*nHeight]. */

/*
 * RAWGREY8 and RAWGREY8VT format flags.
 *
 * If DBPOD_VID_FLAG_RAW_TO_BE_INTERLACED is set, the raw data
 * consists of two fields that need interlacing.
 *
 * If DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2 is set, the first line
 * of the image is in the second field, otherwise, the first line
 * of the image is in the first field.  This only applies if the
 * data needs to be interlaced.
 *
 * The start of the second field in the raw data is determined as
 * follows:
 *
 * (a) If the frame height (H) is even, the second field begins
 *     after H/2 rows.
 * (b) If the frame height (H) is odd and the
 *     DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2 flag is set, the second
 *     field begins after (H-1)/2 rows.
 * (c) If the frame height (H) is odd and the
 *     DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2 flag is _not_ set, the
 *     second field begins after (H+1)/2 rows.
 */
#define DBPOD_VID_FLAG_RAW_TO_BE_INTERLACED 0x01
#define DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2  0x02

/********************************************
 *
 * 'Start Asynchronous Video Data' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_START_ASYNC_VIDEO     0x0061
#define DBPOD_RSPCODE_START_ASYNC_VIDEO     0x8061

/* 'Start Asynchronous Video Data' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_START_ASYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_START_ASYNC_VIDEO;

/* 'Start Asynchronous Video Data' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_START_ASYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_START_ASYNC_VIDEO;

/********************************************
 *
 * 'Stop Asynchronous Video Data' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_STOP_ASYNC_VIDEO      0x0062
#define DBPOD_RSPCODE_STOP_ASYNC_VIDEO      0x8062

/* 'Stop Asynchronous Video Data' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_STOP_ASYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_STOP_ASYNC_VIDEO;

/* 'Stop Asynchronous Video Data' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_STOP_ASYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_STOP_ASYNC_VIDEO;

/********************************************
 *
 * 'Asynchronous Video Data' message.
 *
 ********************************************/
#define DBPOD_MSGCODE_ASYNC_VIDEO           0x4060

/* 'Asynchronous Video Data' message buffer is same as 'Synchronous Video Data'
 * response buffer (apart from the message code). */
typedef DBPOD_RSPBUF_SYNC_VIDEO_DUMMY       DBPOD_MSGBUF_ASYNC_VIDEO_DUMMY;
typedef DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT  \
            DBPOD_MSGBUF_ASYNC_VIDEO_UNKNOWN_FORMAT;
typedef DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8    DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8;
#define BASE_SIZE_DBPOD_MSGBUF_ASYNC_VIDEO_UNKNOWN_FORMAT   \
    BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT
#define BASE_SIZE_DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8 \
    BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8

/********************************************
 *
 * Generic UT data message.
 *
 * Used for 'Synchronous Record Data' response, 'Asynchronous Record Data'
 * message, 'Synchronous Display Data' response.
 *
 ********************************************/

/* Interface/Peak format. */
typedef struct TAG_DBPOD_PEAK
{
    LONG    Amplitude;              /* Peak amplitude */
    LONG    Position;               /* Peak position */
} DBPOD_PEAK;

/* 'Generic UT Data' message buffer starts with a header indicating the
 * number of chunks contained in the buffer.  Each chunk comes from a single
 * sequence table entry. */
typedef struct TAG_DBPOD_MSGBUF_UT_SOM
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       nChunks;            /* Number of chunks in message. */
} DBPOD_MSGBUF_UT_SOM;
/* Followed by nChunks variable length chunks. */

/* Each chunk of UT data starts with the following... */
typedef struct TAG_DBPOD_CHUNK_UT_SOC
{
    ULONG       dwLength;           /* Non-inclusive length of chunk (dwSeq onwards). */
    ULONG       dwSeq;              /* Sequence table index */
    LONG        lQuadPos[2];        /* Quadrature readings */
    union
    {
        DBPOD_VIDTRK   vt;              /* Video tracking info as a bitfield. */
        ULONG       dwVt;               /* Video tracking info as ULONG. */
    };                              /* Anonymous union of 'vt' and 'dwVt'. */
    signed char Data[DBPOD_ANYLENGTH]; /* UT data */
} DBPOD_CHUNK_UT_SOC;
/* Basic size of DBPOD_CHUNK_UT_SOC without Data[]. */
#define BASE_SIZE_DBPOD_CHUNK_UT_SOC \
    FIELD_OFFSET(DBPOD_CHUNK_UT_SOC, Data[0])
/* Followed by UT data, 0 to 3 bytes of padding up to a 4-byte boundary,
 * and DBPOD_CHUNK_UT_EOC. */

/* Macros to extract info from 'dwVt'. */
#define DBPOD_VTYPOS(dwVt)  ((USHORT)((dwVt) & 0x3FF))          /* VT Y pos */
#define DBPOD_VTXPOS(dwVt)  ((USHORT)(((dwVt) >> 10) & 0x3FF))  /* VT X pos */
#define DBPOD_VTAMP(dwVt)   ((UCHAR)(((dwVt) >> 24) & 0xFF))    /* VT dot amplitude */
#define DBPOD_VT_UTCOMPRESSED(dwVt) ((unsigned)(((dwVt) >> 20) & 1)) /* UT data compressed */
#define DBPOD_VT_UT16BIT(dwVt)      ((unsigned)(((dwVt) >> 21) & 1)) /* UT data 16-bit */

/* Each chunk of UT data ends with the following, starting on a 4-byte
 * boundary... */
/*
 * Note: if DBPOD_START_UT_EXTRA_ENCS was set in the dwFlags member of
 * DBPOD_CMDBUF_START_UT when the starting UT acquisition, the final elements
 * of Peak[] may be replaced with readings from the extra quadrature encoders
 * that don't fit in the lQuadPos[] member of DBPOD_CHUNK_UT_SOC.
 */
typedef struct TAG_DBPOD_CHUNK_UT_EOC
{
    LONG        fLossSig;           /* Loss of signal flag */
    DBPOD_PEAK Interface;           /* Interface gate */
    DBPOD_PEAK Peak[16];            /* Detected peaks (unused peaks are zero) */
} DBPOD_CHUNK_UT_EOC;

/*
 * Example UT data message with two chunks:
 *
 * Offset  Length       Field                       Value
 * ------  ------       -----                       -----
 *      0       4       hdr.dwLength                1888 (= 1892 - 4)
 *      4       4       hdr.dwSequence              ?
 *      8       2       hdr.wCmd                    ?
 *     10       2       hdr.wSubCode                0
 *
 *     12       4       nChunks                     2
 *
 * (first chunk soc)
 *     16       4       dwLength                    1180 (= 1200 - 16 - 4)
 *     20       4       dwSeq                       ?
 *     24       4       lQuadPos[0]                 ?
 *     28       4       lQuadPos[1]                 ?
 *     32       4       dwVt or vt                  ?
 * (first chunk data)
 *     36    1024       Data[] (length 1024)        ?
 * (first chunk eoc)
 *   1060       4       fLossSig                    ?
 *   1064       4       Interface.Amplitude         ?
 *   1068       4       Interface.Position          ?
 *   1072       4       Peak[0].Amplitude           ?
 *   1076       4       Peak[0].Position            ?
 *   ...
 *   1192       4       Peak[15].Amplitude          ?
 *   1196       4       Peak[15].Position           ?
 *
 * (second chunk)
 *   1200       4       dwLength                    688 (= 1892 - 1200 - 4)
 *   1204     688       -- remainder of second chunk --
 *
 *   1892       0       -- this is the end of the message --
 */

/********************************************
 *
 * 'Synchronous Record Data' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_SYNC_RECORD           0x0070
#define DBPOD_RSPCODE_SYNC_RECORD           0x8070

/* 'Synchronous Record Data' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_SYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       dwBufferSize;       /* Preferred buffer size in bytes. */
} DBPOD_CMDBUF_SYNC_RECORD;

/* 'Synchronous Record Data' response buffer is same as 'Generic UT Data'
 * message buffer. */
typedef DBPOD_MSGBUF_UT_SOM     UPLUS_RSPBUF_SYNC_RECORD_SOM;

/********************************************
 *
 * 'Start Asynchronous Record Data' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_START_ASYNC_RECORD    0x0071
#define DBPOD_RSPCODE_START_ASYNC_RECORD    0x8071

/* 'Start Asynchronous Record Data' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_START_ASYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       dwBufferSize;       /* Preferred buffer size in bytes. */
} DBPOD_CMDBUF_START_ASYNC_RECORD;

/* 'Start Asynchronous Record Data' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_START_ASYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_START_ASYNC_RECORD;

/********************************************
 *
 * 'Stop Asynchronous Record Data' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_STOP_ASYNC_RECORD     0x0072
#define DBPOD_RSPCODE_STOP_ASYNC_RECORD     0x8072

/* 'Stop Asynchronous RECORD Data' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_STOP_ASYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_STOP_ASYNC_RECORD;

/* 'Stop Asynchronous RECORD Data' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_STOP_ASYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_STOP_ASYNC_RECORD;

/********************************************
 *
 * 'Asynchronous Record Data' message.
 *
 ********************************************/
#define DBPOD_MSGCODE_ASYNC_RECORD          0x4070

/* 'Asynchronous Record Data' message buffer is same as 'Generic UT Data'
 * message buffer. */
typedef DBPOD_MSGBUF_UT_SOM   UPLUS_MSGBUF_ASYNC_RECORD_SOM;

/********************************************
 *
 * 'Synchronous Display Data' command and response.
 *
 * Gets the latest, previously unsent data for a specific sequence table
 * entry.  The response will contain at most one chunk.
 *
 ********************************************/
#define DBPOD_CMDCODE_SYNC_DISPLAY          0x0080
#define DBPOD_RSPCODE_SYNC_DISPLAY          0x8080

/* 'Synchronous Display Data' command buffer. */
typedef struct TAG_DBPOD_CMDBUF_SYNC_DISPLAY
{
    DBPOD_MSGHDR hdr;               /* Message header. */
    ULONG       dwSeq;              /* Sequence table index. */
} DBPOD_CMDBUF_SYNC_DISPLAY;

/* 'Synchronous Display Data' response buffer is same as 'Generic UT Data'
 * message buffer and will contain at most one chunk. */
typedef DBPOD_MSGBUF_UT_SOM     UPLUS_RSPBUF_SYNC_DISPLAY_SOM;

/********************************************
 *
 * 'Asynchronous Error' message.
 *
 ********************************************/
#define DBPOD_MSGCODE_ASYNC_ERROR           0x4100

/* 'Asynchronous Error' message buffer is just a message header. */
typedef struct TAG_DBPOD_MSGBUF_ASYNC_ERROR
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_MSGBUF_ASYNC_ERROR;

/********************************************
 *
 * 'Abort' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_ABORT                 0x0101
#define DBPOD_RSPCODE_ABORT                 0x8101

/* 'Abort' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_ABORT
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_ABORT;

/* 'Abort' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_ABORT
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_ABORT;

/********************************************
 *
 * 'Soft Reset' command and response.
 *
 ********************************************/
#define DBPOD_CMDCODE_SOFT_RESET            0x0102
#define DBPOD_RSPCODE_SOFT_RESET            0x8102

/* 'Soft Reset' command buffer is just a message header. */
typedef struct TAG_DBPOD_CMDBUF_SOFT_RESET
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_CMDBUF_SOFT_RESET;

/* 'Soft Reset' response buffer is just a message header. */
typedef struct TAG_DBPOD_RSPBUF_SOFT_RESET
{
    DBPOD_MSGHDR hdr;               /* Message header. */
} DBPOD_RSPBUF_SOFT_RESET;

/* Restore warnings to previous setting */
#ifdef _MSC_VER
#pragma warning (pop)
#endif /* _MSC_VER */

/* Restore packing to previous setting */
#if defined(_MSC_VER) || defined(__GNUC__)
//#pragma pack(pop)
#endif /* _MSC_VER */

#if defined(__cplusplus)
}
#endif

/* vi: set ai et sw=4 ts=4: */
#endif
