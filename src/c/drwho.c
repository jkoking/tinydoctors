#include "pebble.h"

static Window *window;

// time layer
TextLayer *text_time_layer;
TextLayer *text_date_layer;
static GFont time_font;
static GFont date_font;

// battery status
static uint8_t battery_level;
static bool battery_plugged;
static Layer *battery_layer;

// timer for 'animation'
AppTimer *timer_handle;
static void handle_timer(void* data);
int cycleCounter = 0;
static bool regeneration = false;

// BT connectivity
static bool start = true;	//variable checks if watchface is initiated
//static bool inverter_active;
//int timer = 0;

// rand generator
int starting_seed, combined_seed, random_number;
int actual_time, combined_time_values, random_time;
int NUM_EXTRAS = 128; //number of extras available x2 (=> 64 monsters => 128 Resource_IDs including 'blanks'... )

int sec_counter = 0;
bool monster_trigger;

/* -----------------------------IMAGE LAYERS--------------------------- */
/* BATTERY WHITE */
const int BATTERY_WHITE_IMAGE_RESOURCE_IDS[] = {
 	RESOURCE_ID_IMAGE_BATTERY_00,
  	RESOURCE_ID_IMAGE_BATTERY_10,
  	RESOURCE_ID_IMAGE_BATTERY_20,
  	RESOURCE_ID_IMAGE_BATTERY_30,
  	RESOURCE_ID_IMAGE_BATTERY_40,
  	RESOURCE_ID_IMAGE_BATTERY_50,
  	RESOURCE_ID_IMAGE_BATTERY_60,
  	RESOURCE_ID_IMAGE_BATTERY_70,
  	RESOURCE_ID_IMAGE_BATTERY_80,
  	RESOURCE_ID_IMAGE_BATTERY_90,
  	RESOURCE_ID_IMAGE_BATTERY_100,
  	RESOURCE_ID_IMAGE_BATTERY_CHARGING,
	RESOURCE_ID_IMAGE_DISCONNECTED		// DISCONNECTION IMAGE
};

/* BATTERY BLACK */
const int BATTERY_BLACK_IMAGE_RESOURCE_IDS[] = {
  	RESOURCE_ID_IMAGE_BATTERY_00,
  	RESOURCE_ID_IMAGE_BATTERY_10,
  	RESOURCE_ID_IMAGE_BATTERY_20,
  	RESOURCE_ID_IMAGE_BATTERY_30,
  	RESOURCE_ID_IMAGE_BATTERY_40,
  	RESOURCE_ID_IMAGE_BATTERY_50,
  	RESOURCE_ID_IMAGE_BATTERY_60,
  	RESOURCE_ID_IMAGE_BATTERY_70,
  	RESOURCE_ID_IMAGE_BATTERY_80,
  	RESOURCE_ID_IMAGE_BATTERY_90,
  	RESOURCE_ID_IMAGE_BATTERY_100,
	RESOURCE_ID_IMAGE_BATTERY_CHARGING,
	RESOURCE_ID_IMAGE_DISCONNECTED		// DISCONNECTION IMAGE black
};

/* ---meet the Doctor(s)--- */
const int DOCTOR_IMAGE_RESOURCE_IDS[] = {
  	RESOURCE_ID_IMAGE_DOC_12,
  	RESOURCE_ID_IMAGE_DOC_1,
  	RESOURCE_ID_IMAGE_DOC_2,
  	RESOURCE_ID_IMAGE_DOC_3,
  	RESOURCE_ID_IMAGE_DOC_4,
  	RESOURCE_ID_IMAGE_DOC_5,
  	RESOURCE_ID_IMAGE_DOC_6,
  	RESOURCE_ID_IMAGE_DOC_7,
  	RESOURCE_ID_IMAGE_DOC_8,
  	RESOURCE_ID_IMAGE_DOC_9,
  	RESOURCE_ID_IMAGE_DOC_10,
  	RESOURCE_ID_IMAGE_DOC_11,
  	RESOURCE_ID_IMAGE_DOC_12,
	RESOURCE_ID_IMAGE_SPECIAL_DOC_WAR,
	RESOURCE_ID_IMAGE_DOC_4_2,
	RESOURCE_ID_IMAGE_DOC_11_2,
};

