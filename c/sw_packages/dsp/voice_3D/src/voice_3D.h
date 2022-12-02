/*
 * file : VOICE_3D.h
 *
 */

#ifndef _VOICE_3D_H
#define _VOICE_3D_H

struct VOICE_3D_Instance_t {
	real_t center_gain;
	real_t side_gain;
	real_t _3D_gain;
	real_t C;
	real_t D;
	uint8_t enable_3D;
	uint8_t enable_voice;
};

#endif
