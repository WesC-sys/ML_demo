/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>

#include <caf/events/click_event.h>
#include "ml_app_mode_event.h"

#define MODULE ml_app_mode
#include <caf/events/module_state_event.h>

#include <zephyr/drivers/i2c.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(MODULE, CONFIG_ML_APP_MODE_LOG_LEVEL);

#ifdef CONFIG_ML_APP_MODE_CONTROL_BUTTON_ID
  #define MODE_CONTROL			true
  #define MODE_CONTROL_BUTTON_ID	CONFIG_ML_APP_MODE_CONTROL_BUTTON_ID
#else
  #define MODE_CONTROL			false
  #define MODE_CONTROL_BUTTON_ID	0
#endif /* CONFIG_ML_APP_MODE_CONTROL_BUTTON_ID */

#define I2C1_NODE DT_NODELABEL(relay)


static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C1_NODE);


static enum ml_app_mode mode = IS_ENABLED(CONFIG_ML_APP_ML_RUNNER) ?
			     ML_APP_MODE_MODEL_RUNNING : ML_APP_MODE_DATA_FORWARDING;


static void broadcast_mode(void)
{
	struct ml_app_mode_event *event = new_ml_app_mode_event();

	event->mode = mode;
	APP_EVENT_SUBMIT(event);
}


static bool handle_click_event(const struct click_event *event)
{
	if ((event->key_id == MODE_CONTROL_BUTTON_ID) &&
		(event->click == CLICK_SHORT)) {
		uint8_t enable[1] = {0x01};
		i2c_write_dt(&dev_i2c, enable, sizeof(enable));
	}
	else if ((event->key_id == MODE_CONTROL_BUTTON_ID) &&
			(event->click == CLICK_DOUBLE)) {
		uint8_t disable[1] = {0x00};
		i2c_write_dt(&dev_i2c, disable, sizeof(disable));
	}
}

static bool app_event_handler(const struct app_event_header *aeh)
{
	if (MODE_CONTROL &&
	    is_click_event(aeh)) {
		return handle_click_event(cast_click_event(aeh));
	}

	if (is_module_state_event(aeh)) {
		const struct module_state_event *event = cast_module_state_event(aeh);

		if (check_state(event, MODULE_ID(main), MODULE_STATE_READY)) {
			static bool initialized;

			__ASSERT_NO_MSG(!initialized);
			module_set_state(MODULE_STATE_READY);
			initialized = true;
			broadcast_mode();
		}

		return false;
	}

	/* If event is unhandled, unsubscribe. */
	__ASSERT_NO_MSG(false);

	return false;
}

APP_EVENT_LISTENER(MODULE, app_event_handler);
APP_EVENT_SUBSCRIBE(MODULE, module_state_event);
#if MODE_CONTROL
APP_EVENT_SUBSCRIBE(MODULE, click_event);
#endif /* MODE_CONTROL */