/* ---AM/PM--- */
const int AMPM_IMAGE_RESOURCE_IDS[] = {
	RESOURCE_ID_IMAGE_WINDOWS_AMPM
};

/* EXTRAS */
const int EXTRAS_WHITE_IMAGE_RESOURCE_IDS[] = {
	RESOURCE_ID_IMAGE_EXTRA_ANGEL,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CYBERMAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_VERVOID,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_K9,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SONTARAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_WATCHER,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SLITHEEN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ADIPOSE,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SILENCE,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SILURIAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ICEWARRIOR,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_KAMELION,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MELKUR,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_KROMON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_OMEGA,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_OOD,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_QUARK,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ROBOT,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SATAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SIL,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SNOWMAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ZAGREUS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_YETI,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ZYGON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DECAYINGMASTER,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DRASHIG,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_EMPTYCHILD,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MORBIUS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_PRISONERZERO,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_WIRRN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CASSANDRA,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_RIVERSONG,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_FACEOFBOE,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CROOKEDMAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_JUDOON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SATURNYNE,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MOLEALIEN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_KANDYMAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MARA,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MALUS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_IDRIS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_NIMONCOUSIN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_VIYRAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DAVROS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DRAGON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SYCORAX,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_TREX,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SKYHEAD,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DRATHRO,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CYBERKING,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ALPHACENTAURI,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CAPTAINJACK,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DAEMON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ZARBI,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MENOPTERA,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MONOID,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_WHITEROBOT,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DRACONIAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ANDROIDREAPER,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CLOCKWORKROBOT,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_LIBERTY,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DALEK,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_TIMELORD,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_OGTARDIS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00,
	RESOURCE_ID_IMAGE_DATE_DITHER // DATE DITHER EFFECT (doesn't count as EXTRAS at all!)
};

const int EXTRAS_BLACK_IMAGE_RESOURCE_IDS[] = {
	RESOURCE_ID_IMAGE_EXTRA_ANGEL,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CYBERMAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_VERVOID,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_K9,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SONTARAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_WATCHER,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SLITHEEN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ADIPOSE,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SILENCE,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SILURIAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ICEWARRIOR,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_KAMELION,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MELKUR,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_KROMON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_OMEGA,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_OOD,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_QUARK,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ROBOT,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SATAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SIL,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SNOWMAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ZAGREUS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_YETI,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ZYGON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DECAYINGMASTER,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DRASHIG,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_EMPTYCHILD,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MORBIUS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_PRISONERZERO,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_WIRRN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CASSANDRA,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_RIVERSONG,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_FACEOFBOE,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CROOKEDMAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_JUDOON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SATURNYNE,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MOLEALIEN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_KANDYMAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MARA,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MALUS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_IDRIS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_NIMONCOUSIN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_VIYRAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DAVROS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DRAGON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SYCORAX,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_TREX,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_SKYHEAD,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DRATHRO,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CYBERKING,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ALPHACENTAURI,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CAPTAINJACK,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DAEMON,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ZARBI,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MENOPTERA,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_MONOID,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_WHITEROBOT,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DRACONIAN,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_ANDROIDREAPER,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_CLOCKWORKROBOT,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_LIBERTY,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_DALEK,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_TIMELORD,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00, RESOURCE_ID_IMAGE_EXTRA_OGTARDIS,
	RESOURCE_ID_IMAGE_EXTRA_00BLANK00,
	RESOURCE_ID_IMAGE_DATE_DITHER // DATE DITHER EFFECT (doesn't count as EXTRAS at all!)
}; 

/* ---REGENERATION--- */
const int REGEN_IMAGE_RESOURCE_IDS[] = {
	RESOURCE_ID_IMAGE_REGEN,
	RESOURCE_ID_IMAGE_REGEN_INV
};
/* -------------------------------------------------------------------- */

