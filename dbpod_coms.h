#ifndef DBPOD_COMS_H__INCLUDED
#define DBPOD_COMS_H__INCLUDED

/*
 * $Id$
 *
 * db-Pod Communications Messages
 *
 * Copyright (c) 2008-2026 MEV Ltd., Bell Technology Ltd.
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

/**
 * \defgroup dbpod_coms dbPOD Message Protocol
 * \brief Message structures for communication between host PC and dbPOD
 * device over a TCP connection.
 *
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
 *    Type      | Description
 *    :-------- | :----------
 *    \c CHAR   | plain 8-bit character
 *    \c INT8   | signed 8-bit integer
 *    \c UCHAR  | unsigned 8-bit integer
 *    \c SHORT  | signed 16-bit integer
 *    \c USHORT | unsigned 16-bit integer
 *    \c LONG   | signed 32-bit integer
 *    \c ULONG  | unsigned 32-bit integer
 *
 * 5. The \c FILETIME type is defined elsewhere as follows:
 *
 *    \code
 *    typedef struct _FILETIME
 *    {
 *        DWORD dwLowDateTime;
 *        DWORD dwHighDateTime;
 *    };
 *    \endcode
 *
 *    where \c DWORD is also an unsigned 32-bit integer.  Together,
 *    \c dwLowDateTime and \c dwHighDateTime form the lower and upper 32-bits
 *    of a 64-bit unsigned value representing the number of 100 ns intervals
 *    since January 1, 1601 (UTC).
 *
 * 6. The \c FIELD_OFFSET() macro (defined elsewhere) is similar to the C
 *    standard \c offsetof() macro except that the result has type \c LONG.
 *
 * 7. All padding fields in command and response messages are reserved for
 *    future use and should be set to 0.
 *
 * \{
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
/** \brief Zero-length array for Microsoft C or GNU C. */
#define DBPOD_ANYLENGTH 0
#else
/** \brief Zero-length array is non-standard, so use 1. */
#define DBPOD_ANYLENGTH 1
#endif

/**
 * \brief Current packet version.
 *
 * Upper 16 bits is major version, lower 16 bits is minor version.
 *
 * Various incompatible changes occurred during development in packet
 * version 0x30000.
 * \n
 *
 * ---------------------------------------------------------------------------
 * \n
 *
 * <b>Packet version 0x40000:</b>
 *
 * 1. \ref DBPOD_CMDBUF_MDU_CONFIG has the <tt>UCHAR nPort</tt> field
 *    replacing the unused <tt>CHAR pad1</tt> field.
 *
 * 2. \ref DBPOD_CMDBUF_MDU_DATA has the <tt>USHORT wLength</tt>,
 *    <tt>UCHAR nPort</tt>, and <tt>CHAR fContinuation</tt> fields replacing
 *    the <tt>ULONG dwLength</tt> field.
 *
 * ---------------------------------------------------------------------------
 * \n
 *
 * <b>Packet version 0x40001:</b>
 *
 * 1. \ref DBPOD_RSPBUF_GET_CAPABILITIES has the <tt>USHORT nChanConfigs</tt>
 *    field replacing the unused <tt>SHORT pad2</tt> field.  This holds the
 *    number of channel configurations supported by the pod.
 *
 * ---------------------------------------------------------------------------
 * \n
 *
 * <b>Packet version 0x40002:</b>
 *
 * 1. \ref DBPOD_RSPBUF_GET_CAPABILITIES has the following changes which
 *    should be backwards compatible:
 *
 *    - The \c dwRFFScaleFreq field has been renamed to \c dwFiltScaleFreq.
 *      It now either applies to, or does not apply to, the LPF, HPF and RFF
 *      filter breakpoints, depending on the other fields defined below.
 *
 *    - The <tt>LONG fVideoTracking</tt> field has been split into
 *      <tt>CHAR fVideoTracking</tt>, <tt>CHAR fScaleLPF</tt>,
 *      <tt>CHAR ScaleHPF</tt>, and <tt>CHAR fScaleRFF</tt> fields.
 *      \c fVideoTracking was previously set to \c TRUE.
 *
 *      - If \c fScaleLPF is \c FALSE, the low-pass filter breakpoint
 *        frequencies in <tt>anLPF[]</tt> are absolute (except that a value
 *        of 0 means "broadband").  If \c fScaleLPF is \c TRUE, the values in
 *        <tt>anLPF[]</tt> need to be multiplied by the digitization
 *        frequency (\c nSampleFreq in the channel configuration) and divided
 *        by the \c dwFiltScaleFreq value.
 *
 *      - If \c fScaleHPF' is \c FALSE, the high-pass filter breakpoint
 *        frequencies in <tt>anHPF[]</tt> are absolute.  If \c fScaleHPF is
 *        \c TRUE, the values in <tt>anHPF[]</tt> need to be multiplied by
 *        the digitization frequency (\c nSampleFreq in the channel
 *        configuration) and divided by the \c dwFiltScaleFreq value.
 *
 *      - If \c fScaleRFF is \c FALSE, the post-rectification filter
 *        breakpoint frequencies in <tt>anRFF[]</tt> are absolute.  If
 *        \c fScaleHPF is \c TRUE, the values in <tt>anRFF[]</tt> need to be
 *        multiplied by the digitization frequency (\c nSampleFreq in the
 *        channel configuration) and divided by the \c dwFiltScaleFreq value.
 *    .
 *    \note See changes in packet version 0x50000 that affect how \c
 *    dwFiltScaleFreq is used.
 *
 * 2. \ref DBPOD_CMDBUF_CHAN_CONFIG has the <tt>USHORT wCompatLevel</tt>
 *    field replacing the <tt>SHORT pad2</tt> field.
 *
 *    - If \c wCompatLevel is 0 (old application), the \c nHPF, \c nLPF, and
 *      \c nRFF values will be matched against supported values listed in the
 *      capabilities as-is, with no scaling, regardless of the \c fScaleHPF,
 *      \c fScaleLPF, and \c fScaleRFF flags in the capabilities.  This may
 *      result in the filter cut-off frequencies that are actually applied
 *      being different from the specified frequencies.
 *
 *    - If \c wCompatLevel is 1 (new application), the \c nHPF, \c nLPF, and
 *      \c nRFF values will be matched against unscaled or scaled (depending
 *      on the \c fScaleHPF, \c fScaleLPF, and \c fScaleRFF settings in the
 *      capabilities) versions of the supported values listed in the
 *      capabilities.
 *    .
 *    \note
 *    See changes in packet version 0x50000 for additional \c wCompatLevel
 *    value.
 *
 * &zwnj;
 *
 * ---------------------------------------------------------------------------
 * \n
 *
 * <b>Packet version 0x40003:</b>
 *
 * 1. \ref DBPOD_CMDBUF_CHAN_CONFIG has the <tt>USHORT wSpecial</tt> field
 *    replacing the <tt>SHORT pad3</tt> field.  This is for setting a
 *    hardware-specific configuration value, e.g. for setting input impedance
 *    and pre-amplification.
 *
 * 2. \ref DBPOD_RSPBUF_GET_CAPABILITIES has the <tt>UCHAR bHardType</tt> and
 *    <tt>UCHAR bAFEType</tt> fields replacing the <tt>SHORT pad3</tt> field:
 *
 *    \c bHardType values:
 *    \n
 *    Val | Symbol                     | Description
 *    :-: | :------------------------: | :-------------------------------
 *     0  | \c DBPOD_HARDTYPE_ORIG_ETH | Classic Ethernet Pod (Nios II)
 *     1  | \c DBPOD_HARDTYPE_FAST_ETH | "Fast" Ethernet Pod (ARM)
 *     2  | \c DBPOD_HARDTYPE_MINI_ETH | Ethernet Mini Pod (ARM)
 *     3  | \c DBPOD_HARDTYPE_MINI_USB | USB3.0 Mini Pod (FTDI)
 *
 *    \n \c bAFEType values:
 *    \n
 *    Val | Symbol                     | Description
 *    :-: | :------------------------: | :-------------------------------
 *     0  | \c DBPOD_AFETYPE_ORIGINAL  | Classic Ethernet Pod or Fast Pod
 *     1  | \c DBPOD_AFETYPE_MAX2077   | MAX2077 -- MiniPod (Ether or USB)
 *
 * ---------------------------------------------------------------------------
 * \n
 *
 * <b>Packet version 0x40004:</b>
 *
 * Packet version 0x40004 has changes to the encoder configuration command
 * for configuring external resets, and supports a "maximum" mode for the
 * average gate, a "sorted peaks" mode for the peaks gate, and a new scan
 * type for use with "maximum" mode.  There is also an option for including
 * readings from all four quadrature encoders in the UT data, replacing some
 * of the peaks in the message.
 *
 * 1. \ref DBPOD_CMDBUF_ENCODER_CONFIG has the <tt>USHORT wDifferential</tt>,
 *    <tt>USHORT wResetEnable</tt>, <tt>USHORT wInvertSense</tt>, and
 *    <tt>USHORT wResetSource</tt> fields replacing the
 *    <tt>ULONG dwDifferential</tt> and <tt>ULONG dwInvertSense</tt> fields,
 *    and is the same size as before.
 *
 * 2. For channel configuration commands, the "averaging" gate supports a new
 *    "maximum" mode.  This produces the extreme values (those with largest
 *    absolute value) at each sample position from a set of passes instead of
 *    the usual "mean" value.  The "maximum" mode is enabled by setting the
 *    \c nControl field of the averaging gate to \c DBPOD_AVERAGING_MAX.
 *
 *    Averaging gate \c nControl values:
 *    \n
 *    Val | Symbol                  | Description
 *    :-: | :---------------------: | :-------------------------------
 *     0  | \c DBPOD_AVERAGING_MEAN | produces the mean values
 *     1  | \c DBPOD_AVERAGING_MAX  | <b>(new)</b> produces the maximum (extreme) values
 *
 *    \n "Maximum" mode works for Ethernet MiniPod FPGA version 7 or later
 *    (as reported by the \c wHardVersion fields of the 'Get Capabilities'
 *    response) and for all(?) versions of the USB MiniPod.
 *
 * 3. Also for channel configuration commands, the "peaks" gate supports a
 *    new "sorted peaks" mode.  This sorts the peaks in descending order of
 *    size.  The "sorted peaks" mode is enabled by setting the peak type in
 *    bits 15 to 8 of the \c nControl field of the peaks gate to \c
 *    DBPOD_PEAKTYPE_SORTED.  Note that bits 7 to 0 of the \c nControl field
 *    sets the number of peaks required, but this is currently ignored.
 *
 *    Peak gate \c nControl values \ref DBPOD_PEAKS_NCONTROL(num, type):
 *    \n
 *    Param   | Description
 *    :-----: | :----------------------------------
 *    \c num  | number of peaks (currently ignored)
 *    \c type | peak type
 *
 *    \n Peak type values (bits 15 to 8 of \c nControl):
 *    \n
 *    Val | Symbol                        | Description
 *    :-: | :---------------------------: | :-------------------------------
 *     0  | \c DBPOD_PEAKTYPE_FIRST_N_POS | positive peaks
 *     1  | \c DBPOD_PEAKTYPE_FIRST_N     | bipolar peaks
 *     2  | \c DBPOD_PEAKTYPE_SORTED      | <b>(new)</b> sorted peaks
 *
 *    \n (Note that current hardware always produces bipolar peaks and the
 *    "number of peaks" value is ignored.)
 *
 *    "Sorted peaks" mode works for Ethernet MiniPod FPGA version 7 or
 *    later (as reported by the \c wHardVersion field of the 'Get
 *    Capabilities' response) and for all(?) versions of the USB MiniPod.
 *
 * 4. The \c ScanType field of \ref DBPOD_CMDBUF_SCAN_CONFIG supports an
 *    additional value, \c DBPOD_SCAN_FREEPOSITION.  This is like free-run
 *    mode, but changes in scan position may have additional effects.  This
 *    is for use with the new "maximum" UT mode, where it results in
 *    production of maximised UT data from all sweeps since the previous
 *    change in scan position.
 *
 *    Scan configuration \c ScanType values:
 *    \n
 *    Val | Symbol                       | Description
 *    :-: | :--------------------------: | :-------------------------------
 *     0  | \c DBPOD_SCAN_FREERUN        | free-run mode
 *     1  | \c DBPOD_SCAN_TIEDTOPOSITION | tied-to-position mode
 *     2  | \c DBPOD_SCAN_FREEPOSITION   | <b>(new)</b> "free position" mode
 *
 *    \n "Free position" mode works for Ethernet MiniPod FPGA version 7 or
 *    later (as reported by the \c wHardVersion field of the 'Get
 *    Capabilities' response) and for all(?) versions of the USB MiniPod.
 *
 * 5. The \c dwFlags field of \ref DBPOD_CMDBUF_START_UT has an additional
 *    bit-mask flag value, \c DBPOD_START_UT_EXTRA_ENCS.  <b>OR</b>-ing \c
 *    DBPOD_START_UT_EXTRA into \c dwFlags has the effect of making encoder
 *    readings beyond encoders 0 and 1 appearing at the end of the UT data in
 *    place of some of the peaks.
 *
 *    \c dwFlags' bit-mask values:
 *    \n
 *    Bit-mask | Symbol                       | Description
 *    :------: | :--------------------------: | :----------------------------
 *     0x0001  | \c DBPOD_START_UT_DESPARKLE  | turn on desparkler
 *     0x0002  | \c DBPOD_START_UT_COMPRESS   | turn on compression
 *     0x0004  | \c DBPOD_START_UT_EXTRA_ENCS | <b>(new)</b> extra encoders in UT
 *
 *    \note The "compress" option is reserved for future use.  The
 *    "desparkle" option is a legacy option which may have no effect on
 *    current hardware.)
 *
 *    This works for Ethernet MiniPod FPGA version 7 or later (as reported by
 *    the \c wHardVersion field of the 'Get Capabilities' response) and for
 *    all(?) versions of the USB MiniPod.
 *
 * ---------------------------------------------------------------------------
 * \n
 *
 * <b>Packet version 0x50000:</b>
 *
 * 1. \ref DBPOD_RSPBUF_GET_CAPABILITIES has the <tt>UCHAR bPAType</tt> field
 *    replacing the first half of the <tt>SHORT pad4</tt> field, which is now
 *    <tt>UCHAR pad4</tt>. There is also an additional value 4 for the
 *    \c bHardType field, and additional value 2 for the \c bAFEType field.
 *
 *    \c bHardType values:
 *    \n
 *    Val | Symbol                       | Description
 *    :-: | :--------------------------: | :-------------------------------
 *     0  | \c DBPOD_HARDTYPE_ORIG_ETH   | Classic Ethernet Pod (Nios II)
 *     1  | \c DBPOD_HARDTYPE_FAST_ETH   | "Fast" Ethernet Pod (ARM)
 *     2  | \c DBPOD_HARDTYPE_MINI_ETH   | Ethernet Mini Pod (ARM)
 *     3  | \c DBPOD_HARDTYPE_MINI_USB   | USB3.0 Mini Pod (FTDI)
 *     4  | \c DBPOD_HARDTYPE_MINIPA_ETH | Ethernet Mini Phased Array (ARM)
 *
 *    \n \c bAFEType values:
 *    \n
 *    Val | Symbol                     | Description
 *    :-: | :------------------------: | :-------------------------------
 *     0  | \c DBPOD_AFETYPE_ORIGINAL  | Classic Ethernet Pod or Fast Pod
 *     1  | \c DBPOD_AFETYPE_MAX2077   | MAX2077 -- MiniPod (Ether or USB)
 *     2  | \c DBPOD_AFETYPE_VCA5807   | VCA5807 -- Mini Phased Array
 *
 *    \n \c bPAType values:
 *    \n
 *    Val | Symbol                   | Description
 *    :-: | :----------------------: | :-------------------------------
 *     0  | \c DBPOD_PATYPE_NONE     | No phased array
 *     1  | \c DBPOD_PATYPE_MINI_PBF | MiniPod Phased Array partial beam former
 *    \n
 *
 * 2. There is an extended version of the 'Get Capabilities' command using
 *    the same command code, but including a <tt>ULONG dwMaxCapLength</tt>
 *    field.  If this field is included in the 'Get Capabilities' command, it
 *    specifies the maximum length of the 'Get Capabilities' response, not
 *    including the message header (i.e., it specifies the maximum length
 *    from the start of the \c szHwName field onwards in \ref
 *    DBPOD_RSPBUF_GET_CAPABILITIES).  However, \c dwMaxCapLength values less
 *    than 512 will be increased to 512 for backwards compatibility.
 *
 *    All capability fields from \c szHwName to \c dwMaxPRF inclusive
 *    (corresponding to a \c dwMaxCapLength value of 512) will be included in
 *    the 'Get Capabilities' response regardless of the presence of the \c
 *    dwMaxCapLength field or its value.  Higher values of the \c
 *    dwMaxCapLength field allow additional capability fields to be included
 *    in the response.
 *
 * 3. The extended 'Get Capabilities' response includes the following
 *    additional fields if the \c dwMaxCapLength value in the command is high
 *    enough:
 *
 *    - <tt>USHORT wScaleMaskLPF</tt> is a bit-mask indicating which of the
 *      low-pass filter breakpoint frequencies in <tt>anLPF[]</tt> are
 *      scaled and which are absolute.  This is more informative than the
 *      \c fScaleLPF field.  A '1' bit at a bit-position in the mask
 *      indicates that the entry at the corresponding index in
 *      <tt>anLPF[]</tt> needs to be multiplied by the digitization
 *      frequency (\c nSampleFreq in the channel configuration) and divided
 *      by the \c dwFiltScaleFreq value.  A '0' bit at a bit-position in the
 *      mask indicates that the entry at the corresponding index in
 *      <tt>anLPF[]</tt> is absolute (except that a value of 0 means
 *      "broadband").  The value is only taken into account during channel
 *      configuration if the \c wCompatLevel value in the channel
 *      configuration is at least 2.
 *
 *    - <tt>USHORT wScaleMaskHPF</tt> is a bit-mask indicating which of the
 *      high-pass filter breakpoint frequencies in <tt>anHPF[]</tt> are
 *      scaled and which are absolute.  This is more informative than the
 *      \c fScaleHPF field.  A '1' bit at a bit-position in the mask
 *      indicates that the entry at the corresponding index in
 *      <tt>anHPF[]</tt> needs to be multiplied by the digitization
 *      frequency (\c nSampleFreq in the channel configuration) and divided
 *      by the \c dwFiltScaleFreq value.  A '0' bit at a bit-position in the
 *      mask indicates that the entry at the corresponding index in
 *      <tt>anHPF[]</tt> is absolute.  The value is only taken into account
 *      during channel configuration if the \c wCompatLevel value in the
 *      channel configuration is at least 2.
 *
 *    - <tt>USHORT wScaleMaskRFF</tt> is a bit-mask indicating which of the
 *      post-rectification filter breakpoint frequencies in <tt>anRFF[]</tt>
 *      are scaled and which are absolute.  This is more informative
 *      than the \c fScaleRFF field.  A '1' bit at a bit-position in the
 *      mask indicates that the entry at the corresponding index in
 *      <tt>anRFF[]</tt> needs to be multiplied by the digitization
 *      frequency (\c nSampleFreq in the channel configuration) and divided
 *      by the \c dwFiltScaleFreq value.  A '0' bit at a bit-position in the
 *      mask indicates that the entry at the corresponding index in
 *      <tt>anRFF[]</tt> is absolute.  The value is only taken into account
 *      during channel configuration if the \c wCompatLevel value in the
 *      channel configuration is at least 2.
 *
 *    - <tt>USHORT wMaxAperture</tt> is the maximum number of parallel ADCs
 *      for phased array.
 *
 *    - <tt>USHORT wMaxElements</tt> is the maximum number of phased array
 *      elements.
 *
 *    - <tt>USHORT wTxFocusGranularity</tt> is the TX focus delay granularity
 *      (TX focus step size) in nanoseconds for phased array.
 *
 *    - <tt>USHORT wRxFocusGranularity</tt> is the RX focus delay granularity
 *      (RX focus step size) in nanoseconds for phased array.
 *
 *    - <tt>USHORT wTxMaxFocusSteps</tt> is the maximum number of TX focus
 *      delay steps for phased array.  So the maximum TX focus delay in
 *      nanoseconds is \c wTxMaxFocusSteps \c &times; \c wTxFocusGranularity.
 *
 *    - <tt>USHORT wRxMaxFocusSteps</tt> is the maximum number of RX focus
 *      delay steps for phased array.  So the maximum RX focus delay
 *      in nanoseconds is \c wRxMaxFocusSteps &times; \c wRxFocusGranularity.
 *
 *    - <tt>SHORT nMinBalanceGain</tt> is the minimum phased array element
 *      balancing gain in millibels.  It is the minimum for both the global
 *      element balancing gain and (since packet version 0x50001) the sequence
 *      table specific element gains, and is the minimum for the sum of those
 *      two gains.
 *
 *    - <tt>SHORT nMaxBalanceGain</tt> is the maximum phased array element
 *      balancing gain in millibels.  It is the maximum for both the global
 *      element balancing gain and (since packet version 0x50001) the sequence
 *      table specific element gains, and is the maximum for the sum of those
 *      two gains.
 *
 *    - <tt>SHORT nBalanceGainRes</tt> is the nominal granularity of the the
 *      phased array balancing gain in millibels at a gain of &minus;1000
 *      millibels.  It applies to the global element balancing gain and (since
 *      packet version 0x50001) the sequence table specific element gains.
 *      The reference point of &minus;1000 millibels is for the sum of those
 *      two gains.  If the phased array element gains are linear in the
 *      hardware implementation (as they currently are), the actual
 *      granularity in millibels would become exponentially larger at lower
 *      gains, eventually becoming infinite when the linear gain used by the
 *      hardware becomes zero (negative infinity millibels).
 *
 * 4. The interpretation of the \c dwFiltScaleFreq field value (in \ref
 *    DBPOD_RSPBUF_GET_CAPABILITIES) and its effect on the scaling of the
 *    filter frequencies has changed when the value of \c dwFiltScaleFreq is
 *    less than the maximum digitization rate in <tt>adwDigFreq[0]</tt>.
 *    Currently, this only affects Mini Phased Array devices because the
 *    MiniPod and FastPod have \c dwFiltScaleFreq equal to
 *    <tt>adwDigFreq[0]</tt>.  However, the interpretation of the new \c
 *    wScaleMaskLPF, \c wScaleMaskHPF, and \c wScaleMaskRFF capabilities also
 *    affects the MiniPod devices.
 *
 *    The scaling of filter frequencies will be disabled for channels that
 *    have digitization rate (\c nSampleFreq in the channel configuration)
 *    set higher than (or equal to) the \c dwFiltScaleFreq value.
 *
 *    For backwards compatibility, when checking the configured filter
 *    frequencies in the channel configuration command, the pod software will
 *    only apply the "\c nSampleFreq higher than \c dwFiltScaleFreq" rule,
 *    and will only use the \c wScaleMaskHPF, \c wScaleMaskLPF, and \c
 *    wScaleMaskRFF bit-mask values in the capabilities when the \c
 *    wCompatLevel value  in the channel configuration is at least 2.
 *
 *    - If \c wCompatLevel is 0 (old application), the \c nHPF, \c nLPF, and
 *      \c nRFF values will be matched against supported values listed in the
 *      capabilities as-is, with no scaling.  The \c fScaleHPF, \c fScaleLPF,
 *      and \c fScaleRFF flags, and the \c wScaleMaskHPF, \c wScaleMaskLPF,
 *      and \c wScaleMaskRFF bit-mask values are ignored.  This may result in
 *      the filter cut-off frequencies that are actually applied being
 *      different from the specified frequencies.
 *
 *    - If \c wCompatLevel is 1 (less old application), the \c nHPF, \c nLPF,
 *      and \c nRFF values will be matched against unscaled or scaled
 *      versions of the supported values listed in the capabilities.  It is
 *      assumed that the client is aware of the \c fScaleHPF, \c fScaleLPF,
 *      and \c fScaleRFF flags in the capabilities, but is unaware of the
 *      \c wScaleMaskHPF, \c wScaleMaskLPF, and \c wScaleMaskRFF bit-mask
 *      values in the capabiilities, and is also unaware that scaling should
 *      only be applied for digitization rates higher than the \c
 *      dwFiltScaleFreq value in the capabilities.  It is assumed that the
 *      client has set the \c nHPF value in the channel configuration message
 *      to match a scaled <tt>anHPF[]</tt> entry if the \c fScaleHPF value in
 *      the capabilities is \c TRUE, regardless of the \c wScaleMaskHPF
 *      bit-mask value in the capabilities, and regardless of whether the
 *      digitization rate (\c nSampleFreq in the channel configuration) is
 *      lower or higher than the \c dwFiltScaleFreq value in the
 *      capabilities.  Similarly, it is assumed that the client set the \c
 *      nLPF value in the channel configuration to match a scaled
 *      <tt>anLPF[]</tt> entry if the \c fScaleLPF value in the capabilities
 *      is \c TRUE, regardless of the \c wScaleMaskLPF value and the
 *      digitization rate, and it is assumed that the client has set the \c
 *      nRFF value in the channel configuration to match a scaled
 *      <tt>anRFF[]</tt> entry if the \c fScaleRFF value in the capabilities
 *      is \c TRUE, regardless of the \c wScaleMaskRFF value and the
 *      digitization rate).  This may result in the filter cut-off
 *      frequencies that are actually applied being different from the
 *      specified frequencies when the digitization rate is higher than the
 *      \c dwFiltScaleFreq value or when only some of the supported filter
 *      cut-off frequencies are actually scaled.
 *
 *    - If \c wCompatLevel is 2 (new application), the \c nHPF, \c nLPF, and
 *      \c nRFF values will be matched against unscaled or scaled versions of
 *      the supported values listed in the capabilities.  It is assumed that
 *      the client is aware of the \c wScaleMaskHPF, \c wScaleMaskLPF, and
 *      \c wScaleMaskRFF bit-mask capabilities, and is aware of the rule that
 *      scaling should be disabled when the digitization rate (\c nSampleFreq
 *      in the channel configuration) is higher than the \c dwFiltScaleFreq
 *      value in the capabilities.  It is assumed that the client has set the
 *      \c nHPF value in the channel configuration message to match a scaled
 *      <tt>anHPF[]</tt> entry if and only if the corresponding bit of the
 *      \c wScaleMaskHPF value in the capabilities is '1' and the
 *      digitization rate (\c nSampleFreq in the channel configuration) is
 *      lower than (or equal to) the \c dwFiltScaleFreq value in the
 *      capabilities.  Similarly, it is assumed that the client set the \c
 *      nLPF value in the channel configuration to match a scaled
 *      <tt>anLPF[]</tt> entry if and only if the corresponding bit of the
 *      \c wScaleMaskLPF value in the capabilities is '1' and the
 *      digitization rate is lower than (or equal to) the \c dwFiltScaleFreq
 *      value, and has set the \c nRFF value in the channel configuration to
 *      match a scaled <tt>anRFF[]</tt> entry if and only if the
 *      corresponding bit of the \c wScaleMaskRFF value in the capabilities
 *      is '1' and the digitization rate is lower than (or equal to) the
 *      \c dwFiltScaleFreq value.
 *
 * 5. Added 'Phased Array Channel Element/Delay/Gain' command and response
 *    (\ref DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY and \ref
 *    DBPOD_RSPBUF_PA_CHAN_ELEM_DELAY).  This command configures various
 *    parts of the phased array as determined by the value of the \c
 *    bTypeMask field, and may be used several times to configure different
 *    parts.
 *
 *    Some parts are specific to a single sequence table entry, specified by
 *    the \c nIndex field, and some parts are global.  The \c nIndex field
 *    can be set to &minus;1 to apply sequence table entry specific parts to
 *    all sequence table entries.  The \c nIndex field value is ignored when
 *    configuring global parts.
 *
 *    If the values for a particular type are identical (e.g. identical lists
 *    of TX and RX element numbers for a sequence table entry) then they can
 *    be configured in a single message by setting the \c bTypeMask field to
 *    a combination of bits.
 *
 *    The sequence table entry specific parts are the list of TX element
 *    numbers, the list of RX element numbers, the list of TX element delays,
 *    and the list of RX element delays.  The pod software will impose
 *    restrictions on the sets of element numbers that can be included in the
 *    list due to hardware limitations (e.g. no gaps in the element numbers
 *    in the list).
 *
 *    The global parts are a list containing the balancing gain for each
 *    physical element, and a list containing the balancing delay for each
 *    physical element.
 *
 * 6. The \c nTx and \c nRx fields in the 'Channel Configuration' command
 *    structure (\ref DBPOD_CMDBUF_CHAN_CONFIG) can be set to the special
 *    value &minus;2 to specify that the sequence table entry will use the
 *    phased array elements.
 *
 * 7. The \c dwFlags field of \ref DBPOD_CMDBUF_START_UT has a group of bits
 *    to specify how phased array elements should be mapped:
 *
 *    Additional \c dwFlags bit-mask values:
 *    \n
 *    Bit-mask   | Symbol                                 | Description
 *    :--------: | :------------------------------------: | :-----------
 *    0x00000000 | \c DBPOD_START_UT_PE_MAP_DEFAULT       | use default mapping
 *    0x01000000 | \c DBPOD_START_UT_PE_MAP_NATIVE        | use native mapping
 *    0x02000000 | \c DBPOD_START_UT_PE_MAP_ODD_EVEN_SWAP | swap odd/even
 *    0xFF000000 | \c DBPOD_START_UT_PE_MAP_MASK          | mask for values above
 *
 *    \n The \c dwFlags value <b>AND</b>-ed with \c DBPOD_START_UT_PE_MAP_MASK
 *    should match one of the other values in the above table.  They can be
 *    combined with unrelated \c dwFlags bits outside the mask.
 *
 *    For Mini Phased Array, the default mapping (\c
 *    DBPOD_START_UT_PE_MAP_DEFAULT) does the same thing as the swap odd/even
 *    mapping (\c DBPOD_START_UT_PE_MAP_ODD_EVEN_SWAP).
 *
 * ---------------------------------------------------------------------------
 * \n
 *
 * <b>Packet version 0x50001:</b>
 *
 * 1. The \c bTypeMask field of \ref DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY supports
 *    an additional bit-mask value \c DBPOD_PAMASK_RX_GAIN.  This is used to
 *    configure a list of sequence table entry specific, per-element gains for
 *    the sequence table entry specified by the \c nIndex field.  The list can
 *    be applied to all sequence table entries by setting \c nIndex to
 *    &minus;1.
 *
 *    Each element of the list will set the gain for the corresponding
 *    RX element in the list of RX element numbers for the aperture.
 *    This will get added to the global balancing gain for the element.
 *
 *    \c bTypeMask values:
 *    \n
 *    Bit-mask | Symbol                 | Description
 *    :------: | :--------------------: | :-----------
 *      0x01   | DBPOD_PAMASK_TX_ELEM   | TX element numbers
 *      0x02   | DBPOD_PAMASK_RX_ELEM   | RX element numbers
 *      0x04   | DBPOD_PAMASK_TX_DELAY  | TX delays (nanoseconds)
 *      0x08   | DBPOD_PAMASK_RX_DELAY  | RX delays (nanoseconds)
 *      0x10   | DBPOD_PAMASK_BAL_GAIN  | Balancing gains (millibels)
 *      0x20   | DBPOD_PAMASK_BAL_DELAY | Balancing delays (nanoseconds)
 *      0x08   | DBPOD_PAMASK_RX_GAIN   | RX element gains (millibels)
 *
 */
