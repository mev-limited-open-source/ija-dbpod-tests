#ifndef PAUTPOD_ERRS_H__INCLUDED
#define PAUTPOD_ERRS_H__INCLUDED

/*
 * $Id$
 *
 * db-Pod Error Numbers
 *
 * Copyright (c) 2008-2015 MEV Ltd., Bell Technology Ltd.
 * All rights reserved.
 *
 * MODULE CONTENTS
 *
 * dbpod_errs.h
 *
 * DESCRIPTION
 *
 * Error numbers used in db-Pod error response messages.
 *
 * AMENDMENT RECORD
 *
 * $Log$
 *
 */

/*
 * N.B. The error numbers are fairly random, but where possible have the
 * same values as corresponding Win32 error numbers for db-UT.
 */

/* Command too short. */
#define DBPOD_ERR_COMMAND_TOO_SHORT             10000

/* Command not supported. */
#define DBPOD_ERR_COMMAND_NOT_SUPPORTED         10001

/* System error. */
#define DBPOD_ERR_SYSTEM_ERROR                  10002

/* Sequence index out of range. */
#define DBPOD_ERR_SEQ_IDX_RANGE                 1793

/* Sequence delay out of range. */
#define DBPOD_ERR_SEQ_DELAY_RANGE               1909

/* Sequence repeats out of range. */
#define DBPOD_ERR_SEQ_REPEAT_RANGE              1327

/* Physical Tx/Rx channel out of range. */
#define DBPOD_ERR_TXRX_RANGE                    1331

/* Physical Tx/Rx element specified more than once (for phased array). */
#define DBPOD_ERR_TXRX_REPEAT_IDX               1314

/* Physical Tx/Tx element timing out of range (for phased array). */
#define DBPOD_ERR_TXRX_TIME_OFFSET_RANGE        1397

/* Voltage out of range. */
#define DBPOD_ERR_VOLTAGE_RANGE                 1361

/* Fire pulse width period out of range. */
#define DBPOD_ERR_FIRE_PERIOD_RANGE             1346

/* Sample width (in bits) not supported. */
#define DBPOD_ERR_CHAN_SAMPLE_BITS              1056

/* Channel gain out of range. */
#define DBPOD_ERR_CHAN_GAIN_RANGE               1365

/* Sequence repeats value out of range. */
#define DBPOD_ERR_REPEAT_RANGE                  1239

/* High pass filter value not supported. */
#define DBPOD_ERR_HPF_VALUE                     1240

/* Low pass filter value not supported. */
#define DBPOD_ERR_LPF_VALUE                     1326

/* Rectifier type not supported. */
#define DBPOD_ERR_RECTIFIER_TYPE                1380

/* Post-rectification digital low pass filter value not supported. */
#define DBPOD_ERR_RECTIFIER_LPF                 1334

/* Sample frequency not supported. */
#define DBPOD_ERR_SAMPLE_FREQ                   1366

/* DAC curve out of bounds. */
#define DBPOD_ERR_DAC_CURVE_BOUNDARY            1363

/* DAC curve not not aligned. */
#define DBPOD_ERR_DAC_CURVE_ALIGN               1385

/* DAC sample rate divisor value not supported. */
#define DBPOD_ERR_DAC_RATE_DIVISOR              1358

/* DAC trigger type not supported. */
#define DBPOD_ERR_DAC_TRIG_TYPE                 1348

/* Bad gate count. */
#define DBPOD_ERR_GATE_COUNT                    1368

/* Unsupported gate type. */
#define DBPOD_ERR_GATE_TYPE                     1351

/* Gate threshold out of range. */
#define DBPOD_ERR_GATE_THRESHOLD_RANGE          1310

/* Gate out of bounds. */
#define DBPOD_ERR_GATE_BOUNDARY                 1347

/* Sweep length too long. */
#define DBPOD_ERR_SWEEP_LENGTH                  1360

/* UT acquisition (already) running. */
#define DBPOD_ERR_UT_STARTED                    1236

/* Video capture (already) running. */
#define DBPOD_ERR_VC_STARTED                    1107

/* Bad gate reference. */
#define DBPOD_ERR_GATE_REFERENCE                1230

/* Interface gate gain out of range. */
#define DBPOD_ERR_GATE_IF_GAIN                  1238

/* Interface gate polarity unsupported. */
#define DBPOD_ERR_GATE_IF_POLARITY              1357