/* --------------------------RESOURCE LAYERS--------------------------- */
#define TOTAL_DOC_DIGITS 1
static GBitmap *doc_digits_images[TOTAL_DOC_DIGITS];
static BitmapLayer *doc_digits_layers[TOTAL_DOC_DIGITS];

#define TOTAL_EXTRAS_DIGITS 4
static GBitmap *extras_digits_images[TOTAL_EXTRAS_DIGITS];
static BitmapLayer *extras_digits_layers[TOTAL_EXTRAS_DIGITS];

#define TOTAL_REGEN_DIGITS 1
static GBitmap *regen_digits_images[TOTAL_REGEN_DIGITS];
static BitmapLayer *regen_digits_layers[TOTAL_REGEN_DIGITS];

#define TOTAL_BAT_DIGITS 4
static GBitmap *bat_digits_images[TOTAL_BAT_DIGITS];
static BitmapLayer *bat_digits_layers[TOTAL_BAT_DIGITS];

#define TOTAL_AMPM_DIGITS 1
static GBitmap *ampm_digits_images[TOTAL_AMPM_DIGITS];
static BitmapLayer *ampm_digits_layers[TOTAL_AMPM_DIGITS];
/* -------------------------------------------------------------------- */

// setting up the containers
static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
	
	GBitmap *old_image = *bmp_image;

 	*bmp_image = gbitmap_create_with_resource(resource_id);
 	GRect frame = (GRect) {
   		.origin = origin,
   		.size = gbitmap_get_bounds(*bmp_image).size
	};
 	bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
 	layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

 	if (old_image != NULL) {
 		gbitmap_destroy(old_image);
	}
}

// 'animation' handler
#define COOKIE_MY_TIMER 1

void handle_timer(void *data) {
	
	if ( regeneration == true ) {

	//Layer *window_layer = window_get_root_layer(window);
		//uint32_t cookie = (uint32_t) data;
    	//if (cookie == COOKIE_MY_TIMER) {
        //update the gear to the next frame of animation.
        cycleCounter++;
    	layer_set_hidden(bitmap_layer_get_layer(regen_digits_layers[0]), false);
        set_container_image(&regen_digits_images[0], regen_digits_layers[0], REGEN_IMAGE_RESOURCE_IDS[cycleCounter % 2], GPoint(0, 0));
        timer_handle = app_timer_register(100 /* milliseconds */, handle_timer, (void *) COOKIE_MY_TIMER);
    	//}		
	}
  	else {
   		layer_set_hidden(bitmap_layer_get_layer(regen_digits_layers[0]), true);
  		set_container_image(&regen_digits_images[0], regen_digits_layers[0], REGEN_IMAGE_RESOURCE_IDS[0], GPoint(0, 0));
  		set_container_image(&regen_digits_images[0], regen_digits_layers[0], REGEN_IMAGE_RESOURCE_IDS[1], GPoint(0, 0));
	}
}

// BT connectivity
static void handle_bluetooth(bool connected) {
	
	if (connected) {
		if (start) {
   			layer_set_hidden(bitmap_layer_get_layer(bat_digits_layers[2]), true);
   			layer_set_hidden(bitmap_layer_get_layer(bat_digits_layers[3]), true);
		}
		else {
			layer_set_hidden(bitmap_layer_get_layer(bat_digits_layers[2]), true);
   			layer_set_hidden(bitmap_layer_get_layer(bat_digits_layers[3]), true);
			vibes_double_pulse();
		}
	}
	else {
		if (start) {
		   	layer_set_hidden(bitmap_layer_get_layer(bat_digits_layers[2]), false);
			bitmap_layer_set_compositing_mode(bat_digits_layers[2], GCompOpOr);
			set_container_image(&bat_digits_images[2], bat_digits_layers[2], BATTERY_WHITE_IMAGE_RESOURCE_IDS[12], GPoint(119, 50));
	   		layer_set_hidden(bitmap_layer_get_layer(bat_digits_layers[3]), false);
    		bitmap_layer_set_compositing_mode(bat_digits_layers[3], GCompOpClear);
			set_container_image(&bat_digits_images[3], bat_digits_layers[3], BATTERY_BLACK_IMAGE_RESOURCE_IDS[12], GPoint(119, 50));
		}
		else {
		   	layer_set_hidden(bitmap_layer_get_layer(bat_digits_layers[2]), false);
			bitmap_layer_set_compositing_mode(bat_digits_layers[2], GCompOpOr);
			set_container_image(&bat_digits_images[2], bat_digits_layers[2], BATTERY_WHITE_IMAGE_RESOURCE_IDS[12], GPoint(119, 50));
	   		layer_set_hidden(bitmap_layer_get_layer(bat_digits_layers[3]), false);
    		bitmap_layer_set_compositing_mode(bat_digits_layers[3], GCompOpClear);
			set_container_image(&bat_digits_images[3], bat_digits_layers[3], BATTERY_BLACK_IMAGE_RESOURCE_IDS[12], GPoint(119, 50));

			vibes_enqueue_custom_pattern( (VibePattern) {
   				.durations = (uint32_t []) {100, 100, 100, 100, 100},
   				.num_segments = 5
			} );
		}	
	}
}