#define DBPOD_CURRENT_PACKET_VERSION    0x50001

/**
 * \brief Message Header.
 *
 * Every message begins with a message header.
 */
typedef struct TAG_DBPOD_MSGHDR
{
    ULONG       dwLength;       /**< Length of message (not including length).
                                 */
    ULONG       dwSequence;     /**< Sequence number of command
                                 * (copied to synchronous response). */
    USHORT      wCmd;           /**< \ref MessageCodes "Message code". */
    USHORT      wSubCode;       /**< Sub code for response. */
} DBPOD_MSGHDR;

/**
 * \anchor MessageCodes
 * \name Message codes
 *
 * These macros define values for the \ref DBPOD_MSGHDR::wCmd "wCmd"
 * member of \ref DBPOD_MSGHDR.
 *
 * Note that a synchronous error response to any command consists of just a
 * message header with the appropriate response code and a non-zero sub-code.
 *
 * \{
 */
/**
 * \brief Synchronous response bit.
 *
 * This bit in \ref DBPOD_MSGHDR::wCmd "wCmd" marks a synchronous response
 * to a command.
 */
#define DBPOD_MSGFLAG_SYNC_RESPONSE         0x8000
/**
 * \brief Asynchronous response bit.
 *
 * This bit in \ref DBPOD_MSGHDR::wCmd "wCmd" marks the message as an
 * asynchronous response with no matching command.
 */
#define DBPOD_MSGFLAG_ASYNC_RESPONSE        0x4000
/**
 * \brief 'Get Capabilities' command message code.
 * \sa \ref DBPOD_CMDBUF_GET_CAPABILITIES,
 * \ref DBPOD_CMDBUF_GET_CAPABILITIES_EXT.
 */
#define DBPOD_CMDCODE_GET_CAPABILITIES      0x0000
/**
 * \brief 'Get Capabilities' response message code.
 * \sa \ref DBPOD_RSPBUF_GET_CAPABILITIES.
 */
#define DBPOD_RSPCODE_GET_CAPABILITIES      0x8000
/**
 * \brief 'Diagnostic Tests' command message code.
 * \sa \ref DBPOD_CMDBUF_DIAGS.
 */
#define DBPOD_CMDCODE_DIAGS                 0x0001
/**
 * \brief 'Diagnostic Tests' response message code.
 * \sa \ref DBPOD_RSPBUF_DIAGS.
 */
#define DBPOD_RSPCODE_DIAGS                 0x8001
/**
 * \brief 'Dummy' command message code.
 * \sa \ref DBPOD_CMDBUF_DUMMY.
 */
#define DBPOD_CMDCODE_DUMMY                 0x0002
/**
 * \brief 'Dummy' response message code.
 * \sa \ref DBPOD_RSPBUF_DUMMY.
 */
#define DBPOD_RSPCODE_DUMMY                 0x8002
/**
 * \brief 'Start UT' command message code.
 * \sa \ref DBPOD_CMDBUF_START_UT.
 */
#define DBPOD_CMDCODE_START_UT              0x0010
/**
 * \brief 'Start UT' response message code.
 * \sa \ref DBPOD_RSPBUF_START_UT.
 */
#define DBPOD_RSPCODE_START_UT              0x8010
/**
 * \brief 'Stop UT' command message code.
 * \sa \ref DBPOD_CMDBUF_STOP_UT.
 */
#define DBPOD_CMDCODE_STOP_UT               0x0011
/**
 * \brief 'Stop UT' response message code.
 * \sa \ref DBPOD_RSPBUF_STOP_UT.
 */
#define DBPOD_RSPCODE_STOP_UT               0x8011
/**
 * \brief 'Start VC' command message code.
 * \sa \ref DBPOD_CMDBUF_START_VC.
 */
#define DBPOD_CMDCODE_START_VC              0x0012
/**
 * \brief 'Start VC' response message code.
 * \sa \ref DBPOD_RSPBUF_START_VC.
 */
#define DBPOD_RSPCODE_START_VC              0x8012
/**
 * \brief 'Stop VC' command message code.
 * \sa \ref DBPOD_CMDBUF_STOP_VC.
 */
#define DBPOD_CMDCODE_STOP_VC               0x0013
/**
 * \brief 'Stop VC' response message code.
 * \sa \ref DBPOD_RSPBUF_STOP_VC.
 */
#define DBPOD_RSPCODE_STOP_VC               0x8013
/**
 * \brief 'Set Flash Parameters' command message code.
 * \sa \ref DBPOD_CMDBUF_SET_FLASH_PARAMS.
 */
#define DBPOD_CMDCODE_SET_FLASH_PARAMS      0x0020
/**
 * \brief 'Set Flash Parameters' response message code.
 * \sa \ref DBPOD_RSPBUF_SET_FLASH_PARAMS.
 */
#define DBPOD_RSPCODE_SET_FLASH_PARAMS      0x8020
/**
 * \brief 'Get Flash Parameters' command message code.
 * \sa \ref DBPOD_CMDBUF_GET_FLASH_PARAMS.
 */
#define DBPOD_CMDCODE_GET_FLASH_PARAMS      0x0021
/**
 * \brief 'Get Flash Parameters' response message code.
 * \sa \ref DBPOD_RSPBUF_GET_FLASH_PARAMS.
 */
#define DBPOD_RSPCODE_GET_FLASH_PARAMS      0x8021
/**
 * \brief 'Get MAC Address' command message code.
 * \sa \ref DBPOD_CMDBUF_GET_MAC_ADDRESS.
 */
#define DBPOD_CMDCODE_GET_MAC_ADDRESS       0x0022
/**
 * \brief 'Get MAC Address' response message code.
 * \sa \ref DBPOD_RSPBUF_GET_MAC_ADDRESS.
 */
#define DBPOD_RSPCODE_GET_MAC_ADDRESS       0x8022
/**
 * \brief 'Channel/Sequence Configuration' command message code.
 * \sa \ref DBPOD_CMDBUF_CHAN_CONFIG.
 */
#define DBPOD_CMDCODE_CHAN_CONFIG           0x0030
/**
 * \brief 'Channel/Sequence Configuration' response message code.
 * \sa \ref DBPOD_RSPBUF_CHAN_CONFIG.
 */
#define DBPOD_RSPCODE_CHAN_CONFIG           0x8030
/**
 * \brief 'DAC Memory Set' command message code.
 * \sa \ref DBPOD_CMDBUF_DAC_MEMORY_SET.
 */
#define DBPOD_CMDCODE_DAC_MEMORY_SET        0x0031
/**
 * \brief 'DAC Memory Set' response message code.
 * \sa \ref DBPOD_RSPBUF_DAC_MEMORY_SET.
 */
#define DBPOD_RSPCODE_DAC_MEMORY_SET        0x8031
/**
 * \brief 'Encoder Configuration' command message code.
 * \sa \ref DBPOD_CMDBUF_ENCODER_CONFIG.
 */
#define DBPOD_CMDCODE_ENCODER_CONFIG        0x0032
/**
 * \brief 'Encoder Configuration' response message code.
 * \sa \ref DBPOD_RSPBUF_ENCODER_CONFIG.
 */
#define DBPOD_RSPCODE_ENCODER_CONFIG        0x8032
/**
 * \brief 'Video Configuration' command message code.
 * \sa \ref DBPOD_CMDBUF_VIDEO_CONFIG.
 */
#define DBPOD_CMDCODE_VIDEO_CONFIG          0x0033
/**
 * \brief 'Video Configuration' response message code.
 * \sa \ref DBPOD_RSPBUF_VIDEO_CONFIG.
 */
#define DBPOD_RSPCODE_VIDEO_CONFIG          0x8033
/**
 * \brief 'Scan Configuration' command message code.
 * \sa \ref DBPOD_CMDBUF_SCAN_CONFIG.
 */
#define DBPOD_CMDCODE_SCAN_CONFIG           0x0034
/**
 * \brief 'Scan Configuration' response message code.
 * \sa \ref DBPOD_RSPBUF_SCAN_CONFIG.
 */
#define DBPOD_RSPCODE_SCAN_CONFIG           0x8034
/**
 * \brief 'Phased Array Channel Element/Delay/Gain' command message code.
 * \sa \ref DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY.
 */
#define DBPOD_CMDCODE_PA_CHAN_ELEM_DELAY    0x0035
/**
 * \brief 'Phased Array Channel Element/Delay/Gain' response message code.
 * \sa \ref DBPOD_RSPBUF_PA_CHAN_ELEM_DELAY.
 */
#define DBPOD_RSPCODE_PA_CHAN_ELEM_DELAY    0x8035
/**
 * \brief 'Set Encoders' command message code.
 * \sa \ref DBPOD_CMDBUF_SET_ENCODERS.
 */
#define DBPOD_CMDCODE_SET_ENCODERS          0x0040
/**
 * \brief 'Set Encoders' response message code.
 * \sa \ref DBPOD_RSPBUF_SET_ENCODERS.
 */
#define DBPOD_RSPCODE_SET_ENCODERS          0x8040
/**
 * \brief 'Get Encoders' command message code.
 * \sa \ref DBPOD_CMDBUF_GET_ENCODERS.
 */
#define DBPOD_CMDCODE_GET_ENCODERS          0x0041
/**
 * \brief 'Get Encoders' response message code.
 * \sa \ref DBPOD_RSPBUF_GET_ENCODERS.
 */
#define DBPOD_RSPCODE_GET_ENCODERS          0x8041
/**
 * \brief 'Set LED' command message code.
 * \sa \ref DBPOD_CMDBUF_SET_LED.
 */
#define DBPOD_CMDCODE_SET_LED               0x0042
/**
 * \brief 'Set LED' response message code.
 * \sa \ref DBPOD_RSPBUF_SET_LED.
 */
#define DBPOD_RSPCODE_SET_LED               0x8042
/**
 * \brief 'Get LED' command message code.
 * \sa \ref DBPOD_CMDBUF_GET_LED.
 */
#define DBPOD_CMDCODE_GET_LED               0x0043
/**
 * \brief 'Get LED' response message code.
 * \sa \ref DBPOD_RSPBUF_GET_LED.
 */
#define DBPOD_RSPCODE_GET_LED               0x8043
/**
 * \brief 'MDU Configuration' command message code.
 * \sa \ref DBPOD_CMDBUF_MDU_CONFIG.
 */
#define DBPOD_CMDCODE_MDU_CONFIG            0x0050
/**
 * \brief 'MDU Configuration' response message code.
 * \sa \ref DBPOD_RSPBUF_MDU_CONFIG.
 */
