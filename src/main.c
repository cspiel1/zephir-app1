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
#include <zephyr/net/net_config.h>

#include <re.h>
#include <baresip.h>


int extern_baresip_config(struct conf *conf)
{
	conf_set(conf, "sip_listen", "0.0.0.0:5060");
	conf_set(conf, "rtp_stats", "no");
	conf_set(conf, "rtcp_enable", "no");
	conf_set(conf, "module", "opus");
	conf_set(conf, "module", "g711");
	conf_set(conf, "module", "auwm8960\n");
	conf_set(conf, "module", "auresamp\n");
	conf_set(conf, "module_app", "menu\n");

	conf_set(conf, "audio_player", "auwm8960\n");
	conf_set(conf, "audio_source", "auwm8960\n");
	conf_set(conf, "audio_alert", "auwm8960\n");
/*        conf_set(conf, "audio_channels", "2\n");*/
/*        conf_set(conf, "audio_srate", "48000\n");*/

	conf_set(conf, "opus_stereo", "yes\n");
	conf_set(conf, "opus_sprop_stereo", "yes\n");

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

	ua_alloc(NULL, "<sip:zephyr@localhost>;answermode=auto");
	printf("Starting main loop now ...\n");
	err = re_main(NULL);
	printf("Main loop left\n");
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


static void receiver_cb(struct net_mgmt_event_callback *cb,
			uint32_t nm_event, struct net_if *iface)
{
	printf("%s:%d HUUUUUU %u\n", __func__, __LINE__, nm_event);
	if (nm_event == NET_EVENT_IPV4_ADDR_ADD) {
		printf("Got IP:\n");
	}
}


int main(void)
{
	struct net_mgmt_event_callback rx_cb;
	pthread_t tid;

	(void) pthread_create(&tid, NULL, baresip_thread, NULL);
	net_mgmt_init_event_callback(&rx_cb, receiver_cb,
				     NET_EVENT_IPV4_ADDR_ADD);
	net_mgmt_add_event_callback(&rx_cb);

	while (1) {
		sleep(1);
	}
}