static void update_display(struct tm *current_time) {
	
	sec_counter++;
	
	if (sec_counter < random_time) { //2912

		monster_trigger = false;
	}
	else if (sec_counter == random_time) {
		
		monster_trigger = true;
		sec_counter = 0;
	}
	
	if (start == true){
		starting_seed = ((current_time->tm_sec) * 2) + NUM_EXTRAS + 321;
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "generated additional value for EXTRAS random number [%d].", starting_seed);

		actual_time = (current_time->tm_sec) + (current_time->tm_min) + 123;
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "generated additional time value random number [%d].", starting_seed);
	}

 	//if( ((current_time->tm_min == 0) && (current_time->tm_sec == 0)) || (start == true) ){
				
			set_container_image(&doc_digits_images[0], doc_digits_layers[0], DOCTOR_IMAGE_RESOURCE_IDS[(current_time->tm_hour)%12], GPoint(0, 0));
   			//APP_LOG(APP_LOG_LEVEL_DEBUG, "drawing doctor #%d", ((current_time->tm_hour)%12));
	//}
	if ( (((current_time->tm_hour) == 8) || ((current_time->tm_hour) == 20)) && ((current_time->tm_min) >= 30) ) {
			set_container_image(&doc_digits_images[0], doc_digits_layers[0], DOCTOR_IMAGE_RESOURCE_IDS[13], GPoint(0, 0));
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "drawing War Doctor");
	}
	else if ( (((current_time->tm_hour) == 4) || ((current_time->tm_hour) == 16)) && ((current_time->tm_min) >= 30) ) {
			set_container_image(&doc_digits_images[0], doc_digits_layers[0], DOCTOR_IMAGE_RESOURCE_IDS[14], GPoint(0, 0));
	}
	else if ( (((current_time->tm_hour) == 11) || ((current_time->tm_hour) == 23)) && ((current_time->tm_min) >= 30) ) {
			set_container_image(&doc_digits_images[0], doc_digits_layers[0], DOCTOR_IMAGE_RESOURCE_IDS[15], GPoint(0, 0));
	}

	//int max_monster_fun = 3600;
	int max_monster_fun = 1800;
	//int max_monster_fun = 900;
	//int max_monster_fun = 60;
	
	if ((start) || (monster_trigger))  {

		static long seed = 100;
		seed  = (((seed * 214013L + 2531011L) >> 16) & 32767);
		combined_seed = seed + starting_seed;
		random_number = combined_seed % NUM_EXTRAS;
   		APP_LOG(APP_LOG_LEVEL_DEBUG, "random EXTRAS #%d will be displayed...", random_number);

		//random_time = 5; //DEBUG MODE, monsters will appear for 5 seconds
		
		static long time_seed = 100;
		time_seed  = (((time_seed * 214013L + 2531011L) >> 16) & 32767);
		combined_time_values = time_seed + actual_time;
		random_time = (combined_time_values % max_monster_fun); // extras should appear for max. 'max_monster_fun' seconds
    	APP_LOG(APP_LOG_LEVEL_DEBUG, "extras will be visible for %d seconds (~%d minutes)...", random_time, random_time/60);

		}

	// AM indicator (TARDIS lights)
 	if (current_time->tm_hour < 12) {
 		layer_set_hidden(bitmap_layer_get_layer(ampm_digits_layers[0]), false);
		set_container_image(&ampm_digits_images[0], ampm_digits_layers[0], AMPM_IMAGE_RESOURCE_IDS[0], GPoint(129, 80));
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "AM/PM yes");
	} 
	else {
		layer_set_hidden(bitmap_layer_get_layer(ampm_digits_layers[0]), true);
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "AM/PM yes");
	}
	
	// dithering if the date
	   	layer_set_hidden(bitmap_layer_get_layer(extras_digits_layers[0]), false);
		bitmap_layer_set_compositing_mode(extras_digits_layers[0], GCompOpOr);
		set_container_image(&extras_digits_images[0], extras_digits_layers[0], EXTRAS_WHITE_IMAGE_RESOURCE_IDS[NUM_EXTRAS], GPoint(113, 113)); // "NUM_EXTRAS" also defines the last spot of the resource list => RESOURCE_ID_IMAGE_DATE_DITHER_WHITE
	   	layer_set_hidden(bitmap_layer_get_layer(extras_digits_layers[1]), false);
    	bitmap_layer_set_compositing_mode(extras_digits_layers[1], GCompOpClear);
		set_container_image(&extras_digits_images[1], extras_digits_layers[1], EXTRAS_BLACK_IMAGE_RESOURCE_IDS[NUM_EXTRAS], GPoint(113, 113)); // "NUM_EXTRAS" also defines the last spot of the resource list => RESOURCE_ID_IMAGE_DATE_DITHER_BLACK
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "date shadow yes");
	
  	// the regeneration appears on every hour change for 2 seconds (and also at 8:30 and 20:30 (=appearance of the "WAR DOCTOR"))
 	if ( (((current_time->tm_min) == 59) && ((current_time->tm_sec) >= 58)) || (   (((current_time->tm_hour) == 8) || ((current_time->tm_hour) == 20))  && ((current_time->tm_min) == 29) && ((current_time->tm_sec) >= 58)) ) {
		regeneration = true;
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "set regeneration TRUE");
 	}
  	else {
  		regeneration = false;
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "set regeneration FALSE");
	}
	
	/* ------------------------------------------------- NEW --------------------------------------------------- */
	
			layer_set_hidden(bitmap_layer_get_layer(extras_digits_layers[2]), false);
			bitmap_layer_set_compositing_mode(extras_digits_layers[2], GCompOpOr);
			set_container_image(&extras_digits_images[2], extras_digits_layers[2], EXTRAS_WHITE_IMAGE_RESOURCE_IDS[random_number], GPoint(95, 72));	//95, 72
	    	//APP_LOG(APP_LOG_LEVEL_DEBUG, "EXTRAS_WHITE_IMAGE_RESOURCE_IDS[%d] gezeichnet", random_number);
   			layer_set_hidden(bitmap_layer_get_layer(extras_digits_layers[3]), false);
    		bitmap_layer_set_compositing_mode(extras_digits_layers[3], GCompOpClear);
			set_container_image(&extras_digits_images[3], extras_digits_layers[3], EXTRAS_BLACK_IMAGE_RESOURCE_IDS[random_number], GPoint(95, 72));	
    		//APP_LOG(APP_LOG_LEVEL_DEBUG, "EXTRAS_BLACK_IMAGE_RESOURCE_IDS[%d] gezeichnet", random_number);

	/* --------------------------------------------------------------------------------------------------------- */

	
}