#define DBPOD_RSPCODE_MDU_CONFIG            0x8050
/**
 * \brief 'MDU Data' command message code.
 * \sa \ref DBPOD_CMDBUF_MDU_DATA.
 */
#define DBPOD_CMDCODE_MDU_DATA              0x0051
/**
 * \brief 'MDU Data' response message code.
 * \sa \ref DBPOD_RSPBUF_MDU_DATA.
 */
#define DBPOD_RSPCODE_MDU_DATA              0x8051
/**
 * \brief 'Synchronous Video Data' command message code.
 * \sa \ref DBPOD_CMDBUF_SYNC_VIDEO.
 */
#define DBPOD_CMDCODE_SYNC_VIDEO            0x0060
/**
 * \brief 'Synchronous Video Data' response message code.
 * \sa \ref DBPOD_RSPBUF_SYNC_VIDEO_DUMMY,
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT,
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8,
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT.
 */
#define DBPOD_RSPCODE_SYNC_VIDEO            0x8060
/**
 * \brief 'Start Asynchronous Video Data' command message code.
 * \sa \ref DBPOD_CMDBUF_START_ASYNC_VIDEO.
 */
#define DBPOD_CMDCODE_START_ASYNC_VIDEO     0x0061
/**
 * \brief 'Start Asynchronous Video Data' response message code.
 * \sa \ref DBPOD_RSPBUF_START_ASYNC_VIDEO.
 */
#define DBPOD_RSPCODE_START_ASYNC_VIDEO     0x8061
/**
 * \brief 'Stop Asynchronous Video Data' command message code.
 * \sa \ref DBPOD_CMDBUF_STOP_ASYNC_VIDEO.
 */
#define DBPOD_CMDCODE_STOP_ASYNC_VIDEO      0x0062
/**
 * \brief 'Stop Asynchronous Video Data' response message code.
 * \sa \ref DBPOD_RSPBUF_STOP_ASYNC_VIDEO.
 */
#define DBPOD_RSPCODE_STOP_ASYNC_VIDEO      0x8062
/**
 * \brief 'Asynchronous Video Data' message code.
 * \sa \ref DBPOD_MSGBUF_ASYNC_VIDEO_DUMMY,
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_UNKNOWN_FORMAT,
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8,
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8VT.
 */
#define DBPOD_MSGCODE_ASYNC_VIDEO           0x4060
/**
 * \brief 'Synchronous Record Data' command message code.
 * \sa \ref DBPOD_CMDBUF_SYNC_RECORD.
 */
#define DBPOD_CMDCODE_SYNC_RECORD           0x0070
/**
 * \brief 'Synchronous Record Data' response message code.
 * \sa \ref DBPOD_RSPBUF_SYNC_RECORD_SOM,
 * \ref DBPOD_CHUNK_UT_SOC,
 * \ref DBPOD_CHUNK_UT_EOC.
 */
#define DBPOD_RSPCODE_SYNC_RECORD           0x8070
/**
 * \brief 'Start Asynchronous Record Data' command message code.
 * \sa \ref DBPOD_CMDBUF_START_ASYNC_RECORD.
 */
#define DBPOD_CMDCODE_START_ASYNC_RECORD    0x0071
/**
 * \brief 'Start Asynchronous Record Data' response message code.
 * \sa \ref DBPOD_RSPBUF_START_ASYNC_RECORD.
 */
#define DBPOD_RSPCODE_START_ASYNC_RECORD    0x8071
/**
 * \brief 'Stop Asynchronous Record Data' command message code.
 * \sa \ref DBPOD_CMDBUF_STOP_ASYNC_RECORD.
 */
#define DBPOD_CMDCODE_STOP_ASYNC_RECORD     0x0072
/**
 * \brief 'Stop Asynchronous Record Data' response message code.
 * \sa \ref DBPOD_RSPBUF_STOP_ASYNC_RECORD.
 */
#define DBPOD_RSPCODE_STOP_ASYNC_RECORD     0x8072
/**
 * \brief 'Asynchronous Record Data' message code.
 * \sa \ref DBPOD_MSGBUF_ASYNC_RECORD_SOM,
 * \ref DBPOD_CHUNK_UT_SOC,
 * \ref DBPOD_CHUNK_UT_EOC.
 */
#define DBPOD_MSGCODE_ASYNC_RECORD          0x4070
/**
 * \brief 'Synchronous Display Data' command message code.
 * \sa \ref DBPOD_CMDBUF_SYNC_DISPLAY.
 */
#define DBPOD_CMDCODE_SYNC_DISPLAY          0x0080
/**
 * \brief 'Synchronous Display Data' response message code.
 * \sa \ref DBPOD_RSPBUF_SYNC_DISPLAY_SOM,
 * \ref DBPOD_CHUNK_UT_SOC,
 * \ref DBPOD_CHUNK_UT_EOC.
 */
#define DBPOD_RSPCODE_SYNC_DISPLAY          0x8080
/**
 * \brief 'Asynchronous Error' message code.
 * \sa \ref DBPOD_MSGBUF_ASYNC_ERROR.
 */
#define DBPOD_MSGCODE_ASYNC_ERROR           0x4100
/**
 * \brief 'Abort' command message code.
 * \sa \ref DBPOD_CMDBUF_ABORT.
 */
#define DBPOD_CMDCODE_ABORT                 0x0101
/**
 * \brief 'Abort' response message code.
 * \sa \ref DBPOD_RSPBUF_ABORT.
 */
#define DBPOD_RSPCODE_ABORT                 0x8101
/**
 * \brief 'Soft Reset' command message code.
 * \sa \ref DBPOD_CMDBUF_SOFT_RESET.
 */
#define DBPOD_CMDCODE_SOFT_RESET            0x0102
/**
 * \brief 'Soft Reset' response message code.
 * \sa \ref DBPOD_CMDBUF_SOFT_RESET.
 */
#define DBPOD_RSPCODE_SOFT_RESET            0x8102
/** \} */

/********************************************
 *
 * 'Get Capabilities' command and response.
 *
 ********************************************/

/**
 * \brief 'Get Capabilities' command buffer.
 *
 * The 'Get Capabilities' command is used to get the device capabilities.
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_GET_CAPABILITIES.
 *
 * The expected response is a partial \ref DBPOD_RSPBUF_GET_CAPABILITIES
 * "'Get Capabilities' response" up to and including the \c dwMaxPRF field.
 *
 * \sa \ref DBPOD_CMDBUF_GET_CAPABILITIES_EXT.
 */
typedef struct TAG_DBPOD_CMDBUF_GET_CAPABILITIES
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_GET_CAPABILITIES;

/**
 * \brief Extended 'Get Capabilities' command buffer.
 *
 * The 'Get Capabilities' command is used to get the device capabilities.
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_GET_CAPABILITIES.
 *
 * The expected response is a partial or full \ref
 * DBPOD_RSPBUF_GET_CAPABILITIES "'Get Capabilities'" response including the \c
 * dwMaxPRF field and possibly additional fields if \c dwMaxCapLength is
 * greater than 512 and the dbPOD device supports packet version 0x50000 or
 * later.
 *
 * To request all capabilities known to this version of the header file, set \c
 * dwMaxCapLength to <tt>sizeof(DBPOD_RSPBUF_GET_CAPABILITIES)</tt> &minus;
 * <tt>sizeof(DBPOD_MSGHDR)</tt>.  If the dbPOD device does not support the
 * full size \ref DBPOD_RSPBUF_GET_CAPABILITIES then the \ref
 * DBPOD_MSGHDR::dwLength "hdr.dwLength" member of the response message will
 * be less than <tt>sizeof(DBPOD_RSPBUF_GET_CAPABILITIES)</tt> &minus;
 * <tt>sizeof(ULONG)</tt>, but will be at least 520
 * (<tt>sizeof(DBPOD_MSGHDR)</tt> + 512 &minus; <tt>sizeof(ULONG)</tt>), which
 * covers all members up to and including at least the \c dwMaxPRF member.
 */
typedef struct TAG_DBPOD_CMDBUF_GET_CAPABILITIES_EXT
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG dwMaxCapLength;           /**< Maximum length of capabilities data,
                                     * not including the message header. */
} DBPOD_CMDBUF_GET_CAPABILITIES_EXT;

/**
 * \brief Sample format for capabilities.
 *
 * This is used by the elements of the \ref
 * DBPOD_RSPBUF_GET_CAPABILITIES::SampleFormat "SampleFormat" array member
 * of \ref DBPOD_RSPBUF_GET_CAPABILITIES.
 */
typedef struct TAG_DBPOD_SAMPLE_FORMAT
{
    UCHAR nDigBits;                 /**< Sample width in bits. */
    UCHAR nSpanBits;                /**< Sample span in bits. */
    UCHAR nSamplesPerWord;          /**< Samples per 32-bit word. */
    UCHAR bFlags;                   /**< \ref SampleFormatFlags
                                     * "Sample format flags".  */
} DBPOD_SAMPLE_FORMAT;

/**
 * \anchor SampleFormatFlags
 * \name Sample format flags
 *
 * These macros define values for the \ref DBPOD_SAMPLE_FORMAT::bFlags
 * "bFlags" member of \ref DBPOD_SAMPLE_FORMAT.
 *
 * \{
 */
/** \brief 2's complement, not offset binary. */
#define DBPOD_SAMPLE_FLAG_SIGNED    0x01
/** \} */

/**
 * \brief 'Get Capabilities' response buffer.
 *
 * This is the normal response to the \ref DBPOD_CMDBUF_GET_CAPABILITIES
 * "'Get Capabilities' command" and the \ref DBPOD_CMDBUF_GET_CAPABILITIES_EXT
 * "Extended 'Get Capabilities' command".  The \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_RSPCODE_GET_CAPABILITIES.
 *
 * The response will be truncated if it is in reply to the non-extended \ref
 * DBPOD_CMDBUF_GET_CAPABILITIES "'Get Capabilities' command" or if the dbPOD
 * device cannot fill in all the data requested by the \ref
 * DBPOD_CMDBUF_GET_CAPABILITIES_EXT "extended 'Get Capabilities' command".
 *
 * The overall length of the response message including the \ref
 * DBPOD_MSGHDR::dwLength "hdr.dwLength" field will be <tt>hdr.dwLength</tt> +
 * 4 (<tt>hdr.dwLength</tt> + <tt>sizeof(ULONG)</tt>) and will include all
 * fields whose offset plus size is less than or equal to the overall response
 * message length.  The overall length of the response message will be at least
 * 524 (<tt>sizeof(DBPOD_MSGHDR</tt> + 512) and no more than the \ref
 * DBPOD_CMDBUF_GET_CAPABILITIES_EXT "extended 'Get Capabilities' command"'s \c
 * dwMaxCapLength field plus 12 (\c dwMaxCapLength +
 * <tt>sizeof(DBPOD_MSGHDR)</tt>), so the \ref DBPOD_MSGHDR::dwLength
 * "hdr.dwLength" field will be 4 (<tt>sizeof(ULONG)</tt>) less than that.
 * This will cover everything up to at least the end of the \c dwMaxPRF field.
 */
typedef struct TAG_DBPOD_RSPBUF_GET_CAPABILITIES
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    CHAR        szHwName[80];       /**< Hardware name. */
    ULONG       dwPodVersion;       /**< Overall version of db-Pod system:
                                     * HIWORD major, LOWORD minor. */
    ULONG       dwPacketVersion;    /**< Version of packet structure:
                                     * HIWORD major, LOWORD minor. */
    USHORT      wHardVersion;       /**< Version of hardware. */
    USHORT      wDrvVersion;        /**< Version of driver:
                                     * HIBYTE major, LOBYTE minor. */
    LONG        fPower2Avg;         /**< Flag: averager values must be
                                     * a power of 2. */
    LONG        fGlobalAvg;         /**< Flag: All firings must use same
                                     * averaging value. */
    LONG        fOverlapGates;      /**< Flag: Overlapping gates supported. */
    LONG        fDelayTiedToDig;    /**< Flag: Delay generation clocked at
                                     * digitiser. */
    SHORT       nSampleFormats;     /**< Number of sample formats supported. */
    SHORT       fGlobalSampleFormat;/**< Flag: Sample format must be
                                     * configured alike for all channels. */
    DBPOD_SAMPLE_FORMAT SampleFormat[4]; /**< Sample formats. */
    ULONG       dwMaxPoints;        /**< Maximum number of points per firing. */
    SHORT       nDigFreq;           /**< Number of digitisation frequencies
                                     * supported. */
    USHORT      nChanConfigs;       /**< Number of Channel/Sequence table
                                     * entries supported by pod. */
    ULONG       adwDigFreq[16];     /**< List of digitisation frequencies
                                     * in Hz supported by pod. */
    ULONG       dwMaxDelay;         /**< Maximum delay before digitiser
                                     * in nanoseconds. */
    ULONG       dwMaxRange;         /**< Maximum digitiser range
                                     * in nanoseconds. */
    SHORT       nMaxAvg;            /**< Maximum averaging value. */
    UCHAR       bHardType;          /**< \ref HwTypeCodes "Hardware type code".
                                     */
    UCHAR       bAFEType;           /**< \ref AFETypeCodes
                                     * "Hardware AFE type code". */
    ULONG       dwFastMemSize;      /**< ADC fast memory size in bytes. */
    ULONG       dwFifoMemSize;      /**< ADC results FIFO size in bytes. */
    ULONG       dwDacMemSize;       /**< DAC memory size in DAC curve
                                     * elements. */
    ULONG       dwDacMemPageSize;   /**< DAC memory page size in DAC curve
                                     * elements. */
    LONG        fPower2DacDivisor;  /**< Flag: DAC sample rate divisor must be
                                     * a power of 2. */
    SHORT       nMinDacDivisor;     /**< Minimum DAC sample rate divisor. */
    SHORT       nMaxDacDivisor;     /**< Maximum DAC sample rate divisor. */
    SHORT       nMinHT;             /**< Minimum HT voltage in volts. */
    SHORT       nMaxHT;             /**< Maximum HT voltage in volts. */
    SHORT       nHTResolution;      /**< HT voltage granularity in volts. */
    UCHAR       bPAType;            /**< \ref PATypeCodes
                                     * "Phased array type code". */
    UCHAR       pad4;               /**< (padding) */
    LONG        fGlobalHT;          /**< Flag: HT voltage must be configured
                                     * alike for all channels. */
    LONG        fGlobalPW;          /**< Flag: pulse width must be configured
                                     * alike for all channels. */
    SHORT       nMinPW;             /**< Minimum pulse width in nanoseconds. */
    SHORT       nMaxPW;             /**< Maximum pulse width in nanoseconds. */
    SHORT       nPWResolution;      /**< Pulse width granularity
                                     * in nanoseconds. */
    SHORT       nMaxPulserPower;    /**< Maximum pulser output power
                                     * in Watts. */
    SHORT       nMinGain;           /**< Minimum gain in millibels
                                     * (100 mB = 1 dB). */
    SHORT       nMaxGain;           /**< Maximum gain in millibels. */
    SHORT       nGainResolution;    /**< Gain granularity in millibels. */
    SHORT       nLowPass;           /**< Number of low-pass filter breakpoints.
                                     * A &minus;1 indicates a continuous range.
                                     */
    LONG        anLPF[16];          /**< Low-pass filter breakpoints,
                                     * or continuous range from
                                     * <tt>anLPF[0]</tt> to <tt>anLPF[1]</tt>,
                                     * in Hz. */
    LONG        fGlobalLPF;         /**< Flag: low-pass filters must be
                                     * configured alike for all channels. */
    SHORT       nHighPass;          /**< Number of high-pass filter
                                     * breakpoints. A &minus;1 indicates a
                                     * continuous range. */
    SHORT       pad5;               /**< (padding) */
    LONG        anHPF[16];          /**< High-pass filter breakpoints,
                                     * or continuous range from
                                     * <tt>anHPF[0]</tt> to <tt>anHPF[1]</tt>,
                                     * in Hz. */
    LONG        fGlobalHPF;         /**< Flag: high-pass filters must be
                                     * configured alike for all channels. */
    SHORT       nRectFilters;       /**< Number of post-rectification filter
                                     * breakpoints. A &minus;1 indicates a
                                     * continuous range. */
    SHORT       pad6;               /**< (padding) */
    LONG        anRFF[16];          /**< Post-rectification filter breakpoints,
                                     * or continuous range from
                                     * <tt>anRFF[0]</tt> to <tt>anRFF[1]</tt>,
                                     * in Hz. */
    ULONG       dwFiltScaleFreq;    /**< Filter scale frequency.
                                     * Used in conjunction with #fScaleLPF,
                                     * #fScaleHPF, and #fScaleRFF.
                                     * If those are all \c FALSE,
                                     * #dwFiltScaleFreq is unused and may be
                                     * 0.  Otherwise, it holds the denomimator
                                     * of the scaling factor (generally the
                                     * highest digitization frequency). */
    LONG        fGlobalRFF;         /**< Flag: post-rectification filters must
                                     * be configured alike for all channels. */
    SHORT       nMinTrigPulse;      /**< Minimum pulse width of the external
                                     * trigger input in microseconds. */
    SHORT       nChannels;          /**< Number of conventional physical
                                     * transducer channels (not phased array).
                                     */
    SHORT       nDACs;              /**< Maximum number of DAC curves. */
    SHORT       nGates;             /**< Maximum number of gates per channel. */
    SHORT       nEncoders;          /**< Number of encoder axes. */
    SHORT       nPots;              /**< Number of potentiometer axes. */
    SHORT       nPotBits;           /**< Number of bits on the potentiometer
                                     * ADC. */
    USHORT      wProjNum;           /**< Project number for hardware
                                     * specials. */
    CHAR        fVideoTracking;     /**< Flag: video tracking available. */
    CHAR        fScaleLPF;          /**< Flag: low-pass filter breakpoint
                                     * scaling.  If \c FALSE, values in
                                     * #anLPF[] are absolute.  If \c TRUE,
                                     * values in #anLPF[] should be multiplied
                                     * by the digitization frequency and
                                     * divided by #dwFiltScaleFreq. */
    CHAR        fScaleHPF;          /**< Flag: high-pass filter breakpoint
                                     * scaling.  If \c FALSE, values in
                                     * #anHPF[] are absolute.  If \c TRUE,
                                     * values in #anHPF[] should be multiplied
                                     * by the digitization frequency and
                                     * divided by #dwFiltScaleFreq. */
    CHAR        fScaleRFF;          /**< Flag: post-rectification filter
                                     * breakpoint scaling.  If \c FALSE, values
                                     * in #anRFF[] are absolute.  If \c TRUE,
                                     * values in #anRFF[] should be multiplied
                                     * by the digitization frequency and
                                     * divided by #dwFiltScaleFreq. */
    ULONG       dwMinPRF;           /**< Minimum PRF in microseconds. */
    ULONG       dwMaxPRF;           /**< Maximum PRF in microseconds. */
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
    USHORT      wScaleMaskLPF;      /**< Low-pass filter breakpoint scaling
                                     * bit-mask.  Indicates which entries in
                                     * #anLPF[] are scaled and which ones are
                                     * absolute.  A '1' in a bit position
                                     * indicates that the entry at the
                                     * corresponding index in #anLPF[] is
                                     * scaled and a '0' in a bit position
                                     * indicates that the entry at the
                                     * corresponding index in #anLPF[] is
                                     * absolute.  This is more informative than
                                     * #fScaleLPF. */
    USHORT      wScaleMaskHPF;      /**< High-pass filter breakpoint scaling
                                     * bit-mask.  Indicates which entries in
                                     * #anHPF[] are scaled and which ones are
                                     * absolute.  A '1' in a bit position
                                     * indicates that the entry at the
                                     * corresponding index in #anHPF[] is
                                     * scaled and a '0' in a bit position
                                     * indicates that the entry at the
                                     * corresponding index in #anHPF[] is
                                     * absolute.  This is more informative than
                                     * #fScaleHPF.  */
    USHORT      wScaleMaskRFF;      /**< Post-rectification filter breakpoint
                                     * scaling bit-mask.  Indicates which
                                     * entries in #anRFF[] are scaled and which
                                     * ones are absolute.  A '1' in a bit
                                     * position indicates that the entry at the
                                     * corresponding index in #anRFF[] is
                                     * scaled and a '0' in a bit position
                                     * indicates that the entry at the
                                     * corresponding index in #anRFF[] is
                                     * absolute.  This is more informative than
                                     * #fScaleRFF. */
    USHORT      wMaxAperture;       /**< Phased array maximum aperture size
                                     * (maximum number of parallel ADCs). */
    USHORT      wMaxElements;       /**< Phased array maximum number of
                                     * elements. */
    USHORT      wTxFocusGranularity;/**< Phased array TX focus (delays)
                                     * granularity in nanoseconds. */
    USHORT      wRxFocusGranularity;/**< Phased array RX focus (delays)
                                     * granularity in nanoseconds.. */
    USHORT      wTxMaxFocusSteps;   /**< Phased array maximum number of TX
                                     * delay steps that the hardware supports.
                                     * So the maximum TX delay in nanoseconds
                                     * is * #wTxMaxFocusSteps &times;
                                     * #wRxFocusGranularity. */
    USHORT      wRxMaxFocusSteps;   /**< Phased array maximum number of RX
                                     * delay steps that the hardware supports.
                                     * So the maximum RX delay in nanoseconds
                                     * is #wRxMaxFocusSteps &times;
                                     * #wRxFocusGranularity. */
    SHORT       nMinBalanceGain;    /**< Phased array minimum element balancing
                                     * gain in millibels. */
    SHORT       nMaxBalanceGain;    /**< Phased array maximum element balancing
                                     * gain in millibels. */
    SHORT       nBalanceGainRes;    /**< Phased array element balancing gain
                                     * granularity in millibels at a gain of
                                     * &minus;1000 millibels.  The granularity
                                     * may increase exponentially for gains
                                     * less than &minus;1000 millibels. */
    /*
     * The length from the start of the 'szHwName' field up to this point
     * is 536 bytes.
     */
} DBPOD_RSPBUF_GET_CAPABILITIES;
/**
 * \brief Legacy (unextended) size of \ref DBPOD_RSPBUF_GET_CAPABILITIES
 * up to and including \c dwMaxPRF.
 */
