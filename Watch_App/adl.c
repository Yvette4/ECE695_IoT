#include <tizen.h> // standard header from the template
#include <service_app.h> // standard header from the template

// headers that will be needed for our service:
#include <sensor.h>
#include <player.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlog.h> // for logging purposes

#include "adl.h"

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

static void
accelerometer_event_callback(sensor_h sensor, sensor_event_s *event, void *user_data)
{
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    if(type == SENSOR_ACCELEROMETER)
    {
    	dlog_print(DLOG_INFO, LOG_TAG, "accelerometer: %f, %f, %f", event->values[0], event->values[1], event->values[2]);
    }
}

static void
gyroscope_event_callback(sensor_h sensor, sensor_event_s *event, void *user_data)
{
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    if(type == SENSOR_GYROSCOPE)
    {
    	dlog_print(DLOG_INFO, LOG_TAG, "GYROSCOPE: %f, %f, %f", event->values[0], event->values[1], event->values[2]);
    }
}

static void
_btn_start_collection_cb(void *data, Evas_Object *btn, void *ev EINA_UNUSED)
{
	dlog_print(DLOG_INFO, LOG_TAG, " YVETTE: started data collection - clicked button");

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

	// start collecting accelerometer data
	if (sensor_get_default_sensor(SENSOR_ACCELEROMETER, &ad->accelerometer) == SENSOR_ERROR_NONE)
	{
	    if (sensor_create_listener(ad->accelerometer, &ad->acc_listener) == SENSOR_ERROR_NONE
	        && sensor_listener_set_event_cb(ad->acc_listener, 100, accelerometer_event_callback, NULL) == SENSOR_ERROR_NONE)
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
	        && sensor_listener_set_event_cb(ad->gyro_listener, 100, gyroscope_event_callback, NULL) == SENSOR_ERROR_NONE)
	    {
	        if (sensor_listener_start(ad->gyro_listener) == SENSOR_ERROR_NONE)
	        {
	        	dlog_print(DLOG_INFO, LOG_TAG, "gyroscope sensor listener started!");
	        }
	    }
	}

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

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	dlog_print(DLOG_INFO, LOG_TAG, " YVETTE: inside app_control");

////sensor event callback implementation
//void accelerometer_event_callback(sensor_h sensor, sensor_event_s *event, void *user_data)
//{
//    sensor_type_e type;
//    sensor_get_type(sensor, &type);
//    if(type == SENSOR_ACCELEROMETER)
//    {
//        LOGI("accelerometer: %f, %f, %f", event->values[0], event->values[1], event->values[2]);
//    }
//}
//
////You must declare sensor handle and sensor listener handle.
//sensor_h sensor;
//sensor_listener_h listener;
//
////Starting sensor listener
//sensor_type_e type = SENSOR_ACCELEROMETER;
//
//if (sensor_get_default_sensor(type, &sensor) == SENSOR_ERROR_NONE)
//{
//    if (sensor_create_listener(sensor, &listener) == SENSOR_ERROR_NONE
//        && sensor_listener_set_event_cb(listener, 100, accelerometer_event_callback, NULL) == SENSOR_ERROR_NONE)
//    {
//        if (sensor_listener_start(listener) == SENSOR_ERROR_NONE)
//        {
//            LOGI("sensor listener started!");
//        }
//    }
//}
//
////Stopping sensor listener
//int err = sensor_listener_stop(listener);
////handle error codes as you wish
//err = sensor_destroy_listener(listener);
	/* Handle the launch request. */
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