//--BATTERY LAYER
void battery_layer_update_callback(Layer *layer, GContext *ctx) {

  	//graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  	//graphics_context_set_stroke_color(ctx, GColorBlack);
  	//graphics_context_set_fill_color(ctx, GColorBlack);

	if (!battery_plugged) {
		bitmap_layer_set_compositing_mode(bat_digits_layers[0], GCompOpOr);
  		set_container_image(&bat_digits_images[0], bat_digits_layers[0], BATTERY_WHITE_IMAGE_RESOURCE_IDS[battery_level/10], GPoint(83, 0));
  	   	bitmap_layer_set_compositing_mode(bat_digits_layers[1], GCompOpClear);
		set_container_image(&bat_digits_images[1], bat_digits_layers[1], BATTERY_BLACK_IMAGE_RESOURCE_IDS[battery_level/10], GPoint(83, 0));
	}
	else {	
		bitmap_layer_set_compositing_mode(bat_digits_layers[0], GCompOpOr);
  		set_container_image(&bat_digits_images[0], bat_digits_layers[0], BATTERY_WHITE_IMAGE_RESOURCE_IDS[11], GPoint(83, 0));
  	    bitmap_layer_set_compositing_mode(bat_digits_layers[1], GCompOpClear);
		set_container_image(&bat_digits_images[1], bat_digits_layers[1], BATTERY_BLACK_IMAGE_RESOURCE_IDS[11], GPoint(83, 0));
 	}
}

