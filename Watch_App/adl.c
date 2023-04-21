#include <tizen.h> // standard header from the template
#include <service_app.h> // standard header from the template
#include <recorder.h> // for audio recording

// headers that will be needed for our service:
#include <sensor.h>
#include <player.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlog.h> // for logging purposes

#include "adl.h"

#define FILENAME_PREFIX "AUDIO"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	Evas_Object *button;

	// Sensor data
	sensor_h accelerometer;
	sensor_h gyroscope;
	sensor_listener_h acc_listener;
	sensor_listener_h gyro_listener;

	// Audio recording
	recorder_h g_recorder;

    /* application acceleration data */
	float aX;
	float aY;
	float aZ;

    /* application gyroscope data */
	float rX;
	float rY;
	float rZ;

	// Timestamps
	float acc_timestamp;	// accelerometer
	float start_t;
	float gyro_timestamp;	// gyroscope
	float start_t_g;
	char sys_time[30];		// general (system)
	struct tm * timeinfo;

	/* Other objects */
	FILE * fp;

    /* array containing all the data (to be saved in a file) */
    float all_data[30000][8];
    int i;
    int j;

} appdata_s;

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}


static void write_data(void *data)
{
	appdata_s *ad = (appdata_s*)data;

	fprintf(ad->fp, "%s\n", " \"watch_data\": [ ");

	char data_string[2048];

	for( int i=0; i<(ad->i); ++i){

		sprintf(data_string, "{ \"timestamp\": %f, \"aX\": %f , \"aY\": %f, \"aZ\": %f, \"timestamp_g\": %f, \"rX\": %f , \"rY\": %f, \"rZ\": %f },",
				(ad->all_data)[i][0], (ad->all_data)[i][1], (ad->all_data)[i][2], (ad->all_data)[i][3],
				(ad->all_data)[i][4], (ad->all_data)[i][5], (ad->all_data)[i][6], (ad->all_data)[i][7] );
		fprintf(ad->fp, "%s\n", data_string);

	}
	fprintf(ad->fp, "%s\n", "], ");
}


/* Open and save Text file */
/* ----------------------------------------------- */
static void save_data(void *data){

	appdata_s *ad = (appdata_s*)data;

	// This is to delete all the previous files
	//system("exec rm -r /opt/usr/media/Documents/*");

	char filename[50];
	char current_time_str[50];

	// date formating
	sprintf(current_time_str, "%d-%d-%d_%d-%d-%d",
			(ad->timeinfo)->tm_year + 1900,(ad->timeinfo)->tm_mon + 1,
			(ad->timeinfo)->tm_mday,(ad->timeinfo)->tm_hour,
			(ad->timeinfo)->tm_min,(ad->timeinfo)->tm_sec);

//	sprintf(filename, "/opt/usr/media/Downloads/data-%s.txt", current_time_str);
	sprintf(filename, "/opt/usr/Data/data-%s.txt", current_time_str);
	ad->fp = fopen (filename, "w+");


	fprintf(ad->fp, "%s\n", "{ 	\"smartwatch\": { ");
	fprintf(ad->fp, " \"time_start\": %s ,", current_time_str);	// save current time
	write_data(ad);													// save data
	fprintf(ad->fp, "%s\n", "} ");
	fprintf(ad->fp, "%s\n", "} ");

	fclose(ad->fp);													// close file
}

