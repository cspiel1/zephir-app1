/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
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


int main(void)
{
	struct timeval tv;
	int err;

	err = libre_init();
	if (err) {
		warning("Could not init libre\n");
		return err;
	}

	err  = conf_configure();
	err |= extern_baresip_config(conf_cur());
	if (err) {
		warning("Could not configure baresip.\n");
		return err;
	}

	while (1) {
		int res = gettimeofday(&tv, NULL);
		time_t now = time(NULL);
		struct tm tm;
		localtime_r(&now, &tm);

		if (res < 0) {
			printf("Error in gettimeofday(): %d\n", errno);
			return 1;
		}

		re_printf("gettimeofday(): HI(tv_sec)=%d, LO(tv_sec)=%d, "
		       "tv_usec=%d\n\t%s\n", (unsigned int)(tv.tv_sec >> 32),
		       (unsigned int)tv.tv_sec, (unsigned int)tv.tv_usec,
		       asctime(&tm));
		sleep(1);
	}
}