#define LEGACY_SIZE_DBPOD_RSPBUF_GET_CAPABILITIES   \
    (FIELD_OFFSET(DBPOD_RSPBUF_GET_CAPABILITIES, dwMaxPRF) + \
     sizeof(((DBPOD_RSPBUF_GET_CAPABILITIES *)0)->dwMaxPRF))
/**
 * \brief Legacy (unextended) length of capabilities in bytes from the start of
 * \c szHwName to the end of \c dwMaxPRF.
 *
 * This is the minimum value of \c dwMaxCapLength in \ref
 * DBPOD_CMDBUF_GET_CAPABILITIES_EXT and the default value if \c dwMaxCapLength
 * is missing (for \ref DBPOD_CMDBUF_GET_CAPABILITIES).
 */
#define LEGACY_CAPABILITIES_LENGTH  \
    (LEGACY_SIZE_DBPOD_RSPBUF_GET_CAPABILITIES - sizeof(DBPOD_MSGHDR))

/**
 * \anchor HwTypeCodes
 * \name Hardware type codes
 *
 * These macros define values for the
 * \ref DBPOD_RSPBUF_GET_CAPABILITIES::bHardType "bHardType" member of
 * \ref DBPOD_RSPBUF_GET_CAPABILITIES.
 *
 * \{
 */
/** \brief Classic Ethernet Pod (Nios II). */
#define DBPOD_HARDTYPE_ORIG_ETH     0
/** \brief Fast Ethernet Pod (ARM). */
#define DBPOD_HARDTYPE_FAST_ETH     1
/** \brief Ethernet Mini Pod (ARM). */
#define DBPOD_HARDTYPE_MINI_ETH     2
/** \brief USB3.0 Mini Pod (FTDI). */
#define DBPOD_HARDTYPE_MINI_USB     3
/** \brief Ethernet Mini Phased Array (ARM). */
#define DBPOD_HARDTYPE_MINIPA_ETH   4
/** \} */

/**
 * \anchor AFETypeCodes
 * \name Hardware AFE type codes
 *
 * These macros define values for the
 * \ref DBPOD_RSPBUF_GET_CAPABILITIES::bAFEType "bAFEType" member of
 * \ref DBPOD_RSPBUF_GET_CAPABILITIES.
 *
 * \{
 */
/** \brief Classic Ethernet Pod or Fast Pod. */
#define DBPOD_AFETYPE_ORIGINAL      0
/** \brief MAX2077 -- Mini Pod (Ethernet or USB). */
#define DBPOD_AFETYPE_MAX2077       1
/** \brief VCA5807 -- Mini Phased Array. */
#define DBPOD_AFETYPE_VCA5807       2
/** \} */

/**
 * \anchor PATypeCodes
 * \name Hardware phased array type codes
 *
 * These macros define values for the
 * \ref DBPOD_RSPBUF_GET_CAPABILITIES::bPAType "bPAType" member of
 * \ref DBPOD_RSPBUF_GET_CAPABILITIES.
 *
 * \{
 */
/** \brief No phased array. */
#define DBPOD_PATYPE_NONE           0
/** \brief Mini Phased Array partial beam former. */
#define DBPOD_PATYPE_MINI_PBF       1
/** \} */

/********************************************
 *
 * 'Diagnostic Tests' command and response.
 *
 ********************************************/

/**
 * \brief 'Diagnostic Tests' command buffer.
 *
 * The 'Diagnostic Tests' command is used to start a series of self tests or
 * get the results of a previously started series of self tests.  The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to \ref
 * DBPOD_CMDCODE_DIAGS.
 *
 * This command is not currently supported by the dbPOD devices because no
 * tests have been defined or implemented, so the response will be a message
 * header with \ref DBPOD_MSGHDR::wCmd "wCmd" set to \ref DBPOD_RSPCODE_DIAGS
 * and with \ref DBPOD_MSGHDR::wSubCode "wSubCode" set to \ref
 * DBPOD_ERR_COMMAND_NOT_SUPPORTED.
 *
 * If the command is supported by the dbPOD device, the expected response is a
 * \ref DBPOD_RSPBUF_DIAGS "'Diagnostic Tests' response".
 */
typedef struct TAG_DBPOD_CMDBUF_DIAGS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG       nCommand;           /**< \ref DiagCmdCodes
                                     * "Diagnostic command code". */
    ULONG       nDiagTests;         /**< Bitmask of \ref DiagTestBits
                                     * "diagnostic tests". */
} DBPOD_CMDBUF_DIAGS;

/**
 * \anchor DiagCmdCodes
 * \name Diagnostic command codes
 *
 * These macros define values for the \ref DBPOD_CMDBUF_DIAGS::nCommand
 * "nCommand" member of \ref DBPOD_CMDBUF_DIAGS.
 *
 * \{
 */
/** \brief Get test results so far. */
#define DBPOD_DIAGS_COMMAND_GETTEST     0
/** \brief Start tests. */
#define DBPOD_DIAGS_COMMAND_START       1
/** \brief Start tests verbosely. */
#define DBPOD_DIAGS_COMMAND_START_VERBOSE 2
/** \} */

/**
 * \anchor DiagTestBits
 * \name Diagnostic test bits
 *
 * Macros defining diagnostic test bit-mask values.
 *
 * \remark So far, no diagnostic tests have been defined or implemented.
 *
 * \{
 */
/* XXX TO BE DEFINED! XXX */
/** \brief This is just here as a place-holder for documentation purposes. */
#define DBPOD_DIAG_DOCUMENTATION_PLACEHOLDER 0 /* Just a place-holder. */
/** \} */

/**
 * \brief 'Diagnostic Tests' response buffer.
 *
 * This is the normal response to the \ref DBPOD_CMDBUF_DIAGS
 * "'Diagnostic Tests' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_RSPCODE_DIAGS.
 *
 * \remark The 'Diagnostic Tests' command has not been implemented, so this
 * normal response will not occur.
 */
typedef struct TAG_DBPOD_RSPBUF_DIAGS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG       nDiagTests;         /**< Bitmask of \ref DiagTestBits
                                     * "diagnostic tests". */
    ULONG       nDiagComplete;      /**< Bitmask of completed tests. */
    ULONG       nDiagPassed;        /**< Bitmask of passed tests. */
    char        szResult[128];      /**< Text string giving description of a
                                     * test outcome. */
    /* XXX TODO - Append extra information, e.g. analog offsets. */
} DBPOD_RSPBUF_DIAGS;

/********************************************
 *
 * 'Dummy' command and response.
 *
 ********************************************/

/**
 * \brief 'Dummy' command buffer.
 *
 * The 'Dummy' command is used for basic testing purposes.  The \ref
 * DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to \ref DBPOD_CMDCODE_DUMMY.
 *
 * The expected response is a \ref DBPOD_RSPBUF_DUMMY "'Dummy' response"
 * containing variable length dummy data.
 */
typedef struct TAG_DBPOD_CMDBUF_DUMMY
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG       dwReplyLen;         /**< Length of dummy data to send in the
                                      * response in bytes.  The dbPOD device
                                      * rounds this up to a multiple of 4.
                                      */
} DBPOD_CMDBUF_DUMMY;

/**
 * \brief 'Dummy' response buffer.
 *
 * This is the normal response to the \ref DBPOD_CMDBUF_DUMMY
 * "'Dummy' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_RSPCODE_DUMMY.
 *
 * The variable length #bDummyData array contains dummy data up to the end of
 * the response.
 */
typedef struct TAG_DBPOD_RSPBUF_DUMMY
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    UCHAR       bDummyData[DBPOD_ANYLENGTH];    /**< Dummy data bytes. */
} DBPOD_RSPBUF_DUMMY;

/**
 * \brief Basic size of \ref DBPOD_RSPBUF_DUMMY without \ref
 * DBPOD_RSPBUF_DUMMY::bDummyData "bDummyData[]".
 *
 * Followed by array <tt>UCHAR bDummyData[dwReplyLen]</tt> plus possible
 * padding (where \ref DBPOD_CMDBUF_DUMMY::dwReplyLen "dwReplyLen" is from the
 * \ref DBPOD_CMDBUF_DUMMY "'Dummy' command").
 */
#define BASE_SIZE_DBPOD_RSPBUF_DUMMY \
    FIELD_OFFSET(DBPOD_RSPBUF_DUMMY, bDummyData[0])

/********************************************
 *
 * 'Start UT' command and response.
 *
 ********************************************/

/**
 * \brief 'Start UT' command buffer.
 *
 * The 'Start UT' command is used to start UT acquisition.  The \ref
 * DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to \ref DBPOD_CMDCODE_START_UT.
 *
 * \remark All sequence table entries from #nStartIndex to #nEndIndex must
 * have been configured.
 *
 * \remark For dbPOD, #nStartIndex must be 0.
 *
 * \remark On a successful response, the device will discard any previously
 * unread data in its acquisition buffer.
 *
 * The expected response is a \ref DBPOD_RSPBUF_START_UT
 * "'Start UT' response".
 */
typedef struct TAG_DBPOD_CMDBUF_START_UT
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    USHORT      nStartIndex;        /**< Sequence table index to start at. */
    USHORT      nEndIndex;          /**< Sequence table index to stop after. */
    LONG        fMaster;            /**< \c TRUE if the device is a master
                                     * (generating an external trigger pulse
                                     * at the start of sweep for a sequence
                                     * entry);
                                     * \c FALSE if the device is a slave
                                     * (starting the sweep for a sequence
                                     * entry in response to an external
                                     * trigger). */
    ULONG       dwFlags;            /**< \ref StartUtFlags "Start UT flags". */
} DBPOD_CMDBUF_START_UT;

/**
 * \anchor StartUtFlags
 * \name Start UT flags
 *
 * These macros define values that can be bit-wise <b>OR</b>ed together for
 * for the \ref DBPOD_CMDBUF_START_UT::dwFlags "dwFlags" member of
 * \ref DBPOD_CMDBUF_START_UT.
 *
 * \note See the description of \ref DBPOD_CHUNK_UT_EOC for the effect of
 * setting \c DBPOD_START_UT_EXTRA_ENCS.
 *
 * Use top byte of \c dwFlags to specify phased array element swapping.
 *
 * \{
 */
/** \brief Turn on desparkler (\b OR bit). */
#define DBPOD_START_UT_DESPARKLE                0x00000001
/** \brief Turn on compression (\b OR bit). */
#define DBPOD_START_UT_COMPRESS                 0x00000002
/** \brief Extra encoders (\b OR bit). */
#define DBPOD_START_UT_EXTRA_ENCS               0x00000004
/** \brief Phased array default mapping (swaps odd and even elements for
 * Mini Phased Array) (\b OR value). */
#define DBPOD_START_UT_PE_MAP_DEFAULT           0x00000000
/** \brief Phased array native mapping (does not swap any elements)
 * (\b OR value). */
#define DBPOD_START_UT_PE_MAP_NATIVE            0x01000000
/** \brief Phased array swap odd and even elements (\b OR value). */
#define DBPOD_START_UT_PE_MAP_ODD_EVEN_SWAP     0x02000000
/** \brief Phased array mask for element swapping value. */
#define DBPOD_START_UT_PE_MAP_MASK              0xFF000000
/** \} */

/**
 * \brief 'Start UT' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_START_UT
 * "'Start UT' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set
 * to \ref DBPOD_RSPCODE_START_UT.
 */
typedef struct TAG_DBPOD_RSPBUF_START_UT
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_START_UT;

/********************************************
 *
 * 'Stop UT' command and response.
 *
 ********************************************/

/**
 * \brief 'Stop UT' command buffer.
 *
 * The 'Stop UT' command is used to stop a previously started UT acquisition.
 * It does not discard unread data in the device's acquisition buffer.  The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_STOP_UT.
 *
 * The expected response is a \ref DBPOD_RSPBUF_STOP_UT
 * "'Stop UT' response".
 */
typedef struct TAG_DBPOD_CMDBUF_STOP_UT
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_STOP_UT;

/**
 * \brief 'Stop UT' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_STOP_UT
 * "'Stop UT' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set
 * to \ref DBPOD_RSPCODE_STOP_UT.
 */
typedef struct TAG_DBPOD_RSPBUF_STOP_UT
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_STOP_UT;

/********************************************
 *
 * 'Start VC' (video capture) command and response.
 *
 ********************************************/

/**
 * \brief 'Start VC' command buffer.
 *
 * The 'Start VC' command is used to start video capture.  The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_START_VC.
 *
 * \remark Video must have been configured and enabled by the
 * \ref DBPOD_CMDBUF_VIDEO_CONFIG "'Video Configuration' command".
 *
 * \remark Video capture and UT acquisition cannot be active at the same time.
 *
 * \remark On a successful response, the device will discard any previously
 * unread data in its acquisition buffer.
 *
 * The expected response is a \ref DBPOD_RSPBUF_START_VC
 * "'Start VC' response".
 */
typedef struct TAG_DBPOD_CMDBUF_START_VC
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_START_VC;

/**
 * \brief 'Start VC' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_START_VC
 * "'Start VC' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set
 * to \ref DBPOD_RSPCODE_START_VC.
 */
typedef struct TAG_DBPOD_RSPBUF_START_VC
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_START_VC;

/********************************************
 *
 * 'Stop VC' (video capture) command and response.
 *
 ********************************************/

/**
 * \brief 'Stop VC' command buffer.
 *
 * The 'Stop VC' command is used to stop a previously started video capture.
 * It does not discard unread data in the device's acquisition buffer. The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_STOP_VC.
 *
 * The expected response is a \ref DBPOD_RSPBUF_STOP_VC "'Stop VC' response".
 */
typedef struct TAG_DBPOD_CMDBUF_STOP_VC
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_STOP_VC;

/**
 * \brief 'Stop VC' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_STOP_VC
 * "'Stop VC' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set
 * to \ref DBPOD_RSPCODE_STOP_VC.
 */
typedef struct TAG_DBPOD_RSPBUF_STOP_VC
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_STOP_VC;

/********************************************
 *
 * 'Set Flash Parameters' command and response.
 *
 ********************************************/

/**
 * \brief 'Set Flash Parameters' command buffer.
 *
 * The 'Set Flash Parameters' command is used to set non-volatile calibration
 * parameters in device.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set
 * to \ref DBPOD_CMDCODE_SET_FLASH_PARAMS.
 *
 * \remark The same buffer format is used for the
 * \ref DBPOD_RSPBUF_GET_FLASH_PARAMS "'Get Flash Parameters' response".
 *
 * The expected response is a \ref DBPOD_RSPBUF_SET_FLASH_PARAMS
 * "'Set Flash Parameters' response".
 */
typedef struct TAG_DBPOD_CMDBUF_SET_FLASH_PARAMS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    FILETIME    CalibTime;          /**< Calibration timestamp in 100 ns
                                     * intervals since January 1, 1601 (UTC). */
    ULONG       ValidChans;         /**< Bit-mask of valid channels. */
    SHORT       NullOffset;         /**< Null Offset corrections. */
    SHORT       GainRef2;           /**< Gain Stage 2 reference tweak. */
    SHORT       GainRef3;           /**< Gain Stage 3 reference tweak. */
    SHORT       GainMult;           /**< DAC gain correction multiplier
                                     * times 1000 (a gain correction
                                     * multiplier of 1 is reprepresented by
                                     * \c GainMult value 1000). */
    SHORT       GainOffset;         /**< DAC gain correction offset in
                                     * millibels (100 mB = 1 dB). */
    SHORT       pad1;               /**< (padding) */
} DBPOD_CMDBUF_SET_FLASH_PARAMS;
/* (Same as 'Get Flash Parameters' response buffer.) */

/**
 * \brief 'Set Flash Parameters' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_SET_FLASH_PARAMS
 * "'Set Flash Parameters' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_RSPCODE_SET_FLASH_PARAMS.
 */
typedef struct TAG_DBPOD_RSPBUF_SET_FLASH_PARAMS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_SET_FLASH_PARAMS;

/********************************************
 *
 * 'Get Flash Parameters' command and response.
 *
 ********************************************/

/**
 * \brief 'Get Flash Parameters' command buffer.
 *
 * The 'Get Flash Parameters' command is used to get non-volatile calibration
 * parameters from the device.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member
 * is set to \ref DBPOD_CMDCODE_GET_FLASH_PARAMS.
 *
 * The expected response is a \ref DBPOD_RSPBUF_GET_FLASH_PARAMS
 * "'Get Flash Parameters' response".
 */
typedef struct TAG_DBPOD_CMDBUF_GET_FLASH_PARAMS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_GET_FLASH_PARAMS;

/**
 * \typedef DBPOD_RSPBUF_GET_FLASH_PARAMS
 * \brief 'Get Flash Parameters' response buffer.
 *
 * This is the normal response to the \ref DBPOD_CMDBUF_GET_FLASH_PARAMS
 * "'Get Flash Parameters' command".
 *
 * The buffer is the same format as the \ref DBPOD_CMDBUF_SET_FLASH_PARAMS
 * "'Set Flash Parameters' command", but the \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_RSPCODE_GET_FLASH_PARAMS.
 */
typedef DBPOD_CMDBUF_SET_FLASH_PARAMS   DBPOD_RSPBUF_GET_FLASH_PARAMS;

/********************************************
 *
 * 'Get MAC Address' command and response.
 *
 * Note: The MAC address should be unique to a particular pod.)
 *
 ********************************************/

/**
 * \brief 'Get MAC Address' command buffer.
 *
 * The 'Get MAC Address' command is used to get the MAC address of the dbPOD.
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_GET_MAC_ADDRESS.
 *
 * The expected response is a \ref DBPOD_RSPBUF_GET_MAC_ADDRESS
 * "'Get MAC Address' response".
 */
typedef struct TAG_DBPOD_CMDBUF_GET_MAC_ADDRESS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_GET_MAC_ADDRESS;

/**
 * \brief 'Get MAC Address' response buffer.
 *
 * This is the normal response to the \ref DBPOD_CMDBUF_GET_MAC_ADDRESS
 * "'Get MAC Address' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_RSPCODE_GET_MAC_ADDRESS.
 */
typedef struct TAG_DBPOD_RSPBUF_GET_MAC_ADDRESS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    UCHAR       MacAddress[6];      /**< MAC address. */
    SHORT       pad1;               /**< (padding) */
} DBPOD_RSPBUF_GET_MAC_ADDRESS;

/********************************************
 *
 * 'Channel/Sequence Configuration' command and response.
 *
 ********************************************/

/**
 * \brief Gate configuration.
 *
 * Describes the configuration of a gate included in the
 * \ref DBPOD_CMDBUF_CHAN_CONFIG::Gate "Gate" array member of the
 * \ref DBPOD_CMDBUF_CHAN_CONFIG "'Channel/Sequence Configuration'" structure.
 */
typedef struct TAG_DBPOD_GATECFG
{
    /** \ref GateTypeCodes "Gate type code". */
    UCHAR       GateType;
    /** \ref GateRefCodes "Gate reference code". */
    UCHAR       Reference;
    SHORT       pad1;               /**< (padding) */
    /** Threshold percentage x 10 (not applicable to all gate types). */
    LONG        nThreshold;
    /**
     * Sample position at which gate starts in digitiser clock periods
     * relative to the \ref GateRefCodes "gate reference" specified by
     * #Reference.
     */
    LONG        lStart;
    /** Number of samples in gate in digitiser clock periods. */
    LONG        lWidth;
    /**
     * Control value depending on the gate type:
     * - For \ref DBPOD_GATETYPE_AVERAGING "'Averaging'" gates, it specifies
     *   the \ref AveragingModeCodes "averaging mode".
     * - For \ref DBPOD_GATETYPE_INTERFACE "'Interface'" and
     *   \ref DBPOD_GATETYPE_LOSS_SIG "'Loss Of Signal'" gates, it specifies
     *   the \ref PolarityCodes "polarity".
     * - For \ref DBPOD_GATETYPE_PEAK "'Peak'" gates, it encodes the maximum
     *   number of peaks (bits 7 to 0) and the \ref PeakTypeCodes "peak type"
     *   (bits 15 to 8) (see \ref PeakControlMacros
     *   "Peak gate control encoding").  These are currently ignored by dbPOD!
     */
    LONG        nControl;
    /**
     * Parameter value depending on the gate type:
     * - For \ref DBPOD_GATETYPE_INTERFACE "'Interface'" gates, it specifies
     *   the Interface Gain in millibels (100 mB = 1 dB).
     */
    LONG        lParameter;
} DBPOD_GATECFG;