void battery_state_handler(BatteryChargeState charge) {
	
	battery_level = charge.charge_percent;
   	battery_plugged = charge.is_plugged;
   	layer_mark_dirty(battery_layer);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
/*	
  	if ( (((tick_time->tm_hour) == 8) || ((tick_time->tm_hour) == 20)) && ((tick_time->tm_min) >= 30) ) {
		
		nowardoc = false;
	}
	else {
		
		nowardoc = true;
	}
*/	
  	update_display(tick_time);
			
  	static char time_text[]   = 	":00";
  	static char date_text[]   = 	"XXX\n00";

  	char *time_format;
  	char *date_format;
  
  	if (regeneration == false) {
  		time_format = ":%M";
  		date_format = "%b%n%e";
 	}
  	else {
  		time_format = "";
  		date_format = "";		
 	}

  	strftime(time_text, sizeof(time_text), time_format, tick_time);
  	strftime(date_text, sizeof(date_text), date_format, tick_time);

  	//Change the TextLayer text to show the new time!
  	text_layer_set_text(text_time_layer, time_text);
  	text_layer_set_text(text_date_layer, date_text);

  	timer_handle = app_timer_register(100 /* milliseconds */, handle_timer, (void *)COOKIE_MY_TIMER);
}

void window_unload(Window *window){
	
  	for (int i = 0; i < TOTAL_DOC_DIGITS; i++) {
    	layer_remove_from_parent(bitmap_layer_get_layer(doc_digits_layers[i]));
    	gbitmap_destroy(doc_digits_images[i]);
    	bitmap_layer_destroy(doc_digits_layers[i]);
 	}
  	for (int i = 0; i < TOTAL_EXTRAS_DIGITS; i++) {
    	layer_remove_from_parent(bitmap_layer_get_layer(extras_digits_layers[i]));
    	gbitmap_destroy(extras_digits_images[i]);
    	bitmap_layer_destroy(extras_digits_layers[i]);
 	}
	for (int i = 0; i < TOTAL_BAT_DIGITS; i++) {
    	layer_remove_from_parent(bitmap_layer_get_layer(bat_digits_layers[i]));
    	gbitmap_destroy(bat_digits_images[i]);
    	bitmap_layer_destroy(bat_digits_layers[i]);
	}
  	for (int i = 0; i < TOTAL_AMPM_DIGITS; i++) {
    	layer_remove_from_parent(bitmap_layer_get_layer(ampm_digits_layers[i]));
    	gbitmap_destroy(ampm_digits_images[i]);
    	bitmap_layer_destroy(ampm_digits_layers[i]);
 	}
   	for (int i = 0; i < TOTAL_REGEN_DIGITS; i++) {
    	layer_remove_from_parent(bitmap_layer_get_layer(regen_digits_layers[i]));
    	gbitmap_destroy(regen_digits_images[i]);
    	bitmap_layer_destroy(regen_digits_layers[i]);
 	}
}