static void
accelerometer_event_callback(sensor_h sensor, sensor_event_s *event, void *user_data)
{
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    if(type == SENSOR_ACCELEROMETER)
    {
    	dlog_print(DLOG_INFO, LOG_TAG, "YVETTE acc: %f, %f, %f", event->values[0], event->values[1], event->values[2]);

    	// date formating
//    	appdata_s * ad = (appdata_s *)user_data;
//    	char current_time_str[50];
//    	sprintf(current_time_str, "%d-%d-%d_%d-%d-%d",
//    			(ad->timeinfo)->tm_year + 1900,(ad->timeinfo)->tm_mon + 1,
//    			(ad->timeinfo)->tm_mday,(ad->timeinfo)->tm_hour,
//    			(ad->timeinfo)->tm_min,(ad->timeinfo)->tm_sec);
//    	fprintf(ad->fp, " \"time_start\": %s ,", current_time_str);	// save current time

//		appdata_s * ad = (appdata_s *)user_data;
//
//		/*	Save the very first timestamp collected
//		*/
//		if (ad->i == 1){
//			ad->start_t = event->timestamp;
//		}
//
//		/*	Acceleration for each axis:
//		*/
//		ad->acc_timestamp = (event->timestamp) - (ad->start_t);
//		ad->aX = event->values[0];
//		ad->aY = event->values[1];
//		ad->aZ = event->values[2];
//
//
//		/*	Purely visual => shows timestamp to user
//			 may affect performance...
//		*/
//		// float to string formatting
//		char timestamp_str[40];
//		sprintf(timestamp_str, "%f",ad->acc_timestamp);
//
////		char * separator = "."; 	// get rid of the .0000 part (by finding ".")
////		char * b = strtok(timestamp_str,separator);
//		sprintf(timestamp_str,"%s\n", timestamp_str);
//
//		// save the string as an int (solve the .0000 problem)
//		//ad->timestamp = atoi(timestamp_str);
//		//sprintf(timestamp_str, "%f",ad->timestamp);
//
//		//char* time = eina_counter_dump(ad->counter);
////		elm_object_text_set(ad->label, timestamp_str);
//
//
//		/*	save data in the all_data array
//		*/
//		(ad->all_data)[ad->i][0] = ad->acc_timestamp;
//		(ad->all_data)[ad->i][1] = ad->aX;
//		(ad->all_data)[ad->i][2] = ad->aY;
//		(ad->all_data)[ad->i][3] = ad->aZ;
//
//		/* increase counter */
//		ad->i += 1;
    }
}

