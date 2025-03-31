CC = cc
COPTS = 
LIBS = -lm -lc
PROGRAMS = survey_sim make_sequence get_ecliptic galactic get_time_gaps \
	make_sequence1 scheduler  time_stamp sequencer get_airmass make_histogram \
	get_time_history 
#	scheduler_new get_time_history

OBJECTS = scheduler.o scheduler_telescope.o scheduler_camera.o socket.o \
         sky_utils.o ecliptic.o scheduler_fits.o scheduler_corrections.o \
	 scheduler_signals.o 

OBJECTS_NEW = scheduler_new.o scheduler_telescope.o scheduler_camera.o socket.o \
         sky_utils.o ecliptic.o scheduler_fits.o scheduler_corrections.o \
	 scheduler_signals.o

SEQUENCER_OBJECTS = sequencer.o scheduler_telescope.o scheduler_camera.o socket.o \
         sky_utils.o ecliptic.o scheduler_fits.o scheduler_corrections.o \
	 scheduler_signals.o


GET_AIRMASS_OBJECTS = get_airmass.o sky_utils.o

.c.o: 
	$(CC) $(COPTS) -c $<

all: $(PROGRAMS)


#test: test.o scheduler_telescope.o socket.o sky_utils.o ecliptic.o
#	$(CC) $(COPTS) -o test test.o scheduler_telescope.o \
#	socket.o sky_utils.o ecliptic.o $(LIBS)

almanac: almanac.o sky_utils.o
	 $(CC) $(COPTS) -o almanac almanac.o sky_utils.o $(LIBS)

make_histogram:	make_histogram.o
	$(CC) $(COPTS) -o make_histogram make_histogram.o -lm

scheduler: $(OBJECTS)
	 $(CC) $(COPTS) -o scheduler $(OBJECTS) $(LIBS)

scheduler_new: $(OBJECTS_NEW)
	 $(CC) $(COPTS) -o scheduler_new $(OBJECTS_NEW) $(LIBS)

sequencer: $(SEQUENCER_OBJECTS)
	 $(CC) $(COPTS) -o sequencer $(SEQUENCER_OBJECTS) $(LIBS)

get_airmass: $(GET_AIRMASS_OBJECTS)
	 $(CC) $(COPTS) -o get_airmass $(GET_AIRMASS_OBJECTS) $(LIBS)

#scheduler: scheduler.o scheduler_telescope.o scheduler_camera.o socket.o \
#	 sky_utils.o ecliptic.o scheduler_fits.o scheduler_corrections.o
#	$(CC) $(COPTS) -o scheduler scheduler.o scheduler_telescope.o \
#	scheduler_camera.o socket.o sky_utils.o ecliptic.o \
#	scheduler_fits.o scheduler_corrections.o $(LIBS)

survey_sim: survey_sim.o sky_utils.o ecliptic.o
	$(CC) $(COPTS) -o survey_sim survey_sim.o sky_utils.o ecliptic.o $(LIBS)

make_sequence: make_sequence.o  ecliptic.o
	$(CC) $(COPTS) -o make_sequence make_sequence.o ecliptic.o $(LIBS)

make_sequence1: make_sequence1.o  ecliptic.o
	$(CC) $(COPTS) -o make_sequence1 make_sequence1.o ecliptic.o $(LIBS)

get_ecliptic: get_ecliptic.o  ecliptic.o
	$(CC) $(COPTS) -o get_ecliptic get_ecliptic.o ecliptic.o $(LIBS)

galactic: galactic.o 
	$(CC) $(COPTS) -o galactic galactic.o $(LIBS)

time_stamp: time_stamp.o 
	$(CC) $(COPTS) -o time_stamp time_stamp.o $(LIBS)

get_time_gaps: get_time_gaps.o
	$(CC) $(COPTS) -o get_time_gaps get_time_gaps.o $(LIBS)

get_time_history: get_time_history.o
	$(CC) $(COPTS) -o get_time_history get_time_history.o $(LIBS)


clean: 
	rm -f $(PROGRAMS) *.o 

install:
	cp $(PROGRAMS) ../bin