/**
 * \anchor GateTypeCodes
 * \name Gate type codes
 *
 * These macros define values for the \ref DBPOD_GATECFG::GateType "GateType"
 * member of \ref DBPOD_GATECFG.
 *
 * \{
 */
/** \brief Averaging gate. */
#define DBPOD_GATETYPE_AVERAGING    0
/** \brief Interface gate. */
#define DBPOD_GATETYPE_INTERFACE    1
/** \brief Display gate. */
#define DBPOD_GATETYPE_DISPLAY      2
/** \brief Record gate. */
#define DBPOD_GATETYPE_RECORD       3
/** \brief Peak gate. */
#define DBPOD_GATETYPE_PEAK         4
/** \brief Loss Of Signal gate. */
#define DBPOD_GATETYPE_LOSS_SIG     5
/** \brief Gain Control Interface gate. */
#define DBPOD_GATETYPE_GAIN_CTRL    6
/** \} */

/**
 * \anchor GateRefCodes
 * \name Gate reference codes
 *
 * These macros define values for the \ref DBPOD_GATECFG::Reference "Reference"
 * member of the \ref DBPOD_GATECFG "gate configuration".  The gate reference
 * modifies the start position of the gate.
 *
 * \{
 */
/** \brief Absolute reference. */
#define DBPOD_GATEREF_ABSOLUTE      0
/** \brief Interface reference. */
#define DBPOD_GATEREF_INTERFACE     1
/** \brief Reference start of previous gate. */
#define DBPOD_GATEREF_PREVGATE      2
/** \} */

/**
 * \anchor AveragingModeCodes
 * \name Averaging mode codes
 *
 * For the \ref DBPOD_GATETYPE_AVERAGING "'Averaging'" gate type, these macros
 * define values for the averaging mode stored in the
 * \ref DBPOD_GATECFG::nControl "nControl" member of \ref DBPOD_GATECFG.
 *
 * \{
 */
/** \brief Produce the mean value of a set of samples. */
#define DBPOD_AVERAGING_MEAN        0
/** \brief Produce the maximum (extreme) value of a set of samples. */
#define DBPOD_AVERAGING_MAX         1
/** \} */

/**
 * \anchor PolarityCodes
 * \name Polarity control for gates
 *
 * For the \ref DBPOD_GATETYPE_INTERFACE "'Interface'" and
 * \ref DBPOD_GATETYPE_LOSS_SIG "'Loss Of Signal'" gate types, these macros
 * define values for the gate polarity type stored in the
 * \ref DBPOD_GATECFG::nControl "nControl" member of \ref DBPOD_GATECFG.
 *
 * \{
 */
/** \brief Unipolar. */
#define DBPOD_POLARITY_UNIPOLAR     0
/** \brief Bipolar. */
#define DBPOD_POLARITY_BIPOLAR      1
/** \} */

/**
 * \anchor PeakControlMacros
 * \name Peak gate control encoding
 *
 * For the \ref DBPOD_GATETYPE_PEAK "'Peak'" gate type, these macros encode
 * and decode the maximum number of peaks (bits 7 to 0) and the
 * \ref PeakTypeCodes "peak type" (bits 15 to 8) for the
 * \ref DBPOD_GATECFG::nControl "nControl" member of the \ref DBPOD_GATECFG.
 *
 * \note For dbPOD, the maximum number of peaks is ignored, and peaks types
 * \c DBPOD_PEAKTYPE_FIRST_N_POS and \c DBPOD_PEAKTYPE_FIRST_N both select
 * bipolar peaks.
 *
 * \{
 */
/**
 * \brief Encode \c nControl value.
 *
 * Encodes a maximum number of peaks (bits 7 to 0) and a
 * \ref PeakTypeCodes "peak type" (bits 15 to 8) into the
 * \ref DBPOD_GATECFG::nControl "nControl" value for a
 * \ref DBPOD_GATETYPE_PEAK "'Peak'" gate.
 *
 * \param[in] num Maximum number of peaks.
 * \param[in] type \ref PeakTypeCodes "peak type".
 *
 * \remark The maximum number of peaks value is currently ignored by dbPOD.
 *
 * \returns Encoded \c nControl value.
 */
#define DBPOD_PEAKS_NCONTROL(num, type) (((num) & 255) | ((type) << 8))
/**
 * \brief Extract maximum number of peaks value.
 *
 * Extracts the maximum number of peaks (bits 7 to 0) value from an encoded
 * \ref DBPOD_GATECFG::nControl "nControl" value for a
 * \ref DBPOD_GATETYPE_PEAK "'Peak'" gate.
 *
 * \param[in] nControl Encoded \c nControl value.
 *
 * \returns Maximum number of peaks.
 */
#define DBPOD_PEAKS_NUM(nControl)       ((nControl) & 255)
/**
 * \brief Extract peak type value.
 *
 * Extracts the \ref PeakTypeCodes "peak type" (bits 15 to 8) value from an
 * encoded \ref DBPOD_GATECFG::nControl "nControl" value for a
 * \ref DBPOD_GATETYPE_PEAK "'Peak'" gate.
 *
 * \param[in] nControl Encoded \c nControl value.
 *
 * \returns Peak type.
 */
#define DBPOD_PEAKS_TYPE(nControl)      (((nControl) >> 8) & 255)
/** \} */

/**
 * \anchor PeakTypeCodes
 * \name Peak type codes
 *
 * These macros define values for the peak type encoded into bits 15 to 8 of
 * the \ref DBPOD_GATECFG::nControl "nControl" member of \ref DBPOD_GATECFG
 * for a \ref DBPOD_GATETYPE_PEAK "'Peak'" gate.
 *
 * \note For dbPOD, peak types \c DBPOD_PEAKTYPE_FIRST_N_POS and
 * \c DBPOD_PEAKTYPE_FIRST_N both select bipolar peaks.
 *
 * \{
 */
/** \brief Positive peaks. */
#define DBPOD_PEAKTYPE_FIRST_N_POS      0
/** \brief Bipolar (positive and negative) peaks. */
#define DBPOD_PEAKTYPE_FIRST_N          1
/** \brief Sorted bipolar peaks. */
#define DBPOD_PEAKTYPE_SORTED           2
/** \} */

/**
 * \brief 'Channel/Sequence Configuration' command buffer.
 *
 * The 'Channel/Sequence Configuration' command is used to configure a sequence
 * table entry.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_CHAN_CONFIG.
 *
 * The variable length #Gate array (of length #nGates) configures each gate
 * defined for the sequence table entry.
 *
 * \remark For dbPOD, #nRepeat must be 1.
 *
 * The expected response is a \ref DBPOD_RSPBUF_CHAN_CONFIG
 * "'Channel/Sequence Configuration' response".
 */
typedef struct TAG_DBPOD_CMDBUF_CHAN_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    USHORT      nIndex;             /**< Sequence table index i.e. channel. */
    SHORT       nDigBits;           /**< Sample size in bits. */
    ULONG       dwDelay;            /**< Time period in microseconds before
                                     * stepping or repeating. */
    SHORT       nRepeat;            /**< Number of repeat firings with #dwDelay
                                     * interval.  Must be 1 for dbPOD. */
    SHORT       nTx;                /**< Physical transmit channel to pulse:
                                     * - 0 to \c nChannels &minus; 1:
                                     *   use specified channel.
                                     * - &minus;1: do not pulse.
                                     * - &minus;2: use phased array. */
    SHORT       nRx;                /**< Physical receiver input channel:
                                     * - 0 to \c nChannels &minus; 1:
                                     *   use specified channel.
                                     * - &minus;1: disable receiver.
                                     * - &minus;2: use phased array. */
    SHORT       nVoltage;           /**< Pulser voltage in volts. */
    SHORT       nPulseWidth;        /**< Pulse Width in nanoseconds. */
    SHORT       nGain;              /**< Gain in millibels (100 mB = 1 dB). */
    LONG        nHPF;               /**< High pass filter in Hz, 0=disable. */
    LONG        nLPF;               /**< Low pass filter in Hz, 0=disable. */
    USHORT      RectifierType;      /**< \ref RectifierTypeCodes
                                     * "Rectification type". */
    /**
     * Compatibility level:
     *
     * - 0 = Old application.  Filter frequency scaling is disabled.  The
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::fScaleHPF "fScaleHPF",
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::fScaleLPF "fScaleLPF", and
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::fScaleRFF "fScaleRFF" flags and
     *   the \ref DBPOD_RSPBUF_GET_CAPABILITIES::wScaleMaskHPF "wScaleMaskHPF",
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::wScaleMaskLPF "wScaleMaskLPF", and
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::wScaleMaskRFF "wScaleMaskRFF"
     *   bit-masks in the \ref DBPOD_RSPBUF_GET_CAPABILITIES "'Get
     *   Capabilities'" response will be ignored for backwards compatibility
     *   reasons, so #nHPF, #nLPF, and #nRFF will be matched against the
     *   absolute values in the capabilities.  This may result in the wrong
     *   filter frequencies being applied.
     * - 1 = Less old application.  Filter frequency scaling is enabled.  The
     *   #nHPF, #nLPF, and #nRFF values are assumed to be set to scaled or
     *   unscaled values depending on the
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::fScaleHPF "fScaleHPF",
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::fScaleLPF "fScaleLPF", and
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::fScaleRFF "fScaleRFF" flags,
     *   respectively, in the
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES "'Get Capabilities'" response.
     *   However, the
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::wScaleMaskHPF "wScaleMaskHPF",
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::wScaleMaskLPF "wScaleMaskLPF", and
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::wScaleMaskRFF "wScaleMaskRFF"
     *   bit-masks will be ignored for backwards compatibility reasons.  This
     *   may result in the some wrong filter frequencies being applied.
     * - 2 = New application.  Filter frequency scaling is enabled.  The
     *   #nHPF, #nLPF, and #nRFF are assumed to be set to scaled or unscaled
     *   values depending on the
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::wScaleMaskHPF "wScaleMaskHPF",
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::wScaleMaskLPF "wScaleMaskLPF", and
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES::wScaleMaskRFF "wScaleMaskRFF"
     *   bit-masks, respectively, in the
     *   \ref DBPOD_RSPBUF_GET_CAPABILITIES "'Get Capabilities'" response.
     */
    USHORT      wCompatLevel;
    LONG        nRFF;               /**< Post rectifier filter (low pass)
                                     * in Hz. */
    ULONG       nSampleFreq;        /**< Digitisation rate (Hz). */
    /**
     * DAC curve start element.  Must be a multiple of the DAC memory page size
     * as reported by the \ref DBPOD_RSPBUF_GET_CAPABILITIES::dwDacMemPageSize
     * "dwDacMemPageSize" member of the \ref DBPOD_RSPBUF_GET_CAPABILITIES
     * "'Get Capabilities' response".  This will be 256 for dbPOD.
     */
    ULONG       dwDACStart;
    ULONG       dwDACLength;        /**< DAC curve length. 0=no DAC. */
    USHORT      nDACDivisor;        /**< DAC output frequency divisor relative
                                     * to Dig Freq. */
    USHORT      DacTrigger;         /**< \ref DacTriggerCodes "DAC trigger".
                                     * Determines when the Distance Amplitude
                                     * Correction curve is applied. */
    SHORT       nGates;             /**< Number of gates defined. */
    USHORT      wSpecial;           /**< Hardware-specific setting. */
    DBPOD_GATECFG  Gate[DBPOD_ANYLENGTH]; /**< Gate definitions. */
} DBPOD_CMDBUF_CHAN_CONFIG;

/**
 * \brief Basic size of \ref DBPOD_CMDBUF_CHAN_CONFIG type without
 * \ref DBPOD_CMDBUF_CHAN_CONFIG::Gate "Gate[]".
 *
 * Followed by array <tt>DBPOD_GATECFG Gate[nGates]</tt>
 * \ref DBPOD_GATECFG "gate configurations" (where \c nGates is from the
 * \ref DBPOD_CMDBUF_CHAN_CONFIG::nGates "nGates" member of
 * \ref DBPOD_CMDBUF_CHAN_CONFIG).
 */
#define BASE_SIZE_DBPOD_CMDBUF_CHAN_CONFIG  \
    FIELD_OFFSET(DBPOD_CMDBUF_CHAN_CONFIG, Gate[0])

/**
 * \anchor RectifierTypeCodes
 * \name Rectifier type codes
 *
 * These macros define values for the
 * \ref DBPOD_CMDBUF_CHAN_CONFIG::RectifierType "RectifierType" member of
 * \ref DBPOD_CMDBUF_CHAN_CONFIG.
 *
 * \{
 */
/** \brief No rectification. */
#define DBPOD_RECTIFIER_NONE    0
/** \brief Negative samples only (positive samples changed to 0). */
#define DBPOD_RECTIFIER_NEG     1
/** \brief Positive samples only (negative samples changed to 0). */
#define DBPOD_RECTIFIER_POS     2
/** \brief Full rectification (negative samples negated to positive). */
#define DBPOD_RECTIFIER_FULL    3
/** \} */

/**
 * \anchor DacTriggerCodes
 * \name DAC trigger codes
 *
 * These macros define values for the
 * \ref DBPOD_CMDBUF_CHAN_CONFIG::DacTrigger "DacTrigger" member of
 * \ref DBPOD_CMDBUF_CHAN_CONFIG.
 *
 * \{
 */
/** \brief Distance Amplitude Correction curve applied after TX pulse. */
#define DBPOD_DACTRIG_TXPULSE   0
/**
 * \brief Distance Amplitude Correction curve applied after interface
 * detected.
 */
#define DBPOD_DACTRIG_IFTRIG    1
/** \} */

/**
 * \brief 'Channel/Sequence Configuration' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_CHAN_CONFIG
 * "'Channel/Sequence Configuration' command".  The \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_RSPCODE_CHAN_CONFIG.
 */
typedef struct TAG_DBPOD_RSPBUF_CHAN_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_CHAN_CONFIG;

/********************************************
 *
 * 'DAC Memory Set' command and response.
 *
 ********************************************/

/**
 * \brief 'DAC Memory Set' command buffer.
 *
 * The 'DAC Memory Set' command is used to set the gain values for a region
 * of the Distance Amplitude Correction curve memory.  The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_DAC_MEMORY_SET.
 *
 * The variable length #nGain array (of length #dwLength) contains the gain
 * values for the region.
 *
 * \remark If #dwLength is not a multiple of the DAC memory page size, the
 * remaining samples in the final page will be set to the final gain value.
 *
 * The expected response is a \ref DBPOD_RSPBUF_DAC_MEMORY_SET
 * "'DAC Memory Set' response".
 */
typedef struct TAG_DBPOD_CMDBUF_DAC_MEMORY_SET
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    /**
     * Start index.  Must be a multiple of the DAC memory page size as
     * reported by the \ref DBPOD_RSPBUF_GET_CAPABILITIES::dwDacMemPageSize
     * "dwDacMemPageSize" member of the \ref DBPOD_RSPBUF_GET_CAPABILITIES
     * "'Get Capabilities' response".  This will be 256 for dbPOD.
     */
    ULONG       dwStart;
    ULONG       dwLength;           /**< Number of gain values. */
    SHORT       nGain[DBPOD_ANYLENGTH]; /**< Gain values in millibels
                                         * (100 mB = 1 dB). */
} DBPOD_CMDBUF_DAC_MEMORY_SET;

/**
 * \brief Basic size of \ref DBPOD_CMDBUF_DAC_MEMORY_SET type without
 * \ref DBPOD_CMDBUF_DAC_MEMORY_SET::nGain "nGain[]".
 *
 * Followed by array <tt>SHORT nGain[dwLength]</tt> of gain values (where
 * \c dwLength is from the \ref DBPOD_CMDBUF_DAC_MEMORY_SET::dwLength
 * "dwLength" member of \ref DBPOD_CMDBUF_DAC_MEMORY_SET).
 */
#define BASE_SIZE_DBPOD_CMDBUF_DAC_MEMORY_SET   \
    FIELD_OFFSET(DBPOD_CMDBUF_DAC_MEMORY_SET, nGain[0])

/**
 * \brief 'DAC Memory Set' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_DAC_MEMORY_SET
 * "'DAC Memory Set' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member
 * is set to \ref DBPOD_RSPCODE_DAC_MEMORY_SET.
 */
typedef struct TAG_DBPOD_RSPBUF_DAC_MEMORY_SET
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_DAC_MEMORY_SET;

/********************************************
 *
 * 'Encoder Configuration' command and response.
 *
 ********************************************/

/**
 * \brief 'Encoder Configuration' command buffer.
 *
 * The 'Encoder Configuration' command is used to configure the quadrature
 * encoder inputs.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_ENCODER_CONFIG.
 *
 * The expected response is a \ref DBPOD_RSPBUF_ENCODER_CONFIG
 * "'Encoder Configuration' response".
 */
typedef struct TAG_DBPOD_CMDBUF_ENCODER_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    /**
     * Bit vector indicating which axes have differential inputs.  Each bit
     * position corresponds to an axis.  For each axis (bit position):
     * bit value 0=single-ended; bit value 1=differential.
     */
    USHORT      wDifferential;
    /**
     * Bit vector indicating which axes have external reset enabled.  Each bit
     * position corresponds to an axis.  For each axis (bit position):
     * bit value 0=external reset disabled; bit value 1=external reset enabled.
     */
    USHORT      wResetEnable;
    /**
     * Bit vector indicating which axes have their input sense inverted.  Each
     * bit position corresponds to an axis.  For each axis (bit position):
     * bit value 0=normal; bit value 1=inverted.
     */
    USHORT      wInvertSense;
    /** Chooses the external reset source for axes that can be reset. */
    USHORT      wResetSource;
} DBPOD_CMDBUF_ENCODER_CONFIG;

/**
 * \brief 'Encoder Configuration' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_ENCODER_CONFIG
 * "'Encoder Configuration' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_RSPCODE_ENCODER_CONFIG.
 */
typedef struct TAG_DBPOD_RSPBUF_ENCODER_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_ENCODER_CONFIG;

/********************************************
 *
 * 'Video Configuration' command and response.
 *
 ********************************************/

/**
 * \brief 'Video Configuration' command buffer.
 *
 * The 'Video Configuration' command is used to configure parameters for
 * video capture, video tracking, and the video tracking LED.  The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_VIDEO_CONFIG.
 *
 * \remark Set #nStandard to &minus;1 to power down the video subsystem.
 *
 * \remark For dbPOD, #nStrmSizeScale must be 1.
 *
 * The expected response is a \ref DBPOD_RSPBUF_VIDEO_CONFIG
 * "'Video Configuration' response".
 */
typedef struct TAG_DBPOD_CMDBUF_VIDEO_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    SHORT       nStandard;          /**< \ref VidStdCodes "Video standard". */
    SHORT       nInterleave;        /**< \ref VidInterleaveCodes
                                     * "Interleave". */
    SHORT       nTrkFrameRate;      /**< Frame Rate for video tracking
                                     * (4 to 25 or 30). */
    SHORT       nXTrkFrameSize;     /**< Total number of Horizontal pixels
                                     * in frame for tracking. */
    SHORT       nYTrkFrameSize;     /**< Total number of Vertical pixels
                                     * in frame for tracking. */
    SHORT       nXTrkOrigin;        /**< Digitisation origin (x co-ordinate)
                                     * for tracking. */
    SHORT       nYTrkOrigin;        /**< Digitisation origin (y co-ordinate)
                                     *  for tracking. */
    SHORT       nXTrkCropSize;      /**< Number of x-pixels to digitise
                                     * for tracking. */
    SHORT       nYTrkCropSize;      /**< Number of y-pixels to digitise
                                     * for tracking. */
    /**
     * Video stream \ref VidStrmScaleCodes
     * "horizontal frame size reduction factor" compared to tracking frame.
     */
    SHORT       nStrmSizeScale;
    SHORT       nStrmRateScale;     /**< Video stream frame rate reduction
                                     * factor compared to tracking (1 to 16)
                                     * or 0 to disable video capture. */
    SHORT       pad1;               /**< (padding) */
    LONG        fAGC;               /**< Flag to enable automatic gain control
                                     * of the video tracking LED. */
    SHORT       nAGCThreshold;      /**< LED threshold (if #fAGC is \c TRUE). */
    SHORT       nFormat;            /**< \ref VidDataFmtCodes
                                     * "Video data format". */
    SHORT       nNoDotThreshold;    /**< "No dot" threshold. */
    SHORT       pad2;               /**< (padding) */
} DBPOD_CMDBUF_VIDEO_CONFIG;

