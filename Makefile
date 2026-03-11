DBPOD_EXES = dummy encoder_test getcaps mdutest recdata1 recdata16 \
	     recdata16_mini recdata16_minipa recdata16_minipa_gains \
	     recdata16_minipa_gains_single_element recdata16_t1 \
	     recdata1_16 recdata1_mdutest recdata1_mini \
	     recdata1_mini_extra_encs recdata1_mini_t1 recdata1_mini_t2 \
	     recdata1_t recdata1_t2 recdata1_t3 recdata1_t4 recdata1_tif \
	     recdata1_vtpal recdata1a recdata256 recdata256_mini \
	     recdata64 recdata64_mini recdata64diff recdata64l recdata8 \
	     set_led vidcapture vidcapture_vt vidconfig vidconfig_ntsc \
	     vidconfig_pal
PAUTPOD_EXES = pulse_shape_test recdata1_16_pulse recdata1_16_pulse_pa
UTIL_EXES = diggaincalc

DBPOD_HDRS = dbpod_coms.h dbpod_errs.h dbpod_wtypes.h
PAUTPOD_HDRS = pautpod_coms.h pautpod_errs.h pautpod_wtypes.h

DBPOD_OBJS = $(patsubst %,%.o,$(DBPOD_EXES))
PAUTPOD_OBJS = $(patsubst %,%.o,$(PAUTPOD_EXES))
UTIL_OBJS = $(patsubst %,%.o,$(UTIL_EXES))

CFLAGS = -O -g
LDLIBS = -lm

ALL_EXES = $(DBPOD_EXES) $(PAUTPOD_EXES) $(UTIL_EXES)

all: $(ALL_EXES)

clean:
	-rm -f $(DBPOD_EXES) $(DBPOD_OBJS)
	-rm -f $(PAUTPOD_EXES) $(PAUTPOD_OBJS)
	-rm -f $(UTIL_EXES) $(UTIL_OBJS)

$(DBPOD_OBJS):: $(DBPOD_HDRS)
$(PAUTPOD_OBJS):: $(PAUTPOD_HDRS)

.PHONY: all clean