static void
gyroscope_event_callback(sensor_h sensor, sensor_event_s *event, void *user_data)
{
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    if(type == SENSOR_GYROSCOPE)
    {
    	dlog_print(DLOG_INFO, LOG_TAG, "YVETTE gyro: %f, %f, %f", event->values[0], event->values[1], event->values[2]);

//		/*	Save the very first timestamp collected
//		*/
//    	appdata_s * ad = (appdata_s *)user_data;
//		if (ad->j == 1){
//			ad->start_t_g = event->timestamp;
//		}
//
//		/*	Acceleration for each axis:
//		*/
//		ad->gyro_timestamp = (event->timestamp) - (ad->start_t_g);
//		ad->rX = event->values[0];
//		ad->rY = event->values[1];
//		ad->rZ = event->values[2];
//
//		/*	save data in the all_data array
//		*/
//		(ad->all_data)[ad->j][4] = ad->gyro_timestamp;
//		(ad->all_data)[ad->j][5] = ad->rX;
//		(ad->all_data)[ad->j][6] = ad->rY;
//		(ad->all_data)[ad->j][7] = ad->rZ;
//
//		/* increase counter */
//		ad->j += 1;
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// AUDIO RECORDING FUNCTIONS
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
static void
_state_changed_cb(recorder_state_e previous, recorder_state_e current, bool by_policy, void *user_data)
{
    dlog_print(DLOG_INFO, LOG_TAG, "_recorder_state_changed_cb (prev: %d, curr: %d)\n", previous, current);
}

static void
_recorder_recording_limit_reached_cb(recorder_recording_limit_type_e type, void *user_data)
{
    dlog_print(DLOG_DEBUG, LOG_TAG, "Recording limit reached: %d\n", type);
}

/* Check the audio recorder state */
static bool
_recorder_expect_state(recorder_h recorder, recorder_state_e expected_state)
{
    recorder_state_e state;
    int error_code = recorder_get_state(recorder, &state);

    dlog_print(DLOG_INFO, LOG_TAG, "recorder state = %d, expected recorder state = %d", state, expected_state);
    if (state == expected_state)
        return 1;

    return 0;
}

static void
_btn_start_collection_cb(void *data, Evas_Object *btn, void *ev EINA_UNUSED)
{
	appdata_s *ad = data;

	if ( strcmp(elm_object_part_text_get(btn, NULL), "Start Collection") == 0 ) // compare button string and "Start"
	{

		/* Start the accelerometer and data collection */
		/* ------------------------------------------- */

//		// reset data array
//		ad->i = 0;
//		memset(ad->all_data, 0.0f, sizeof(ad->all_data));

//		// capture current time
//		time_t rawtime;
//		time (&rawtime);
//		ad->timeinfo = localtime (&rawtime);
//
//		char filename[50];
//		char current_time_str[50];
//
//		// date formating
//		sprintf(current_time_str, "%d-%d-%d_%d-%d-%d",
//				(ad->timeinfo)->tm_year + 1900,(ad->timeinfo)->tm_mon + 1,
//				(ad->timeinfo)->tm_mday,(ad->timeinfo)->tm_hour,
//				(ad->timeinfo)->tm_min,(ad->timeinfo)->tm_sec);
//
//////		sprintf(filename, "/opt/usr/media/Downloads/data-%s.txt", current_time_str);
//		sprintf(filename, "/opt/usr/Data/data-%s.txt", current_time_str);
//		ad->fp = fopen (filename, "w+");

//		// float to string formatting
//		char timestamp_str[20];
//		sprintf(timestamp_str, "%f",ad->start_t);
////		char * separator = ".";
////		char * b = strtok(timestamp_str,separator);
//		sprintf(timestamp_str,"%s\n", timestamp_str);


		dlog_print(DLOG_INFO, LOG_TAG, " YVETTE: started data collection - clicked button");
		// start collecting accelerometer data
		if (sensor_get_default_sensor(SENSOR_ACCELEROMETER, &ad->accelerometer) == SENSOR_ERROR_NONE)
		{
			// 20 ms for 50 Hz (similar to what the paper had)
		    if (sensor_create_listener(ad->accelerometer, &ad->acc_listener) == SENSOR_ERROR_NONE
		        && sensor_listener_set_event_cb(ad->acc_listener, 20, accelerometer_event_callback, NULL) == SENSOR_ERROR_NONE)
		    {
		        if (sensor_listener_start(ad->acc_listener) == SENSOR_ERROR_NONE)
		        {
		        	dlog_print(DLOG_INFO, LOG_TAG, "accelerometer sensor listener started!");
		        }
		    }
		}

		// start collecting gyroscope data
		if (sensor_get_default_sensor(SENSOR_GYROSCOPE, &ad->gyroscope) == SENSOR_ERROR_NONE)
		{
		    if (sensor_create_listener(ad->gyroscope, &ad->gyro_listener) == SENSOR_ERROR_NONE
		        && sensor_listener_set_event_cb(ad->gyro_listener, 20, gyroscope_event_callback, NULL) == SENSOR_ERROR_NONE)
		    {
		        if (sensor_listener_start(ad->gyro_listener) == SENSOR_ERROR_NONE)
		        {
		        	dlog_print(DLOG_INFO, LOG_TAG, "gyroscope sensor listener started!");
		        }
		    }
		}

//		// Start the audio recording
//		int error_code = recorder_prepare(ad->g_recorder);
//		error_code = recorder_start(ad->g_recorder);

		elm_object_text_set(btn,"Stop Collection");
	}
	else
	{

		// Stop data collection
	   int error = sensor_listener_stop(ad->acc_listener); // unset the sensor callback function
		   error = sensor_listener_stop(ad->gyro_listener);

//		/* Stop the recorder and save the recorded data to a file */
//		int error_code = recorder_pause(ad->g_recorder);
//		if (_recorder_expect_state(ad->g_recorder, RECORDER_STATE_RECORDING) || _recorder_expect_state(ad->g_recorder, RECORDER_STATE_PAUSED)) {
//		    error_code = recorder_commit(ad->g_recorder);
//		    if (error_code != RECORDER_ERROR_NONE)
//		    {
//		        dlog_print(DLOG_ERROR, LOG_TAG, "error code = %d", error_code);
//		    }
//		}
//		recorder_unprepare(ad->g_recorder);

	   // change button and label value
	   elm_object_text_set(btn,"Start Collection");
//	   elm_object_text_set(ad->label2, "00:00");

//	   fclose(ad->fp);

//	   save_data(ad); // save data into file
	}

}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Label */ // YVETTE - label is not showing up...
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */
	ad->label = elm_label_add(ad->conform);
	elm_object_text_set(ad->label, "<align=center>ADL Testing</align>");
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->label);

	/* Create a button to start data collection */
	ad->button = elm_button_add(ad->win);
	elm_object_text_set(ad->button, "Start Collection");
	elm_object_style_set(ad->button, "bottom");
	evas_object_smart_callback_add(ad->button, "clicked", _btn_start_collection_cb, &ad);
	elm_object_content_set(ad->conform, ad->button);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	bool sensor_supported = false;
	if (sensor_is_supported(SENSOR_ACCELEROMETER, &sensor_supported) != SENSOR_ERROR_NONE || sensor_supported == false)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "Accelerometer not supported! Service is useless, exiting...");
		ui_app_exit();
		return false;
	}