/**
 * \anchor VidStdCodes
 * \name Video standard codes
 *
 * These macros define values for the \ref DBPOD_CMDBUF_VIDEO_CONFIG::nStandard
 * "nStandard" member of \ref DBPOD_CMDBUF_VIDEO_CONFIG.
 *
 * \{
 */
/** \brief "Unconfigure" video and power down video subsystem. */
#define DBPOD_VID_STD_DISABLE       (-1)
/** \brief Configure for PAL standard video input signal. */
#define DBPOD_VID_STD_PAL           0
/** \brief Configure for NTSC standard video input signal. */
#define DBPOD_VID_STD_NTSC          1
/** \} */

/**
 * \anchor VidInterleaveCodes
 * \name Video interleave codes
 *
 * These macros define values for the
 * \ref DBPOD_CMDBUF_VIDEO_CONFIG::nInterleave "nInterleave" member of
 * \ref DBPOD_CMDBUF_VIDEO_CONFIG.
 *
 * \{
 */
/** \brief Output odd fields only. */
#define DBPOD_VID_INTERLEAVE_ODD    1
/** \brief Output even fields only. */
#define DBPOD_VID_INTERLEAVE_EVEN   2
/** \brief Output both odd and even fields. */
#define DBPOD_VID_INTERLEAVE_BOTH   3
/** \} */

/**
 * \anchor VidStrmScaleCodes
 * \name Horizontal frame size reduction factors
 *
 * These macros define values for the
 * \ref DBPOD_CMDBUF_VIDEO_CONFIG::nStrmSizeScale "nStrmSizeScale" member of
 * \ref DBPOD_CMDBUF_VIDEO_CONFIG.  These set the horizontal size reduction
 * factor for video capture compared to video tracking.
 *
 * \remark For dbPOD, only the value 1 is supported because it does not
 * currently support horizontal size reduction.
 *
 * \remark There is no configuration for vertical frame size reduction.  dbPOD
 * will capture full frames or half frames at a time according to the
 * \ref VidInterleaveCodes "interleave" setting.  The old Microplus-II system
 * always captured half frames (single odd or even fields) at a time.
 *
 * \{
 */
/** \brief No horizontal size reduction. */
#define DBPOD_VID_STRMSIZESCALE_FULL    1
/** \brief Horizontal size reduced to half size (not supported on dbPOD). */
#define DBPOD_VID_STRMSIZESCALE_HALF    2
/** \} */

/**
 * \anchor VidDataFmtCodes
 * \name Video capture data format codes.
 *
 * These macros define the values for the
 * \ref DBPOD_CMDBUF_VIDEO_CONFIG::nFormat "nFormat" member of
 * \ref DBPOD_CMDBUF_VIDEO_CONFIG and the \c nFormat member of the
 * various 'Synchronous Video Data' response buffers
 * (\ref DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT,
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8, and
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT) and matching
 * 'Async Video Data' message buffers
 * (\ref DBPOD_MSGBUF_ASYNC_VIDEO_UNKNOWN_FORMAT,
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8, and
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8VT).
 *
 * \{
 */
/** \brief Unknown video capture data format. */
#define DBPOD_VID_UNKNOWN_FORMAT    (-1)
/**
 * \brief \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8 "RAWGREY8" video capture data.
 */
#define DBPOD_VID_RAWGREY8          0
/**
 * \brief \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT "RAWGREY8VT" video capture
 * data.
 */
#define DBPOD_VID_RAWGREY8VT        1
/** \} */

/**
 * \brief 'Video Configuration' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_VIDEO_CONFIG
 * "'Video Configuration' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_RSPCODE_VIDEO_CONFIG.
 */
typedef struct TAG_DBPOD_RSPBUF_VIDEO_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_VIDEO_CONFIG;

/********************************************
 *
 * 'Scan Configuration' command and response.
 *
 ********************************************/

/**
 * \brief 'Scan Configuration' command buffer.
 *
 * The 'Scan Configuration' command is used to configure the
 * \ref ScanTypeCodes "scan type" and parameters to use during UT acquisition.
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_SCAN_CONFIG.
 *
 * \remark Initially (after opening the device, or after a
 * \ref DBPOD_CMDBUF_SOFT_RESET "'Soft Reset' command"), the scan type will be
 * set to \ref DBPOD_SCAN_FREERUN "free run".
 *
 * The expected response is a \ref DBPOD_RSPBUF_SCAN_CONFIG
 * "'Scan Configuration' response".
 */
typedef struct TAG_DBPOD_CMDBUF_SCAN_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    USHORT      ScanType;           /**< \ref ScanTypeCodes "Scan type". */
    USHORT      nScanAxis;          /**< Axis to monitor when tied to
                                     * position. */
    LONG        lScanStart;         /**< Scan start position. */
    LONG        lScanEnd;           /**< Scan end position. */
    ULONG       ScanStep;           /**< Step between scans as a fixed point,
                                     * 16.16 bit fraction. */
} DBPOD_CMDBUF_SCAN_CONFIG;

/**
 * \anchor ScanTypeCodes
 * \name Scan type codes
 *
 * These macros define values for the \ref DBPOD_CMDBUF_SCAN_CONFIG::ScanType
 * "ScanType" member of \ref DBPOD_CMDBUF_SCAN_CONFIG.
 *
 * \{
 */
/** \brief "Free run" scan. */
#define DBPOD_SCAN_FREERUN          0
/** \brief "Tied to position" scan. */
#define DBPOD_SCAN_TIEDTOPOSITION   1
/** \brief "Free position" scan. */
#define DBPOD_SCAN_FREEPOSITION     2
/** \} */

/**
 * \brief 'Scan Configuration' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_SCAN_CONFIG
 * "'Scan Configuration' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_RSPCODE_SCAN_CONFIG.
 */
typedef struct TAG_DBPOD_RSPBUF_SCAN_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_SCAN_CONFIG;

/********************************************
 *
 * 'Phased Array Channel Element/Delay/Gain' command and response.
 *
 ********************************************/

/**
 * \brief 'Phased Array Channel Element/Delay/Gain' command buffer.
 *
 * The 'Phased Array Channel Element/Delay/Gain' command is used to configure
 * various parts of the phased array as determined by the #bTypeMask field.
 * It may be used several times to configure different parts of the phased
 * array.
 *
 * Some parts are specific to a single sequence table entry specified by the
 * #nIndex field value, but can be applied to all sequence table entries by
 * setting the #nIndex field to &minus;1.  Other parts are always applied
 * globally and ignore the #nIndex field value.
 *
 * The sequence table entry specific parts are the list of TX element numbers,
 * the list of RX element numbers, the list of TX element delays, and the list
 * of RX element delays.  The dbPOD software will impose restrictions on the
 * sets of element numbers that can be included in the list due to hardware
 * limitations (for example, there may be a restriction that prohibits gaps
 * in the element numbers in the list).
 *
 * The global parts are a list containing the balancing gain for each physical
 * element, and a list containing the balancing delay for each physical
 * element.
 *
 * The #bTypeMask field value can specify more than one type of value if the
 * list of data values of length #nCount in the #anValue[] field can be shared
 * by different parts, for example when the lists of TX element numbers and
 * RX element numbers are identical.
 *
 * The expected response is a \ref DBPOD_RSPBUF_PA_CHAN_ELEM_DELAY "'Phased
 * Array Channel Element/Delay/Gain' response".
 */
typedef struct TAG_DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    SHORT       nIndex;             /**< Sequence table index to which this
                                     * command applies:
                                     * - 0 to \c nChanConfigs &minus; 1:
                                     *   specific sequence table entry.
                                     * - &minus;1: all sequence table entries.
                                     * Ignored for balancing gains and
                                     * balancing delays, which are always
                                     * global. */
    UCHAR       bTypeMask;          /**< \ref PATypeMaskBits "PA Elem/Delay
                                     * types". */
    CHAR        pad1;               /**< (padding = 0) */
    USHORT      nCount;             /**< Length of #anValue[] array. */
    SHORT       pad2;               /**< (padding = 0) */
    LONG        anValue[DBPOD_ANYLENGTH];   /**< Element numbers, delays,
                                             * or gains. */
} DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY;

/**
 * \brief Basic size of \ref DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY type without
 * \ref DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY::anValue "anValue[]".
 *
 * Followed by array <tt>LONG anValue[nCount]</tt>.
 */
#define BASE_SIZE_DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY   \
    FIELD_OFFSET(DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY, anValue[0])

/**
 * \anchor PATypeMaskBits
 * \name Phased Array Configuration Type Bits
 *
 * These macros define bit-mask values for the
 * \ref DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY::bTypeMask "bTypeMask" member of
 * \ref DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY.
 *
 * Specifies whether the 'Phased Array Channel Element/Delay/Gain' command is
 * configuring a list of TX element numbers, RX element numbers,
 * TX delays, RX delays, or RX element gains.
 *
 * It is possible to combine TX element numbers and RX element numbers into
 * a single list, or combine TX and RX delays into a single list by setting
 * multiple bits in the \c bTypeMask value.
 *
 * Also used for lists of element balancing gains and element balancing delays.
 * The \c nIndex value is ignored for element balancing gains and element
 * balancing delays because they are applied globally.
 *
 * RX element gain (\c DBPOD_PAMASK_RX_GAIN) is supported since packet version
 * 0x50001.
 *
 * \{
 */
/** \brief TX element numbers. */
#define DBPOD_PAMASK_TX_ELEM    0x01
/** \brief RX element numbers. */
#define DBPOD_PAMASK_RX_ELEM    0x02
/** \brief TX element delays in nanoseconds. */
#define DBPOD_PAMASK_TX_DELAY   0x04
/** \brief RX element delays in nanoseconds. */
#define DBPOD_PAMASK_RX_DELAY   0x08
/** \brief Element balancing gains in millibels (100 mB = 1 dB). */
#define DBPOD_PAMASK_BAL_GAIN   0x10
/** \brief Element balancing delays in nanoseconds. */
#define DBPOD_PAMASK_BAL_DELAY  0x20
/** \brief RX element gains in millibels (100 mB = 1 dB). */
#define DBPOD_PAMASK_RX_GAIN    0x40
/** \} */

/**
 * \brief 'Phased Array Channel Element/Delay/Gain' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_PA_CHAN_ELEM_DELAY
 * "'Phased Array Channel Element/Delay/Gain' command".  The \ref
 * DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to \ref
 * DBPOD_RSPCODE_PA_CHAN_ELEM_DELAY.
 */
typedef struct TAG_DBPOD_RSPBUF_PA_CHAN_ELEM_DELAY
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_PA_CHAN_ELEM_DELAY;

/********************************************
 *
 * 'Set Encoders' command and response.
 *
 ********************************************/

/**
 * \brief 'Set Encoders' command buffer.
 *
 * The 'Set Encoders' command is used to preset the position counters for a
 * subset of quadrature encoders.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_CMDCODE_SET_ENCODERS.
 *
 * The variable length #lReading array contains the new settings for each
 * encoder being set.  Its length is the population count of '1' bits set in
 * #nAxisBits.
 *
 * \remark Settings for non-existent encoders are ignored.
 *
 * The expected response is a \ref DBPOD_RSPBUF_SET_ENCODERS
 * "'Set Encoders' response".
 */
typedef struct TAG_DBPOD_CMDBUF_SET_ENCODERS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    /**
     * Bit vector indicating when axes are having their position preset by
     * this command.  Each bit position corresponds to an encoder axis.  For
     * each axis (bit position): bit value 0=encoder not being set;
     * bit value 1=encoder being set.
     */
    USHORT      nAxisBits;
    SHORT       pad1;               /**< (padding) */
    /**
     * New setting for each encoder axis being set.  The length of the array
     * is the number of '1' bits set in #nAxisBits.  If at least one '1' bit is
     * set, #lReading[0] contains the value for the lowest numbered axis being
     * set (determined by the position of the lowest positioned '1' bit in
     * #nAxisBits).  If at least two '1' bits are set, #lReading[1] contains
     * the value for the next lowest numbered axis being set.  And so on for
     * the remaining '1' bits.
     */
    LONG        lReading[DBPOD_ANYLENGTH];
} DBPOD_CMDBUF_SET_ENCODERS;

/**
 * \brief Basic size of \ref DBPOD_CMDBUF_SET_ENCODERS without
 * \ref DBPOD_CMDBUF_SET_ENCODERS::lReading "lReading[]".
 *
 * Followed by array <tt>LONG lReading[nset]</tt> of new preset values
 * (where \c nset is the number of '1' bits set in the
 * \ref DBPOD_CMDBUF_SET_ENCODERS::nAxisBits "nAxisBits" member of
 * \ref DBPOD_CMDBUF_SET_ENCODERS.
 */
#define BASE_SIZE_DBPOD_CMDBUF_SET_ENCODERS \
    FIELD_OFFSET(DBPOD_CMDBUF_SET_ENCODERS, lReading[0])

/**
 * \brief 'Set Encoders' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_SET_ENCODERS
 * "'Set Encoders' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is
 * set to \ref DBPOD_RSPCODE_SET_ENCODERS.
 */
typedef struct TAG_DBPOD_RSPBUF_SET_ENCODERS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_SET_ENCODERS;

/********************************************
 *
 * 'Get Encoders' command and response.
 *
 ********************************************/

/**
 * \brief 'Get Encoders' command buffer.
 *
 * The 'Get Encoders' command is used to get the current positions of all
 * encoders supported by the device.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to ref DBPOD_CMDCODE_GET_ENCODERS.
 *
 * The expected response is a \ref DBPOD_RSPBUF_GET_ENCODERS
 * "'Get Encoders' response" containing a variable length array of position
 * readings (one per axis supported by the device).
 */
typedef struct TAG_DBPOD_CMDBUF_GET_ENCODERS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_GET_ENCODERS;

/**
 * \brief 'Get Encoders' response buffer.
 *
 * This is the normal response to the \ref DBPOD_CMDBUF_GET_ENCODERS
 * "'Get Encoders' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member
 * is set to \ref DBPOD_RSPCODE_GET_ENCODERS.
 *
 * The variable length #lReading array is #nAxes elements long.
 *
 * \remark For dbPOD, #nAxes is 6:
 * - Axes 0 to 3 are quadrature encoder counter readings.
 * - Axis 4 is video tracking X position.
 * - Axis 5 is video tracking Y position.
 *
 * \note The USB Mini Pod cannot currently read the encoder positions
 * directly with the 'Get Encoders' command so their positions will read as 0
 * and video tracking axes will read as cached copies.  The Ethernet Mini Pod
 * does support reading the encoder positions directly.
 */
typedef struct TAG_DBPOD_RSPBUF_GET_ENCODERS
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    USHORT      nAxes;              /**< Number of axes. */
    SHORT       pad1;               /**< (padding) */
    LONG        lReading[DBPOD_ANYLENGTH]; /**< Readings for axes 0 to
                                             * #nAxes &minus; 1. */
} DBPOD_RSPBUF_GET_ENCODERS;

/**
 * \brief Basic size of \ref DBPOD_RSPBUF_GET_ENCODERS without
 * \ref DBPOD_RSPBUF_GET_ENCODERS::lReading "lReading[]".
 *
 * Followed by array <tt>LONG lReading[nAxes]</tt> of encoder readings (where
 * \c nAxes is from the \ref DBPOD_RSPBUF_GET_ENCODERS::nAxes "nAxes" member
 * of \ref DBPOD_RSPBUF_GET_ENCODERS.
 */
#define BASE_SIZE_DBPOD_RSPBUF_GET_ENCODERS \
    FIELD_OFFSET(DBPOD_RSPBUF_GET_ENCODERS, lReading[0])

/********************************************
 *
 * 'Set LED' command and response.
 *
 ********************************************/

/**
 * \brief 'Set LED' command buffer.
 *
 * The 'Set LED' command is used to set the intensity of the video tracking
 * LED (when automatic gain control is not enabled) and the "no dot" threshold.
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_SET_LED.
 *
 * \remark The same buffer format is used for the
 * \ref DBPOD_RSPBUF_GET_LED "'Get LED' response".
 *
 * The expected response is a \ref DBPOD_RSPBUF_SET_LED "'Set LED' response".
 */
typedef struct TAG_DBPOD_CMDBUF_SET_LED
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    /**
     * For the 'Set LED' command, this is the LED intensity (0..255).
     * For the 'Get LED' response, this is the last peak video pixel brightness
     * (0..255).
     */
    SHORT       nDrive;
    SHORT       nNoDotThreshold;    /**< Video "no dot" threshold (0..255). */
} DBPOD_CMDBUF_SET_LED;

/**
 * \brief 'Set LED' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_SET_LED "'Set LED' command".
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_RSPCODE_SET_LED.
 */
typedef struct TAG_DBPOD_RSPBUF_SET_LED
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
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

/**
 * \brief 'Get LED' command buffer.
 *
 * The 'Get LED' command gets the last peak video value (not the LED intensity)
 * and the configured "no dot" threshold.  The \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_CMDCODE_GET_LED.
 *
 * The expected response is a \ref DBPOD_RSPBUF_GET_LED "'Get LED' response".
 */
typedef struct TAG_DBPOD_CMDBUF_GET_LED
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_GET_LED;

/**
 * \typedef DBPOD_RSPBUF_GET_LED
 * \brief 'Get LED' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_GET_LED "'Get LED' command".
 * The buffer is the same format as the \ref DBPOD_CMDBUF_SET_LED
 * "'Set LED' command", but the \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is
 * set to \ref DBPOD_RSPCODE_GET_LED.
 *
 * \remark The \ref DBPOD_CMDBUF_SET_LED::nDrive "nDrive" member is the last
 * peak video pixel brightness, not the configured LED intensity.
 */
typedef DBPOD_CMDBUF_SET_LED    DBPOD_RSPBUF_GET_LED;

/********************************************
 *
 * 'MDU Configuration' command and response.
 *
 ********************************************/

/**
 * \brief 'MDU Configuration' command buffer.
 *
 * The 'MDU Configuration' command is used to configure the serial port for
 * a MDU (Motor Drive Unit).  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is
 * set to \ref DBPOD_CMDCODE_MDU_CONFIG.
 *
 * The expected response is a \ref DBPOD_RSPBUF_MDU_CONFIG
 * "'MDU Configuration' response".
 */
typedef struct TAG_DBPOD_CMDBUF_MDU_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG       dwSpeed;            /**< Bit rate (bits per second) */
    UCHAR       nDataBits;          /**< Number of data bits (5, 6, 7, 8). */
    UCHAR       nParity;            /**< \ref ParityCodes "Parity"
                                     * (0=None, 1=Odd, 2=Even). */
    UCHAR       nStopBits;          /**< Number of stop bits (1, 2). */
    UCHAR       nPort;              /**< MDU port number. */
} DBPOD_CMDBUF_MDU_CONFIG;

/**
 * \anchor ParityCodes
 * \name MDU serial format parity codes
 *
 * These macros define value for the \ref DBPOD_CMDBUF_MDU_CONFIG::nParity
 * "nParity" member of \ref DBPOD_CMDBUF_MDU_CONFIG.
 *
 * \{
 */
/** \brief No parity bit. */
#define DBPOD_MDU_PARITY_NONE       0
/** \brief Odd parity. */
#define DBPOD_MDU_PARITY_ODD        1
/** \brief Even parity. */
#define DBPOD_MDU_PARITY_EVEN       2
/** \} */

/**
 * \brief 'MDU Configuration' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_MDU_CONFIG
 * "'MDU Configuration' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_RSPCODE_MDU_CONFIG.
 */
typedef struct TAG_DBPOD_RSPBUF_MDU_CONFIG
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_MDU_CONFIG;

/********************************************
 *
 * 'MDU Data' command and response.
 *
 ********************************************/

/**
 * \brief 'MDU Data' command buffer.
 *
 * The 'MDU Data' command is used to send bytes to an MDU serial port and to
 * optionally wait for received bytes.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_CMDCODE_MDU_DATA.
 *
 * The variable length #bData array has length set by #wLength and contains
 * the specified number of bytes.
 *
 * \remark The same buffer format is used for the \ref DBPOD_RSPBUF_MDU_DATA
 * "'MDU Data' response" but there may be up to 3 padding bytes after the
 * #bData array to make the overall message length a multiple of 4 bytes.
 *
 * The expected response is a \ref DBPOD_RSPBUF_MDU_DATA "'MDU Data' response".
 */
typedef struct TAG_DBPOD_CMDBUF_MDU_DATA
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG       dwTimeout;          /**< Time to wait for reply from MDU in
                                     * milliseconds.  0=no reply expected. */
    USHORT      wLength;            /**< Number of bytes to send to MDU.
                                     * (For response, number of bytes
                                     * received from MDU.) */
    UCHAR       nPort;              /**< MDU port number. */
    CHAR        fContinuation;      /**< If 0, old rx data is discarded.
                                     * If non-0, old rx data is read (can be
                                     * used with #dwTimeout==0 to read rx data
                                     * from previous command). */
    UCHAR       bData[DBPOD_ANYLENGTH];    /**< Data bytes. */
} DBPOD_CMDBUF_MDU_DATA;
/* (Same as 'MDU Data' response buffer.) */