static void init(void) {
	
 	window = window_create();

 	if (window == NULL) {
     	APP_LOG(APP_LOG_LEVEL_WARNING, "OOM: couldn't allocate window!?");
     	return;
	}
 	window_stack_push(window, true /* Animated */);
 	Layer *window_layer = window_get_root_layer(window);

  	GRect dummy_frame = { {0, 0}, {0, 0} };
  	for (int i = 0; i < TOTAL_DOC_DIGITS; ++i) {
    	doc_digits_layers[i] = bitmap_layer_create(dummy_frame);
    	layer_add_child(window_layer, bitmap_layer_get_layer(doc_digits_layers[i]));
 	}
	
  	time_font  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_24));
  	date_font  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_16));

  	text_time_layer = text_layer_create(GRect(80, 0, 64, 168));
  	text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  	text_layer_set_text_color(text_time_layer, GColorWhite);
  	text_layer_set_background_color(text_time_layer, GColorClear);
  	text_layer_set_font(text_time_layer, time_font);
  	layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  	text_date_layer = text_layer_create(GRect(83, 110, 60, 168)); // y = 109 (bei Linie!)
  	text_layer_set_text_alignment(text_date_layer, GTextAlignmentRight);
  	text_layer_set_text_color(text_date_layer, GColorBlack);
  	text_layer_set_background_color(text_date_layer, GColorClear);
 	text_layer_set_font(text_date_layer, date_font);
  	layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

	for (int i = 0; i < TOTAL_EXTRAS_DIGITS; ++i) {
    	extras_digits_layers[i] = bitmap_layer_create(dummy_frame);
    	layer_add_child(window_layer, bitmap_layer_get_layer(extras_digits_layers[i]));
 	}
  	for (int i = 0; i < TOTAL_AMPM_DIGITS; ++i) {
    	ampm_digits_layers[i] = bitmap_layer_create(dummy_frame);
    	layer_add_child(window_layer, bitmap_layer_get_layer(ampm_digits_layers[i]));
 	 }
  	for (int i = 0; i < TOTAL_BAT_DIGITS; ++i) {
    	bat_digits_layers[i] = bitmap_layer_create(dummy_frame);
    	layer_add_child(window_layer, bitmap_layer_get_layer(bat_digits_layers[i]));
 	}
  	for (int i = 0; i < TOTAL_REGEN_DIGITS; ++i) {
    	regen_digits_layers[i] = bitmap_layer_create(dummy_frame);
    	layer_add_child(window_layer, bitmap_layer_get_layer(regen_digits_layers[i]));
 	}

  	//BATTERY
  	BatteryChargeState initial = battery_state_service_peek();
  	battery_level = initial.charge_percent;
  	battery_plugged = initial.is_plugged;
  	battery_layer = layer_create(GRect(0,0,144,168)); //SIZE = 144*168 (full screen, because it doesn't really matter anyways)
  	layer_set_update_proc(battery_layer, &battery_layer_update_callback);
  	layer_add_child(window_get_root_layer(window), battery_layer);

  	handle_bluetooth(bluetooth_connection_service_peek());

 	// Avoids a blank screen on watch start.
 	time_t now = time(NULL);
 	struct tm *tick_time = localtime(&now);
    handle_minute_tick(tick_time, SECOND_UNIT);
    tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);

 	update_display(tick_time);

	window_set_window_handlers(window, (WindowHandlers) {
  		//.load = window_load,
  		.unload = window_unload,
    });

	start = false;
}

static void deinit(void) {
	
  	text_layer_destroy(text_time_layer);
  	fonts_unload_custom_font(time_font);
  	text_layer_destroy(text_date_layer);
  	fonts_unload_custom_font(date_font);

 	bluetooth_connection_service_unsubscribe();
 	battery_state_service_unsubscribe();
 	layer_destroy(battery_layer);
	
  	window_destroy(window);
}

int main(void) {

 	init();
 	//tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick); //changed from MINUTE_UNIT
 	bluetooth_connection_service_subscribe(&handle_bluetooth); //BT
 	battery_state_service_subscribe (&battery_state_handler);
 	app_event_loop();
 	deinit();
}