/* Average gate average depth unsupported. */
#define DBPOD_ERR_GATE_AVERAGE                  1229

/* Peak gate number of peaks out of range. */
#define DBPOD_ERR_GATE_PEAK_NUM                 1225

/* Peak gate type of peaks unsupported. */
#define DBPOD_ERR_GATE_PEAK_TYPE                1908

/* Peak gate peak discriminator out of range. */
#define DBPOD_ERR_GATE_PEAK_DISCRIM             1356

/* Loss of signal gate polarity unsupported. */
#define DBPOD_ERR_GATE_LOSS_POLARITY            1810

/* Gates defined but no receiver. */
#define DBPOD_ERR_NO_RX                         1124

/* Start index out of range. */
#define DBPOD_ERR_START_IDX                     1125

/* End index out of range. */
#define DBPOD_ERR_END_IDX                       1122

/* Sequence includes an unconfigured channel. */
#define DBPOD_ERR_CHAN_UNCONFIGURED             1123

/* dbUT channels in sequence have different voltages. */
#define DBPOD_ERR_CHAN_VOLTAGE_MIX              1007

/* dbUT channels in sequence have different sample widths. */
#define DBPOD_ERR_CHAN_SAMPLE_BITS_MIX          1219

/* dbUT channel in sequence includes an unconfigured fire pulse shape. */
#define DBPOD_ERR_CHAN_FIRE_PULSE_UNCONFIGURED  1313

/* Sequence uses too much power. */
#define DBPOD_ERR_POWER                         1226

/* Insufficient averaging memory for sequence. */
#define DBPOD_ERR_AVERAGE_MEMORY                1383

/* UT acquisition (already) stopped. */
#define DBPOD_ERR_UT_STOPPED                    1318

/* Video capture (already) stopped. */
#define DBPOD_ERR_VC_STOPPED                    1324

/* Failed to update calibration data. */
#define DBPOD_ERR_CALIBRATION_FAIL              1315

/* Unsupported video standard (nStandard). */
#define DBPOD_ERR_VIDEO_STANDARD                1354

/* Unsupported video data format (nFormat). */
#define DBPOD_ERR_VIDEO_DATA_FORMAT             1353

/* Unsupported video tracking frame interleave value (nInterleave). */
#define DBPOD_ERR_VIDEO_INTERLEAVE              1345

/* Video tracking frame rate out of range (nTrkFrameRate). */
#define DBPOD_ERR_VIDEO_TRACK_RATE              1343

/* Video tracking frame size out of range (nXTrkFrameSize, nYTrkFrameSize). */
#define DBPOD_ERR_VIDEO_TRACK_FRAME_SIZE        1328

/* Video tracking frame cropping out of range (nXTrkOrigin, nYTrkOrigin,
 * nXTrkCropSize, nYTrkCropSize). */
#define DBPOD_ERR_VIDEO_TRACK_FRAME_CROP        1367

/* Video capture frame size scale out of range (nStrmSizeScale). */
#define DBPOD_ERR_VIDEO_CAPTURE_SIZE_SCALE      1308

/* Video capture frame rate scale out of range (nStrmRateScale). */
#define DBPOD_ERR_VIDEO_CAPTURE_RATE_SCALE      1335

/* LED drive/gain value out of range (nAGCThreshold). */
#define DBPOD_ERR_VIDEO_LED_RANGE               1329

/* "No dot" threshold value out of range (nNoDotThreshold). */
#define DBPOD_ERR_VIDEO_NO_DOT_RANGE            1320

/* Hardware problem configuring video. */
#define DBPOD_ERR_VIDEO_HARDWARE                1322

/* Video not configured successfully. */
#define DBPOD_ERR_VIDEO_NOT_CONFIGURED          1395

/* Video capture disabled. */
#define DBPOD_ERR_VIDEO_CAPTURE_DISABLED        1390

/* Failed to update serial number. */
#define DBPOD_ERR_SERIAL_NUMBER_FAIL            1378

/* Unsupported scan type. */
#define DBPOD_ERR_SCAN_TYPE                     1227

/* Unsupported scan axis. */
#define DBPOD_ERR_SCAN_AXIS                     1228

/* Bad scan step. */
#define DBPOD_ERR_SCAN_STEP                     1379

/* vi: set ai et sw=4 ts=4: */
#endif