//	/* Create the audio recorder handle */
//	int error_code = recorder_create_audiorecorder(&ad->g_recorder);
//	if (error_code != RECORDER_ERROR_NONE)
//	    dlog_print(DLOG_ERROR, LOG_TAG, "fail to create an Audio Recorder: error code = %d", error_code);
//
//	error_code = recorder_set_state_changed_cb(ad->g_recorder, _state_changed_cb, NULL);
//
//	error_code = recorder_set_recording_limit_reached_cb(ad->g_recorder, _recorder_recording_limit_reached_cb, NULL);
//
//	struct tm localtime = {0};
//	time_t rawtime = time(NULL);
//	char filename[256] = {'\0'};
//	size_t size;
//
//	/* Set the audio encoder */
//	error_code = recorder_set_audio_encoder(ad->g_recorder, RECORDER_AUDIO_CODEC_AAC);
//
//	/* Set the file format according to the audio encoder */
//	error_code = recorder_set_file_format(ad->g_recorder, RECORDER_FILE_FORMAT_3GP);
//
//	/* Create the file name */
//	if (localtime_r(&rawtime, &localtime) != NULL) {
//	    size = snprintf(filename, sizeof(filename), "%s/%s-%04i-%02i-%02i_%02i:%02i:%02i.3gp",
//	                    app_get_data_path(), FILENAME_PREFIX,
//	                    localtime.tm_year + 1900, localtime.tm_mon + 1, localtime.tm_mday,
//	                    localtime.tm_hour, localtime.tm_min, localtime.tm_sec);
//	}
//	else
//	{
//		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create file name for audio recorder");
//	    /* Error handling */
//	}
//
//	/* Set the full path and file name */
//	/* Set the file name according to the file format */
//	error_code = recorder_set_filename(ad->g_recorder, filename);
//
//	/* Set the maximum file size to 1024 (kB) */
//	error_code = recorder_attr_set_size_limit(ad->g_recorder, 1024);
//
//	/* Set the audio encoder bitrate */
//	error_code = recorder_attr_set_audio_encoder_bitrate(ad->g_recorder, 28800);
//
//	/* Set the audio device to microphone */
//	error_code = recorder_attr_set_audio_device(ad->g_recorder, RECORDER_AUDIO_DEVICE_MIC);
//
//	/* Set the audio sample rate */
//	error_code = recorder_attr_set_audio_samplerate(ad->g_recorder, 44100);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