/**
 * \brief Basic size of \ref DBPOD_CMDBUF_MDU_DATA without
 * \ref DBPOD_CMDBUF_MDU_DATA::bData "bData[]".
 *
 * Followed by array <tt>UCHAR bData[wLength]</tt> data bytes plus possible
 * padding.
 */
#define BASE_SIZE_DBPOD_CMDBUF_MDU_DATA \
    FIELD_OFFSET(DBPOD_CMDBUF_MDU_DATA, bData[0])

/**
 * \typedef DBPOD_RSPBUF_MDU_DATA
 * \brief 'MDU Data' response buffer.
 *
 * This is the normal response to the \ref DBPOD_CMDBUF_MDU_DATA
 * "'MDU Data' command".
 *
 * The buffer is same format as the \ref DBPOD_CMDBUF_MDU_DATA
 * "'MDU Data' command", but the \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member
 * is set to \ref DBPOD_RSPCODE_MDU_DATA.
 * The \ref DBPOD_CMDBUF_MDU_DATA::wLength "wLength" member is set to the
 * number of bytes received.  The \ref DBPOD_CMDBUF_MDU_DATA::bData "bData[]"
 * member is followed by from 0 to 3 padding bytes to make the overall buffer
 * length a multiple of 4 bytes.
 */
typedef DBPOD_CMDBUF_MDU_DATA  DBPOD_RSPBUF_MDU_DATA;

/**
 * \brief Basic size of \ref DBPOD_RSPBUF_MDU_DATA without
 * \ref DBPOD_RSPBUF_MDU_DATA::bData "bData[]".
 *
 * Followed by array <tt>UCHAR bData[wLength]</tt> data bytes plus possible
 * padding.
 */
#define BASE_SIZE_DBPOD_RSPBUF_MDU_DATA BASE_SIZE_DBPOD_CMDBUF_MDU_DATA

/********************************************
 *
 * 'Synchronous Video Data' command and response.
 *
 ********************************************/

/**
 * \brief 'Synchronous Video Data' command buffer.
 *
 * The 'Synchronous Video Data' command is used to request the latest unsent
 * frame of video data.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set
 * to \ref DBPOD_CMDCODE_SYNC_VIDEO.
 *
 * The expected response is one of \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8
 * "'Synchronous Video Data' (RAWGREY8) response",
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT
 * "'Synchronous Video Data' (RAWGREY8VT) response", or
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT
 * "'Synchronous Video Data' (UNKNOWN FORMAT) response", depending on the
 * device's video configuration.  If no video data is available, the frame
 * will have zero height and width.
 *
 * \remark An alternative to the 'Synchronous Video Data' command is to use
 * the \ref DBPOD_CMDBUF_START_ASYNC_VIDEO
 * "'Start Asynchronous Video Data' command" and let the device send
 * 'Asynchronous Video Data' messages.
 */
typedef struct TAG_DBPOD_CMDBUF_SYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_SYNC_VIDEO;

/* 'Synchronous Video Data' response buffer comes in various flavours
 * depending on the format of the video data, but they all start with the
 * same header. */

/**
 * \brief 'Synchronous Video Data' (DUMMY) response buffer.
 *
 * This is not a proper response to the \ref DBPOD_CMDBUF_SYNC_VIDEO
 * "'Synchronous Video Data' command" but is the common, initial part of
 * the proper responses to that command.  The \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_RSPCODE_SYNC_VIDEO.
 *
 * \remark 'Asynchronous Video Data' message buffers are the same format, but
 * with the \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member set to
 * \ref DBPOD_MSGCODE_ASYNC_VIDEO.
 */
typedef struct TAG_DBPOD_RSPBUF_SYNC_VIDEO_DUMMY
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    SHORT       nFormat;            /**< \ref VidDataFmtCodes
                                     * "Video data format". */
    SHORT       nWidth;             /**< Width of frame in pixels. */
    SHORT       nHeight;            /**< Height of frame in pixels. */
} DBPOD_RSPBUF_SYNC_VIDEO_DUMMY;

/**
 * \brief 'Synchronous Video Data' (UNKNOWN FORMAT) response buffer.
 *
 * This is the expected response to the \ref DBPOD_CMDBUF_SYNC_VIDEO
 * "'Synchronous Video Data' command" when video capture is unconfigured.
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_RSPCODE_SYNC_VIDEO and the #nFormat member is set to
 * \ref DBPOD_VID_UNKNOWN_FORMAT.
 *
 * \remark 'Asynchronous Video Data' (UNKNOWN FORMAT) message buffers are the
 * same format, but with the \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member set to
 * \ref DBPOD_MSGCODE_ASYNC_VIDEO.  However, such messages are not
 * expected to be received.
 */
typedef struct TAG_DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    SHORT       nFormat;            /**< Video data format
                                     * \ref DBPOD_VID_UNKNOWN_FORMAT. */
    SHORT       nWidth;             /**< Width of frame in pixels
                                     * (expected to be 0). */
    SHORT       nHeight;            /**< Height of frame in pixels
                                     * (expected to be 0). */
    SHORT       pad1;               /**< (padding) */
    /**
     * Unknown format video data (expected to be zero length) up to end of
     * message, including possible padding to make the message length a
     * multiple of 4 bytes.
     */
    UCHAR       bData[DBPOD_ANYLENGTH];
} DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT;

/**
 * \brief Basic size of \ref DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT without
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT::bData "bData[]".
 *
 * Followed by <tt>UCHAR bData[len]</tt> bytes of video data of length up to
 * the end of the message and possible padding. (\c len is expected to be 0.)
 */
#define BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT    \
    FIELD_OFFSET(DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT, bData[0])

/**
 * \brief 'Synchronous Video Data' (RAWGREY8) response buffer.
 *
 * This is the expected response to the \ref DBPOD_CMDBUF_SYNC_VIDEO
 * "'Synchronous Video Data' command" when video capture is configured for
 * RAWGREY8 video data format.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_RSPCODE_SYNC_VIDEO and the #nFormat
 * member is set to \ref DBPOD_VID_RAWGREY8.
 *
 * \remark If no video capture data is available, the #nWidth and #nHeight
 * members will be 0.
 *
 * \remark 'Asynchronous Video Data' (RAWGREY8) message buffers are the
 * same format, but with the \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member set to
 * \ref DBPOD_MSGCODE_ASYNC_VIDEO.
 */
typedef struct TAG_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    SHORT       nFormat;            /**< Video data format.
                                     * \ref DBPOD_VID_RAWGREY8. */
    SHORT       nWidth;             /**< Width of frame in pixels. */
    SHORT       nHeight;            /**< Height of frame in pixels. */
    /* nBitsPerPixel will appear in all the 'raw' frame formats. */
    UCHAR       nBitsPerPixel;      /**< Number of bits per pixel (8). */
	UCHAR       bFlags;             /**< \ref VidDataRawFlags "Flags". */
    /**
     * 8-bit pixel data ordered according to the \ref VidDataRawFlags "flags"
     * in #bFlags. Length #nWidth &times; #nHeight.
     */
    UCHAR       bPixels[DBPOD_ANYLENGTH];
} DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8;

/**
 * \brief Basic size of \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8 without
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8::bPixels "bPixels[]".
 *
 * Followed by array <tt>UCHAR bPixels[nWidth*nHeight]</tt> of 8-bit pixel
 * data.
 */
#define BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8  \
    FIELD_OFFSET(DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8, bPixels[0])

/**
 * \brief Video tracking position as a little-endian bitfield structure.
 *
 * This also includes a flag to indicate whether the UT data is compressed
 * and a flag to indicate whether UT data is 8 or 16 bits.
 * Warning: This is non-portable.  Suitable for little-endian Microsoft C.
 *
 * \sa \ref VidTrkULONG "Video tracking position as a ULONG".
 */
typedef struct TAG_DBPOD_VIDTRK
{
	unsigned    uYPos : 10;			/**< Video Tracking Y position
                                     * (1023 if no dot detected). */
	unsigned    uXPos : 10;			/**< Video Tracking X position
                                     * (1023 if no dot detected). */
	unsigned    fUTCompressed : 1;  /**< Flag indicating UT data is
                                     * compressed. */
	unsigned    fUT16Bit : 1;       /**< Flag indicating 16-bit UT data. */
	unsigned	 : 2;				/**< (padding) */
	unsigned    uAmp : 8;			/**< Video Tracking dot amplitude
                                     * (0 to 255). */
} DBPOD_VIDTRK;

/**
 * \anchor VidTrkULONG
 * \name Video tracking position as a ULONG.
 *
 * The video tracking position \c ULONG encodes the X and Y position and dot
 * amplitude.  It also includes some UT data format flags.
 *
 * As a little-endian \c ULONG, the bits are as follows:
 * - Bits 31 to 24: Video Tracking dot amplitude (0 to 255).
 * - Bits 23 to 22: Unused padding.
 * - Bit 21: Flag indicating 16-bit UT data.
 * - Bit 20: Flag indicating UT data is compressed.
 * - Bits 19 to 10: Video Tracking X position (1023 if no dot detected).
 * - Bits 9 to 0: Video Tracking Y position (1023 if no dot detected).
 *
 * \sa \ref DBPOD_VIDTRK.
 *
 * The following macros extract the fields from the video tracking \c ULONG.
 *
 * \{
 */
/**
 * \brief Extracts VT Y position.
 *
 * The Y position is in bits 9 to 0 of the video tracking \c ULONG.
 *
 * \param[in] dwVt Video tracking \c ULONG.
 *
 * \returns Y position (1023 if no dot detected).
 */
#define DBPOD_VTYPOS(dwVt)  ((USHORT)((dwVt) & 0x3FF))
/**
 * \brief Extracts VT X position.
 *
 * The X position is in bits 19 to 10 of the video tracking \c ULONG.
 *
 * \param[in] dwVt Video tracking \c ULONG.
 *
 * \returns X position (1023 if no dot detected).
 */
#define DBPOD_VTXPOS(dwVt)  ((USHORT)(((dwVt) >> 10) & 0x3FF))
/**
 * \brief Extracts VT dot amplitude.
 *
 * The dot amplitude is in bits 31 to 24 of the video tracking \c ULONG.
 *
 * \param[in] dwVt Video tracking \c ULONG.
 *
 * \returns dot amplitude (0 to 255).
 */
#define DBPOD_VTAMP(dwVt)   ((UCHAR)(((dwVt) >> 24) & 0xFF))
/**
 * \brief Extracts "UT data compressed" flag.
 *
 * The "UT data compressed" flag is in bit 20 of the video tracking \c ULONG.
 *
 * \param[in] dwVt Video tracking \c ULONG.
 *
 * \returns 0 if UT data not compressed.
 * \returns 1 if UT data compressed.
 */
#define DBPOD_VT_UTCOMPRESSED(dwVt) ((unsigned)(((dwVt) >> 20) & 1))
/**
 * \brief Extracts "UT data 16-bit" flag.
 *
 * The "UT data 16-bit" flag is in bit 21 of the video tracking \c ULONG.
 *
 * \param[in] dwVt Video tracking \c ULONG.
 *
 * \returns 0 if UT data consists of 8-bit (1-byte) samples.
 * \returns 1 if UT data consists of 16-bit (2-byte) samples.
 */
#define DBPOD_VT_UT16BIT(dwVt) ((unsigned)(((dwVt) >> 21) & 1))
/** \} */

/**
 * \brief 'Synchronous Video Data' (RAWGREY8VT) response buffer.
 *
 * This is the expected response to the \ref DBPOD_CMDBUF_SYNC_VIDEO
 * "'Synchronous Video Data' command" when video capture is configured for
 * RAWGREY8VT video data format.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_RSPCODE_SYNC_VIDEO and the #nFormat
 * member is set to \ref DBPOD_VID_RAWGREY8VT.  It is similar to the RAWGREY8
 * format but with video tracking data added before the pixel data.
 *
 * \remark If no video capture data is available, the #nWidth and #nHeight
 * members will be 0.
 *
 * \remark 'Asynchronous Video Data' (RAWGREY8VT) message buffers are the
 * same format, but with the \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member set to
 * \ref DBPOD_MSGCODE_ASYNC_VIDEO.
 */
typedef struct TAG_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    SHORT       nFormat;            /**< Video data format.
                                     * \ref DBPOD_VID_RAWGREY8VT. */
    SHORT       nWidth;             /**< Width of frame in pixels. */
    SHORT       nHeight;            /**< Height of frame in pixels. */
    /* nBitsPerPixel will appear in all the 'raw' frame formats. */
    UCHAR       nBitsPerPixel;      /**< Number of bits per pixel (8). */
	UCHAR       bFlags;             /**< \ref VidDataRawFlags "Flags". */
	union
	{
		DBPOD_VIDTRK	vt;         /**< Video tracking info as a \c struct. */
		ULONG			dwVt;       /**< \ref VidTrkULONG
                                     * "Video tracking info as a ULONG". */
	};                              /* Anonymous union of 'vt' and 'dwVt'. */
    /**
     * 8-bit pixel data ordered according to the \ref VidDataRawFlags "flags"
     * in #bFlags. Length #nWidth &times; #nHeight.
     */
	UCHAR       bPixels[DBPOD_ANYLENGTH];
} DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT;

/**
 * \brief Basic size of \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT without
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8::bPixels "bPixels[]".
 *
 * Followed by array <tt>UCHAR bPixels[nWidth*nHeight]</tt> of 8-bit pixel
 * data.
 */
#define BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT  \
    FIELD_OFFSET(DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT, bPixels[0])

/**
 * \anchor VidDataRawFlags
 * \name RAWGREY8 and RAWGREY8VT format flags
 *
 * These macros define values may be bit-wise <b>OR</b>ed together in
 * the \c bFlags member of the RAWGREY8 and RAWGREY8VT video data messages
 * (\ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8,
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT,
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8, and
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8VT).
 *
 * If \ref DBPOD_VID_FLAG_RAW_TO_BE_INTERLACED is set, the raw data
 * consists of two fields (1 and 2) that need to be interlaced to reconstruct
 * the full frame.
 *
 * If \ref DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2 is set, the first (top) line of
 * the frame comes from field 2 and the second line comes field 1.
 *
 * If \ref DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2 is _not_ set (but the data needs
 * to be interlaced), the first (top) line of the frame comes from field 1 and
 * the second line comes from field 2.
 *
 * If \ref DBPOD_VID_FLAG_RAW_TO_BE_INTERLACED is set (i.e. the fields need
 * to be interlaced), the start of the second field in the raw data is
 * determined as follows:
 *
 * 1. If the frame height (H) is even, the second field begins
 *    after H/2 rows.
 * 2. If the frame height (H) is odd and the
 *    \ref DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2 flag is set, the second
 *    field begins after (H&minus;1)/2 rows.
 * 3. If the frame height (H) is odd and the
 *    \ref DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2 flag is _not_ set, the
 *    second field begins after (H+1)/2 rows.
 *
 * \{
 */
/**
 * \brief Raw data consists of two fields that need to be interlaced
 * (\b OR bit).
 */
#define DBPOD_VID_FLAG_RAW_TO_BE_INTERLACED 0x01
/** \brief Get top line of frame from field 2 (\b OR bit). */
#define DBPOD_VID_FLAG_RAW_TOP_LINE_FIELD2  0x02
/** \} */

/********************************************
 *
 * 'Start Asynchronous Video Data' command and response.
 *
 ********************************************/

/**
 * \brief 'Start Asynchronous Video Data' command buffer.
 *
 * The 'Start Asynchronous Video Data' command is used to enable the sending
 * of 'Asynchronous Video Data' messages by the device when it has video
 * capture data available.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" is set to
 * \ref DBPOD_CMDCODE_START_ASYNC_VIDEO.
 *
 * The 'Asynchronous Video Data' message buffer format depends on the
 * \ref VidDataFmtCodes "Video data format" configured by the
 * \ref DBPOD_CMDBUF_VIDEO_CONFIG::nFormat "nFormat" member of the
 * \ref DBPOD_CMDBUF_VIDEO_CONFIG "'Video Configuration' command" and should be
 * one of the following:
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8
 * "'Asynchronous Video Data (RAWGREY8) message", or
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8VT
 * "'Asynchronous Video Data (RAWGREY8VT) message".
 *
 * \remark Initially (after opening the device), asynchronous video messages
 * are disabled.  The following commands will disable asynchronous video
 * messages:
 * \ref DBPOD_CMDBUF_STOP_ASYNC_VIDEO "'Stop Asynchronous Video Data' command",
 * \ref DBPOD_CMDBUF_ABORT "'Abort' command",
 * \ref DBPOD_CMDBUF_SOFT_RESET "'Soft Reset' command".
 *
 * The expected response is a \ref DBPOD_RSPBUF_START_ASYNC_VIDEO
 * "'Start Asynchronous Video Data' response.
 */
typedef struct TAG_DBPOD_CMDBUF_START_ASYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_START_ASYNC_VIDEO;

/**
 * \brief 'Start Asynchronous Video Data' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_START_ASYNC_VIDEO
 * "'Start Asynchronous Video Data' command".  The DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to ref DBPOD_RSPCODE_START_ASYNC_VIDEO.
 */
typedef struct TAG_DBPOD_RSPBUF_START_ASYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_START_ASYNC_VIDEO;

/********************************************
 *
 * 'Stop Asynchronous Video Data' command and response buffer.
 *
 ********************************************/

/**
 * \brief 'Stop Asynchronous Video Data' command buffer.
 *
 * The 'Stop Asynchronous Video Data' command is used to disable the sending of
 * 'Asynchronous Video Data' messages by the device.  The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_STOP_ASYNC_VIDEO.
 *
 * The expected response is a \ref DBPOD_RSPBUF_STOP_ASYNC_VIDEO
 * "'Stop Asynchronous Video Data' response".
 */
typedef struct TAG_DBPOD_CMDBUF_STOP_ASYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_STOP_ASYNC_VIDEO;

/**
 * \brief 'Stop Asynchronous Video Data' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_STOP_ASYNC_VIDEO
 * "'Stop Asynchronous Video Data' command".  The \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_RSPCODE_STOP_ASYNC_VIDEO.
 */
typedef struct TAG_DBPOD_RSPBUF_STOP_ASYNC_VIDEO
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_STOP_ASYNC_VIDEO;

/********************************************
 *
 * 'Asynchronous Video Data' message.
 *
 ********************************************/

/**
 * \typedef DBPOD_MSGBUF_ASYNC_VIDEO_DUMMY
 * \brief 'Asynchronous Video Data' (DUMMY) message buffer.
 *
 * This is not a proper 'Asynchronous Video Data' message but is the common,
 * initial part of all 'Asynchronous Video Data' message formats.  The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_MSGCODE_ASYNC_VIDEO.
 *
 * Apart from the value of the <tt>hdr.wCmd</tt> member, it is the same as an
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_DUMMY
 * "'Asynchronous Video Data' (DUMMY) response".
 */
typedef DBPOD_RSPBUF_SYNC_VIDEO_DUMMY       DBPOD_MSGBUF_ASYNC_VIDEO_DUMMY;

/**
 * \typedef DBPOD_MSGBUF_ASYNC_VIDEO_UNKNOWN_FORMAT
 * \brief 'Asynchronous Video Data' (UNKNOWN FORMAT) message buffer.
 *
 * This is an 'Asynchronous Video Data' message when video capture is
 * configured with an unknown \ref VidDataFmtCodes "video data format".  It
 * should not occur in practice.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd"
 * member is set to \ref DBPOD_MSGCODE_ASYNC_VIDEO.
 *
 * Apart from the value of the <tt>hdr.wCmd</tt> member, it is the same as an
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT
 * "'Asynchronous Video Data' (UNKNOWN FORMAT) response".
 */
typedef DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT  \
            DBPOD_MSGBUF_ASYNC_VIDEO_UNKNOWN_FORMAT;

/**
 * \brief Basic size of \ref DBPOD_MSGBUF_ASYNC_VIDEO_UNKNOWN_FORMAT without
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_UNKNOWN_FORMAT::bData "bData[]".
 *
 * Followed by <tt>UCHAR bData[len]</tt> bytes of video data of length up to
 * the end of the message and possible padding.
 */
#define BASE_SIZE_DBPOD_MSGBUF_ASYNC_VIDEO_UNKNOWN_FORMAT   \
    BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_UNKNOWN_FORMAT

/**
 * \typedef DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8
 * \brief 'Asynchronous Video Data' (RAWGREY8) message buffer.
 *
 * This is an 'Asynchronous Video Data' message when video capture is
 * configured for RAWGREY8 video data format.  The \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_MSGCODE_ASYNC_VIDEO.
 *
 * Apart from the value of the <tt>hdr.wCmd</tt> member, it is the same as an
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8
 * "'Asynchronous Video Data' (RAWGREY8) response".
 */
typedef DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8    DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8;

