/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <re.h>
#include <baresip.h>


int extern_baresip_config(struct conf *conf)
{
	conf_set(conf, "sip_listen", "0.0.0.0:5060");
	conf_set(conf, "rtp_stats", "no");
	conf_set(conf, "rtcp_enable", "no");
	conf_set(conf, "module", "g711");
	conf_set(conf, "module", "auwm8960\n");
	conf_set(conf, "module_app", "menu\n");

	conf_set(conf, "audio_player", "auwm8960\n");
	conf_set(conf, "audio_source", "auwm8960\n");
	conf_set(conf, "audio_alert", "auwm8960\n");
	conf_set(conf, "audio_channels", "1\n");
	conf_set(conf, "audio_srate", "8000\n");

	return config_parse_conf(conf_config(), conf);
}


static void ua_exit_handler(void *arg)
{
	(void)arg;
	debug("ua exited -- stopping main runloop\n");

	/* The main run-loop can be stopped now */
	re_cancel();
}


static void *baresip_thread(void *arg)
{
	int err;

	err = libre_init();
	if (err) {
		warning("Could not init libre\n");
		return NULL;
	}

	err  = conf_configure();
	err |= extern_baresip_config(conf_cur());
	if (err) {
		warning("Could not configure baresip.\n");
		return NULL;
	}

	err = baresip_init(conf_config());
	if (err) {
		warning("Could not initialize baresip\n");
		goto out;
	}

	err = ua_init(NULL, true, true, false);
	if (err)
		goto out;

	uag_set_exit_handler(ua_exit_handler, NULL);

	/* Load modules */
	err = conf_modules();
	if (err)
		goto out;

	err = re_main(NULL);

out:

	ua_stop_all(true);

	ua_close();
	conf_close();

	baresip_close();

	info("main: unloading modules\n");
	mod_close();

	libre_close();

	// Check for memory leaks
	tmr_debug();
	mem_debug();
	return NULL;
}


int main(void)
{
	pthread_t tid;

	(void) pthread_create(&tid, NULL, baresip_thread, NULL);
	while (1) {
		sleep(1);
	}
}