/**
 * \brief Basic size of \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8 without
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8::bPixels "bPixels[]".
 *
 * Followed by array <tt>UCHAR bPixels[nWidth*nHeight]</tt> of 8-bit pixel
 * data.
 */
#define BASE_SIZE_DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8 \
    BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8

/**
 * \typedef DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8VT
 * \brief 'Asynchronous Video Data' (RAWGREY8VT) message buffer.
 *
 * This is an 'Asynchronous Video Data' message when video capture is
 * configured for RAWGREY8VT video data format.  The \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_MSGCODE_ASYNC_VIDEO.
 *
 * Apart from the value of the <tt>hdr.wCmd</tt> member, it is the same as an
 * \ref DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT
 * "'Asynchronous Video Data' (RAWGREY8VT) response".
 */
typedef DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT  DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8VT;

/**
 * \brief Basic size of \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8VT without
 * \ref DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8::bPixels "bPixels[]".
 *
 * Followed by array <tt>UCHAR bPixels[nWidth*nHeight]</tt> of 8-bit pixel
 * data.
 */
#define BASE_SIZE_DBPOD_MSGBUF_ASYNC_VIDEO_RAWGREY8VT \
    BASE_SIZE_DBPOD_RSPBUF_SYNC_VIDEO_RAWGREY8VT

/********************************************
 *
 * Generic UT data message.
 *
 * Used for 'Synchronous Record Data' response, 'Asynchronous Record Data'
 * message, 'Synchronous Display Data' response.
 *
 ********************************************/

/**
 * \brief Interface/Peak format.
 *
 * This is the format for Interface gate data and each peak of Peak gate data
 * at the \ref DBPOD_CHUNK_UT_EOC "end of a UT data chunk".
 */
typedef struct TAG_DBPOD_PEAK
{
    LONG    Amplitude;              /**< Peak amplitude. */
    LONG    Position;               /**< Peak position. */
} DBPOD_PEAK;

/**
 * \brief 'Generic UT Data' start of message buffer.
 *
 * This is the format of the start of a 'Synchronous Record Data' response,
 * 'Synchronous Display Data' response, or 'Asynchronous Record Data' message,
 * which are the same format, but differ in the value of the
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member.
 *
 * The start of message is followed by zero or more UT data chunks.  The number
 * of chunks in the message is indicated by the value of the #nChunks member
 * (but for a 'Synchronous Display Data' response, #nChunks will be 0 or 1).
 *
 * Each UT data chunk is produced by a single sequence table entry.  A UT
 * data chunk consists of a \ref DBPOD_CHUNK_UT_SOC "start of chunk" record,
 * followed by an arbitrary amount of sample data (which will be a multiple of
 * 4 bytes), followed by an \ref DBPOD_CHUNK_UT_EOC "end of chunk" record.
 *
 * Here is an example UT record data message composed of two chunks. The first
 * chunk contains 1024 bytes of UT data. The second chunk contains 532 bytes
 * of UT data.
 *
 * | Offset | Length | Field                       | Value                   |
 * | -----: | -----: | :----                       | :----                   |
 * |      0 |      4 | \c hdr.dwLength             | 1888 (= 1892 &minus; 4) |
 * |      4 |      4 | \c hdr.dwSequence           | ?                       |
 * |      8 |      2 | \c hdr.wCmd                 | ?                       |
 * |     10 |      2 | \c hdr.wSubCode             | 0                       |
 * |     12 |      4 | \c nChunks                  | 2                       |
 * |     16 | (1184) | <b>(First chunk ...)</b>    |                         |
 * |     16 |   (20) | <i>(First chunk SOC)</i>    |                         |
 * |     16 |      4 | \c dwLength                 | 1180 (= 1200 &minus; 16 &minus; 4) |
 * |     20 |      4 | \c dwSeq                    | ?                       |
 * |     24 |      4 | <tt>lQuadPos[0]</tt>        | ?                       |
 * |     28 |      4 | <tt>lQuadPos[1]</tt>        | ?                       |
 * |     32 |      4 | \c dwVt or \c vt            | ?                       |
 * |     36 | (1024) | <i>(First chunk data)</i>   |                         |
 * |     36 |   1024 | <tt>Data[]</tt>             | ?                       |
 * |   1060 |  (140) | <i>(First chunk EOC)</i>    |                         |
 * |   1060 |      4 | \c fLossSig                 | ?                       |
 * |   1064 |      4 | \c Interface.Amplitude      | ?                       |
 * |   1068 |      4 | \c Interface.Position       | ?                       |
 * |   1072 |      4 | <tt>Peak[0].Amplitude</tt>  | ?                       |
 * |   1076 |      4 | <tt>Peak[0].Position</tt>   | ?                       |
 * |        |        | ...                         |                         |
 * |   1192 |      4 | <tt>Peak[15].Amplitude</tt> | ?                       |
 * |   1196 |      4 | <tt>Peak[15].Position</tt>  | ?                       |
 * |   1200 |  (692) | <b>(Second chunk ...)</b>   |                         |
 * |   1200 |     20 | <i>(Second chunk SOC)</i>   | (\c dwLength = 688)     |
 * |   1220 |    532 | <i>(Second chunk data)</i>  | ?                       |
 * |   1752 |    140 | <i>(Second chunk EOC)</i>   |                         |
 * |   1892 |    (0) | <b>(End of message)</b>     |                      |
 *
 */
typedef struct TAG_DBPOD_MSGBUF_UT_SOM
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG       nChunks;            /**< Number of chunks in message. */
} DBPOD_MSGBUF_UT_SOM;
/* Followed by nChunks variable length chunks. */

/**
 * \brief Start of UT data chunk.
 *
 * This 20 byte header (up to the start of the #Data array) occurs at the
 * start of each UT data chunk contained within a \ref DBPOD_MSGBUF_UT_SOM
 * "UT data message".  (A UT data message could be a 'Synchronous Record Data'
 * response, a 'Synchronous Display Data' response, or an 'Asynchronous Record
 * Data' message.)
 *
 * - The whole UT data chunk is #dwLength + 4 bytes long.
 * - The start of chunk record is 20 bytes long.
 * - The UT data starts 20 bytes from the start of chunk at the #Data array
 *   member and is #dwLength &minus; 156 bytes long.  This will be a multiple
 *   of 4 bytes long.
 * - The \ref DBPOD_CHUNK_UT_EOC "end of chunk" record starts #dwLength &minus;
 *   136 bytes from the start of chunk and is 140 bytes long.
 */
typedef struct TAG_DBPOD_CHUNK_UT_SOC
{
    ULONG       dwLength;           /**< Non-inclusive length of chunk
                                     * (#dwSeq onwards). */
    ULONG       dwSeq;              /**< Sequence table index. */
    LONG        lQuadPos[2];        /**< Quadrature readings. */
    union
    {
        DBPOD_VIDTRK   vt;          /**< Video tracking info as a \c struct. */
        ULONG       dwVt;           /**< \ref VidTrkULONG
                                     * "Video tracking info as \c ULONG". */
    };                              /* Anonymous union of 'vt' and 'dwVt'. */
    signed char Data[DBPOD_ANYLENGTH]; /**< Start of UT data. */
} DBPOD_CHUNK_UT_SOC;

/**
 * \brief Basic size of \ref DBPOD_CHUNK_UT_SOC type without
 * \ref DBPOD_CHUNK_UT_SOC::Data "Data[]".
 *
 * Followed by \c dwLength &minus; 156 bytes of UT data (where \c dwLength is
 * from the \ref DBPOD_CHUNK_UT_SOC::dwLength "dwLength" member of
 * \ref DBPOD_CHUNK_UT_SOC and is a multiple of 4) and an
 * \ref DBPOD_CHUNK_UT_EOC "end of chunk" record.
 */
#define BASE_SIZE_DBPOD_CHUNK_UT_SOC \
    FIELD_OFFSET(DBPOD_CHUNK_UT_SOC, Data[0])

/**
 * \brief End of UT data chunk.
 *
 * This 140 byte footer occurs on a 4-byte boundary at the end of each UT data
 * chunk contained within a \ref DBPOD_MSGBUF_UT_SOM "UT data message".
 * (A UT data message could be a 'Synchronous Record Data' response, a
 * 'Synchronous Display Data' response, or an 'Asynchronous Record Data'
 * message.)  It encodes data for the 'Loss Of Signal', 'Interface', and
 * 'Peak' gates.
 *
 * \note If \ref DBPOD_START_UT_EXTRA_ENCS was set in the
 * \ref DBPOD_CMDBUF_START_UT::dwFlags "dwFlags" member of the
 * \ref DBPOD_CMDBUF_START_UT "'Start UT' command" when starting UT
 * acquisition, the final elements of the #Peak array member may be replaced
 * with readings from the extra quadrature encoders that do not fit in the
 * \ref DBPOD_CHUNK_UT_SOC::lQuadPos "lQuadPos" array member of
 * \ref DBPOD_CHUNK_UT_SOC.
 */
typedef struct TAG_DBPOD_CHUNK_UT_EOC
{
    /**
     * Loss of signal flag:
     * - \c FALSE = loss of signal not detected.
     * - \c TRUE = loss of signal detected.
     */
    LONG        fLossSig;
    /** Interface gate position and amplitude. */
    DBPOD_PEAK Interface;
    /** Positions and amplitudes of detected peaks. (Unused peaks are zero.) */
    DBPOD_PEAK Peak[16];
} DBPOD_CHUNK_UT_EOC;

/********************************************
 *
 * 'Synchronous Record Data' command and response.
 *
 ********************************************/

/**
 * \brief 'Synchronous Record Data' command buffer.
 *
 * The 'Synchronous Record Data' command is used to request the next unsent
 * chunks of UT record data.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member
 * is set to \ref DBPOD_CMDCODE_SYNC_RECORD.
 *
 * \remark If any unsent record data is available, the response will contain
 * as many unsent UT data chunks will fit in the preferred buffer size
 * indicated by #dwBufferSize, but at least one chunk.
 *
 * \remark If no unsent record data is available, the response will contain
 * zero chunks of UT data.
 *
 * The expected response is a \ref DBPOD_RSPBUF_SYNC_RECORD_SOM
 * "'Synchronous Record Data' response".
 *
 * \remark An alterative to the 'Synchronous Record Data' command is to use
 * the \ref DBPOD_CMDBUF_START_ASYNC_RECORD
 * "'Start Asynchronous Record Data' command" and let thedevice send
 * \ref DBPOD_MSGBUF_ASYNC_RECORD_SOM "'Asynchronous Record Data' messages".
 */
typedef struct TAG_DBPOD_CMDBUF_SYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG       dwBufferSize;       /**< Preferred buffer size in bytes. */
} DBPOD_CMDBUF_SYNC_RECORD;

/**
 * \typedef DBPOD_RSPBUF_SYNC_RECORD_SOM
 * \brief 'Synchronous Record Data' response buffer.
 *
 * This is the normal response to the \ref DBPOD_CMDBUF_SYNC_RECORD
 * "'Synchronous Record Data' command".  Its format is a
 * \ref DBPOD_MSGBUF_UT_SOM "'Generic UT Data' message buffer".
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_RSPCODE_SYNC_RECORD.
 *
 * \remark If no unsent UT record data is available, the
 * \ref DBPOD_MSGBUF_UT_SOM::nChunks "nChunks" member will be 0.
 */
typedef DBPOD_MSGBUF_UT_SOM     DBPOD_RSPBUF_SYNC_RECORD_SOM;
/**
 * \typedef UPLUS_RSPBUF_SYNC_RECORD_SOM
 * \brief Legacy typedef.  Same as \ref DBPOD_RSPBUF_SYNC_RECORD_SOM.
 */
typedef DBPOD_MSGBUF_UT_SOM     UPLUS_RSPBUF_SYNC_RECORD_SOM;

/********************************************
 *
 * 'Start Asynchronous Record Data' command and response buffer.
 *
 ********************************************/

/**
 * \brief 'Start Asynchronous Record Data' command buffer.
 *
 * The 'Start Asynchronous Record Data' command is used to enable the sending
 * of \ref DBPOD_MSGBUF_ASYNC_RECORD_SOM
 * "'Asynchronous Record Data' messages" by the device when it has unsent
 * UT record data available.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member
 * is set to \ref DBPOD_CMDCODE_START_ASYNC_RECORD.
 *
 * \remark The 'Asynchronous Record Data' messages will contain at least one
 * chunk of unsent UT record data, but may contain as many unsent chunks as
 * will fit in the preferred buffer size indicated by the #dwBufferSize
 * member.
 *
 * \remark Initially (after opening the device), asynchronous record data
 * messages are disabled.  The following commands will disable asynchronous
 * record data messages:
 * \ref DBPOD_CMDBUF_STOP_ASYNC_RECORD
 * "'Stop Asynchronous Record Data' command",
 * \ref DBPOD_CMDBUF_ABORT "'Abort' command",
 * \ref DBPOD_CMDBUF_SOFT_RESET "'Soft Reset' command".
 *
 * The expected response is a \ref DBPOD_RSPBUF_START_ASYNC_RECORD
 * "'Start Asynchronous Record Data' response".
 */
typedef struct TAG_DBPOD_CMDBUF_START_ASYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG       dwBufferSize;       /**< Preferred buffer size in bytes. */
} DBPOD_CMDBUF_START_ASYNC_RECORD;

/**
 * \brief 'Start Asynchronous Record Data' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_START_ASYNC_RECORD
 * "'Start Asynchronous Record Data' command".  The \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_RSPCODE_START_ASYNC_RECORD.
 */
typedef struct TAG_DBPOD_RSPBUF_START_ASYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_START_ASYNC_RECORD;

/********************************************
 *
 * 'Stop Asynchronous Record Data' command and response.
 *
 ********************************************/

/**
 * \brief 'Stop Asynchronous Record Data' command buffer.
 *
 * The 'Stop Asynchronous Record Data' command is used to disable the sending
 * of \ref DBPOD_MSGBUF_ASYNC_RECORD_SOM "'Asynchronous Record Data' messages"
 * by the device.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_STOP_ASYNC_RECORD.
 *
 * The expected response is a \ref DBPOD_RSPBUF_STOP_ASYNC_RECORD
 * "'Stop Asynchronous Record Data' response.
 */
typedef struct TAG_DBPOD_CMDBUF_STOP_ASYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_STOP_ASYNC_RECORD;

/**
 * \brief 'Stop Asynchronous RECORD Data' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_STOP_ASYNC_RECORD
 * "'Stop Asynchronous Record Data' command".  The \ref DBPOD_MSGHDR::wCmd
 * "hdr.wCmd" member is set to \ref DBPOD_RSPCODE_STOP_ASYNC_RECORD.
 */
typedef struct TAG_DBPOD_RSPBUF_STOP_ASYNC_RECORD
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_STOP_ASYNC_RECORD;

/********************************************
 *
 * 'Asynchronous Record Data' message.
 *
 ********************************************/

/**
 * \typedef DBPOD_MSGBUF_ASYNC_RECORD_SOM
 * \brief 'Asynchronous Record Data' message buffer.
 *
 * The format of 'Asynchronous Record Data' messages is a
 * \ref DBPOD_MSGBUF_UT_SOM "'Generic UT Data' message buffer".
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_MSGCODE_ASYNC_RECORD.
 */
typedef DBPOD_MSGBUF_UT_SOM   DBPOD_MSGBUF_ASYNC_RECORD_SOM;
/**
 * \typedef UPLUS_MSGBUF_ASYNC_RECORD_SOM
 * \brief Legacy typedef.  Same as \ref DBPOD_MSGBUF_ASYNC_RECORD_SOM.
 */
typedef DBPOD_MSGBUF_UT_SOM   UPLUS_MSGBUF_ASYNC_RECORD_SOM;

/********************************************
 *
 * 'Synchronous Display Data' command and response.
 *
 * Gets the latest, previously unsent data for a specific sequence table
 * entry.  The response will contain at most one chunk.
 *
 ********************************************/

/**
 * \brief 'Synchronous Display Data' command buffer.
 *
 * The 'Synchronous Display Data' command is used to select which sequence
 * table entry to monitor for display data and to return the latest unsent
 * display data (if any) fot that sequence table entry.  The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_SYNC_DISPLAY.
 *
 * \remark The dbPOD only monitors display data for a single sequence table
 * entry at once, so if #dwSeq has changed since the previous 'Synchronous
 * Display Data' command, or if this is the first 'Synchronous Display Data'
 * command since starting UT acquisition, then there will be no display data
 * available.
 *
 * \remark The latest display data is discarded by the dbPOD once it has been
 * sent in a 'Synchronous Display Data' response.
 *
 * The expected response is a \ref DBPOD_RSPBUF_SYNC_DISPLAY_SOM
 * "'Synchronous Display Data' response.
 */
typedef struct TAG_DBPOD_CMDBUF_SYNC_DISPLAY
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
    ULONG       dwSeq;              /**< Sequence table index. */
} DBPOD_CMDBUF_SYNC_DISPLAY;

/* 'Synchronous Display Data' response buffer is same as 'Generic UT Data'
 * message buffer and will contain at most one chunk. */

/**
 * \brief 'Synchronous Display Data' response buffer.
 *
 * This is the normal response to the \ref DBPOD_CMDBUF_SYNC_DISPLAY
 * "'Synchronous Display Data' command".  Its format is a
 * \ref DBPOD_MSGBUF_UT_SOM "'Generic UT Data' message buffer".
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_RSPCODE_SYNC_DISPLAY.
 *
 * \remark If no display data is available, the
 * \ref DBPOD_MSGBUF_UT_SOM::nChunks "nChunks" member will be 0.  If display
 * data is available for the currently monitored sequence table entry, the
 * \c nChunks member will be 1.
 */
typedef DBPOD_MSGBUF_UT_SOM     DBPOD_RSPBUF_SYNC_DISPLAY_SOM;
/**
 * \typedef UPLUS_RSPBUF_SYNC_DISPLAY_SOM
 * \brief Legacy typedef.  Same as \ref DBPOD_RSPBUF_SYNC_DISPLAY_SOM.
 */
typedef DBPOD_MSGBUF_UT_SOM     UPLUS_RSPBUF_SYNC_DISPLAY_SOM;

/********************************************
 *
 * 'Asynchronous Error' message.
 *
 ********************************************/

/**
 * \brief 'Asynchronous Error' message buffer.
 *
 * The 'Asynchronous Error' message may be sent asynchronously by the dbPOD
 * to report an error.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set
 * to \ref DBPOD_MSGCODE_ASYNC_ERROR.
 *
 * \note This is not currently used by dbPOD.
 */
typedef struct TAG_DBPOD_MSGBUF_ASYNC_ERROR
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_MSGBUF_ASYNC_ERROR;

/********************************************
 *
 * 'Abort' command and response.
 *
 ********************************************/

/**
 * \brief 'Abort' command buffer.
 *
 * The 'Abort' command is used to abort UT data acquisition and video capture
 * and discard unread data.  It also disables the sending of asynchronous
 * video data messages and asynchronous record data messages.  The
 * \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_CMDCODE_ABORT.
 *
 * The expected response is a \ref DBPOD_RSPBUF_ABORT "'Abort' response".
 */
typedef struct TAG_DBPOD_CMDBUF_ABORT
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_ABORT;

/**
 * \brief 'Abort' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_ABORT "'Abort' command".
 * The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is set to
 * \ref DBPOD_RSPCODE_ABORT.
 */
typedef struct TAG_DBPOD_RSPBUF_ABORT
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_ABORT;

/********************************************
 *
 * 'Soft Reset' command and response.
 *
 ********************************************/

/**
 * \brief 'Soft Reset' command buffer.
 *
 * The 'Soft Reset' command is used to set the dbPOD to the initial state as
 * though it was just opened.  It aborts UT data acquision and video capture,
 * discards unread data, unconfigures video capture, and unconfigures all
 * sequence table entries.  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is
 * set to \ref DBPOD_CMDCODE_SOFT_RESET.
 *
 * The expected response is a \ref DBPOD_RSPBUF_SOFT_RESET
 * "'Soft Reset' response.
 */
typedef struct TAG_DBPOD_CMDBUF_SOFT_RESET
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_CMDBUF_SOFT_RESET;

/**
 * \brief 'Soft Reset' response buffer.
 *
 * This is the response to the \ref DBPOD_CMDBUF_SOFT_RESET
 * "'Soft Reset' command".  The \ref DBPOD_MSGHDR::wCmd "hdr.wCmd" member is
 * set to \ref DBPOD_RSPCODE_SOFT_RESET.
 */
typedef struct TAG_DBPOD_RSPBUF_SOFT_RESET
{
    DBPOD_MSGHDR hdr;               /**< Message header. */
} DBPOD_RSPBUF_SOFT_RESET;

/* Restore warnings to previous setting */
#ifdef _MSC_VER
#pragma warning (pop)
#endif /* _MSC_VER */

/* Restore packing to previous setting */
#if defined(_MSC_VER) || defined(__GNUC__)
//#pragma pack(pop)
#endif /* _MSC_VER */

/**
 * \}
 */

#if defined(__cplusplus)
}
#endif

/* vi: set ai et sw=4 ts=4: */
#endif
